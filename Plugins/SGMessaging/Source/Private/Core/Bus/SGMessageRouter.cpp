// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Bus/SGMessageRouter.h"
#include "Core/Interface/ISGMessagingModule.h"
#include "HAL/PlatformProcess.h"
#include "Core/Bus/SGMessageDispatchTask.h"
#include "Core/Interface/ISGMessageSubscription.h"
#include "Core/Interface/ISGMessageReceiver.h"
#include "Core/Interface/ISGMessageInterceptor.h"
#include "Core/Interface/ISGMessageBusListener.h"
#include "Core/Settings/SGMessagingSettings.h"


/* FSGMessageRouter structors
 *****************************************************************************/

FSGMessageRouter::FSGMessageRouter()
	: DelayedMessagesSequence(0)
	  , Stopping(false)
	  , Tracer(MakeShared<FSGMessageTracer, ESPMode::ThreadSafe>())
	  , bAllowDelayedMessaging(false)
{
	ActiveSubscriptions.FindOrAdd(NAME_All);
	WorkEvent = FPlatformProcess::GetSynchEventFromPool();

	if (const auto SGMessagingSettings = GetMutableDefault<USGMessagingSettings>())
	{
		bAllowDelayedMessaging = SGMessagingSettings->bAllowDelayedMessaging;
	}
}


FSGMessageRouter::~FSGMessageRouter()
{
	FPlatformProcess::ReturnSynchEventToPool(WorkEvent);
	WorkEvent = nullptr;
}


/* FSGRunnable interface
 *****************************************************************************/

FSingleThreadRunnable* FSGMessageRouter::GetSingleThreadInterface()
{
	return this;
}


bool FSGMessageRouter::Init()
{
	return true;
}


uint32 FSGMessageRouter::Run()
{
	while (!Stopping)
	{
		CurrentTime = FDateTime::UtcNow();

		ProcessCommands();
		ProcessDelayedMessages();

		WorkEvent->Wait(CalculateWaitTime());
	}

	return 0;
}


void FSGMessageRouter::Stop()
{
	Tracer->Stop();
	Stopping = true;
	WorkEvent->Trigger();
}


void FSGMessageRouter::Exit()
{
	TArray<TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>> Recipients;

	// gather all subscribed and registered recipients
	for (const auto& RecipientPair : ActiveRecipients)
	{
		Recipients.AddUnique(RecipientPair.Value);
	}

	for (const auto& SubscriptionsPair : ActiveSubscriptions)
	{
		for (const auto& Subscription : SubscriptionsPair.Value)
		{
			Recipients.AddUnique(Subscription->GetSubscriber());
		}
	}
}


/* FSGMessageRouter implementation
 *****************************************************************************/

FTimespan FSGMessageRouter::CalculateWaitTime()
{
	const FTimespan WaitTime = FTimespan::FromMilliseconds(100);

	if (DelayedMessages.Num() > 0)
	{
		const FTimespan DelayedTime = DelayedMessages.HeapTop().Context->GetTimeSent() - CurrentTime;

		if (DelayedTime < WaitTime)
		{
			return DelayedTime;
		}
	}

	return WaitTime;
}


void FSGMessageRouter::DispatchMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	if (Context->IsValid())
	{
		TArray<TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe>> Recipients;

		const int32 RecipientCount = Context->GetRecipients().Num();

		// get recipients, either from the context...
		if (RecipientCount > 0)
		{
			if (UE_GET_LOG_VERBOSITY(LogSGMessaging) >= ELogVerbosity::Verbose)
			{
				const FString RecipientStr = FString::JoinBy(Context->GetRecipients(), TEXT("+"),
				                                             &FSGMessageAddress::ToString);
				UE_LOG(LogSGMessaging, Verbose, TEXT("Dispatching %s from %s to %s"),
				       *Context->GetMessageTag().ToString(), *Context->GetSender().ToString(), *RecipientStr);
			}

			FilterRecipients(Context, Recipients);

			if (Recipients.Num() < RecipientCount)
			{
				UE_LOG(LogSGMessaging, Verbose, TEXT("%d recipients were filtered out"),
				       RecipientCount - Recipients.Num());
			}
		}
		// ... or from subscriptions
		else
		{
			FilterSubscriptions(ActiveSubscriptions.FindOrAdd(Context->GetMessageTag()), Context, Recipients);
			FilterSubscriptions(ActiveSubscriptions.FindOrAdd(NAME_All), Context, Recipients);

			if (UE_GET_LOG_VERBOSITY(LogSGMessaging) >= ELogVerbosity::Verbose)
			{
				const FString RecipientStr = FString::JoinBy(Context->GetRecipients(), TEXT("+"),
				                                             &FSGMessageAddress::ToString);
				UE_LOG(LogSGMessaging, Verbose, TEXT("Dispatching %s from %s to %s subscribers"),
				       *Context->GetMessageTag().ToString(), *Context->GetSender().ToString(), *RecipientStr);
			}
		}

		// dispatch the message
		for (auto& Recipient : Recipients)
		{
			ENamedThreads::Type RecipientThread = Recipient->GetRecipientThread();

			if (RecipientThread == ENamedThreads::AnyThread)
			{
				Tracer->TraceDispatchedMessage(Context, Recipient.ToSharedRef(), false);
				Recipient->ReceiveMessage(Context);
				Tracer->TraceHandledMessage(Context, Recipient.ToSharedRef());
			}
			else
			{
				TGraphTask<FSGMessageDispatchTask>::CreateTask().ConstructAndDispatchWhenReady(
					RecipientThread, Context, Recipient, Tracer);
			}
		}
	}
}


void FSGMessageRouter::FilterSubscriptions(
	TArray<TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe>>& Subscriptions,
	const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	TArray<TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe>>& OutRecipients
)
{
	const ESGMessageScope MessageScope = Context->GetScope();

	for (int32 SubscriptionIndex = 0; SubscriptionIndex < Subscriptions.Num(); ++SubscriptionIndex)
	{
		const auto Subscription = Subscriptions[SubscriptionIndex];

		if (!Subscription->IsEnabled() || !Subscription->GetScopeRange().Contains(MessageScope))
		{
			continue;
		}

		if (auto Subscriber = Subscription->GetSubscriber().Pin())
		{
			if (MessageScope == ESGMessageScope::Thread)
			{
				const ENamedThreads::Type RecipientThread = Subscriber->GetRecipientThread();
				const ENamedThreads::Type SenderThread = Context->GetSenderThread();

				if (RecipientThread != SenderThread)
				{
					continue;
				}
			}

			OutRecipients.AddUnique(Subscriber);
		}
		else
		{
			Subscriptions.RemoveAtSwap(SubscriptionIndex);
			--SubscriptionIndex;
		}
	}
}


void FSGMessageRouter::FilterRecipients(
	const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	TArray<TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe>>& OutRecipients)
{
	const FSGMessageScopeRange IncludeNetwork = FSGMessageScopeRange::AtLeast(ESGMessageScope::Network);
	const TArray<FSGMessageAddress>& RecipientList = Context->GetRecipients();
	for (const auto& RecipientAddress : RecipientList)
	{
		if (auto Recipient = ActiveRecipients.FindRef(RecipientAddress).Pin())
		{
			// if the recipient is not local and the scope does not include network, filter it out of the recipient list
			if (Recipient->IsLocal() || IncludeNetwork.Contains(Context->GetScope()))
			{
				OutRecipients.AddUnique(Recipient);
			}
		}
		else
		{
			ActiveRecipients.Remove(RecipientAddress);
		}
	}
}


void FSGMessageRouter::ProcessCommands()
{
	FCommandDelegate Command;

	while (Commands.Dequeue(Command))
	{
		Command.Execute();
	}
}


void FSGMessageRouter::ProcessDelayedMessages()
{
	FSGDelayedMessage DelayedMessage;

	while ((DelayedMessages.Num() > 0) && (DelayedMessages.HeapTop().Context->GetTimeSent() <= CurrentTime))
	{
		DelayedMessages.HeapPop(DelayedMessage);
		DispatchMessage(DelayedMessage.Context.ToSharedRef());
	}
}


/* FSingleThreadRunnable interface
 *****************************************************************************/

void FSGMessageRouter::Tick()
{
	CurrentTime = FDateTime::UtcNow();

	ProcessDelayedMessages();
	ProcessCommands();
}


/* FSGMessageRouter callbacks
 *****************************************************************************/

void FSGMessageRouter::HandleAddInterceptor(TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe> Interceptor,
                                            FName MessageTag)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Adding %s as intereceptor for %s messages"),
	       *Interceptor->GetDebugName().ToString(), *MessageTag.ToString());

	ActiveInterceptors.FindOrAdd(MessageTag).AddUnique(Interceptor);
	Tracer->TraceAddedInterceptor(Interceptor, MessageTag);
}


void FSGMessageRouter::HandleAddRecipient(FSGMessageAddress Address,
                                          TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> RecipientPtr)
{
	if (const auto Recipient = RecipientPtr.Pin())
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Adding %s on %s as recipient"), *Recipient->GetDebugName().ToString(),
		       *Address.ToString());

		ActiveRecipients.FindOrAdd(Address) = Recipient;
		Tracer->TraceAddedRecipient(Address, Recipient.ToSharedRef());
		NotifyRegistration(Address, ESGMessageBusNotification::Registered);
	}
}


void FSGMessageRouter::HandleAddSubscriber(TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe> Subscription)
{
	if (const auto Subscriber = Subscription->GetSubscriber().Pin())
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Adding %s as a subscriber for %s messages"),
		       *Subscriber->GetDebugName().ToString(), *Subscription->GetMessageTag().ToString());
	}

	ActiveSubscriptions.FindOrAdd(Subscription->GetMessageTag()).AddUnique(Subscription);
	Tracer->TraceAddedSubscription(Subscription);
}


void FSGMessageRouter::HandleRemoveInterceptor(TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe> Interceptor,
                                               FName MessageTag)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Removing %s as intereceptor for %s messages"),
	       *Interceptor->GetDebugName().ToString(), *MessageTag.ToString());

	if (MessageTag == NAME_All)
	{
		for (auto& InterceptorsPair : ActiveInterceptors)
		{
			InterceptorsPair.Value.Remove(Interceptor);
		}
	}
	else
	{
		auto& Interceptors = ActiveInterceptors.FindOrAdd(MessageTag);
		Interceptors.Remove(Interceptor);
	}

	Tracer->TraceRemovedInterceptor(Interceptor, MessageTag);
}

void FSGMessageRouter::HandleRemoveRecipient(FSGMessageAddress Address)
{
	if (const auto Recipient = ActiveRecipients.FindRef(Address).Pin())
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Removing %s on %s as recipient"), *Recipient->GetDebugName().ToString(),
		       *Address.ToString());

		ActiveRecipients.Remove(Address);
		Tracer->TraceRemovedRecipient(Address);
		NotifyRegistration(Address, ESGMessageBusNotification::Unregistered);
	}
}


void FSGMessageRouter::HandleRemoveSubscriber(TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> SubscriberPtr,
                                              FName MessageTag)
{
	const auto Subscriber = SubscriberPtr.Pin();

	if (!Subscriber.IsValid())
	{
		return;
	}

	for (auto& SubscriptionsPair : ActiveSubscriptions)
	{
		if ((MessageTag != NAME_All) && (MessageTag != SubscriptionsPair.Key))
		{
			continue;
		}

		TArray<TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe>>& Subscriptions = SubscriptionsPair.Value;

		for (int32 SubscriptionIndex = 0; SubscriptionIndex < Subscriptions.Num(); ++SubscriptionIndex)
		{
			const auto Subscription = Subscriptions[SubscriptionIndex];

			if (Subscription->GetSubscriber().Pin() == Subscriber)
			{
				UE_LOG(LogSGMessaging, Verbose, TEXT("Removing %s as a subscriber for %s messages"),
				       *Subscriber->GetDebugName().ToString(), *Subscription->GetMessageTag().ToString());

				Subscriptions.RemoveAtSwap(SubscriptionIndex);
				Tracer->TraceRemovedSubscription(Subscription.ToSharedRef(), MessageTag);

				break;
			}
		}
	}
}


void FSGMessageRouter::HandleRouteMessage(TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> Context)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Routing %s message from %s"), *Context->GetMessageTag().ToString(),
	       *Context->GetSender().ToString());

	Tracer->TraceRoutedMessage(Context);

	// intercept routing
	auto& Interceptors = ActiveInterceptors.FindOrAdd(Context->GetMessageTag());

	for (const auto& Interceptor : Interceptors)
	{
		if (Interceptor->InterceptMessage(Context))
		{
			UE_LOG(LogSGMessaging, Verbose, TEXT("Message was intercepted by %s"),
			       *Interceptor->GetDebugName().ToString());

			Tracer->TraceInterceptedMessage(Context, Interceptor.ToSharedRef());

			return;
		}
	}

	// dispatch the message
	if (bAllowDelayedMessaging && (Context->GetTimeSent() > CurrentTime))
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Queued message for dispatch"));

		DelayedMessages.HeapPush(FSGDelayedMessage(Context, ++DelayedMessagesSequence));
	}
	else
	{
		DispatchMessage(Context);
	}
}

void FSGMessageRouter::HandleAddListener(TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> ListenerPtr)
{
	ActiveRegistrationListeners.AddUnique(ListenerPtr);
}

void FSGMessageRouter::HandleRemoveListener(TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> ListenerPtr)
{
	ActiveRegistrationListeners.Remove(ListenerPtr);
}

void FSGMessageRouter::NotifyRegistration(const FSGMessageAddress& Address, ESGMessageBusNotification Notification)
{
	for (auto It = ActiveRegistrationListeners.CreateIterator(); It; ++It)
	{
		if (auto Listener = It->Pin())
		{
			ENamedThreads::Type ListenerThread = Listener->GetListenerThread();

			if (ListenerThread == ENamedThreads::AnyThread)
			{
				Listener->NotifyRegistration(Address, Notification);
			}
			else
			{
				TGraphTask<FSGBusNotificationDispatchTask>::CreateTask().ConstructAndDispatchWhenReady(
					ListenerThread, Listener, Address, Notification);
			}
		}
		else
		{
			It.RemoveCurrent();
		}
	}
}
