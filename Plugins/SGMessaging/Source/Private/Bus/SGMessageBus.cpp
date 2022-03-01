// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bus/SGMessageBus.h"
#include "Interface/ISGMessagingModule.h"
#include "HAL/RunnableThread.h"
#include "Bus/SGMessageRouter.h"
#include "Bus/SGMessageContext.h"
#include "Bus/SGMessageSubscription.h"
#include "Interface/ISGMessageSender.h"
#include "Interface/ISGMessageReceiver.h"
#include "HAL/ThreadSingleton.h"





/* FSGMessageBus structors
 *****************************************************************************/

FSGMessageBus::FSGMessageBus(FString InName, const TSharedPtr<ISGAuthorizeMessageRecipients>& InRecipientAuthorizer)
	: Name(MoveTemp(InName))
	, RecipientAuthorizer(InRecipientAuthorizer)
{
	Router = new FSGMessageRouter();
	RouterThread = FRunnableThread::Create(Router, *FString::Printf(TEXT("FSGMessageBus.%s.Router"), *Name), 128 * 1024, TPri_Normal, FPlatformAffinity::GetPoolThreadMask());

	check(Router != nullptr);
}


FSGMessageBus::~FSGMessageBus()
{
	Shutdown();

	delete Router;
}


/* ISGMessageBus interface
 *****************************************************************************/

void FSGMessageBus::Forward(
	const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	const TArray<FSGMessageAddress>& Recipients,
	const FTimespan& Delay,
	const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Forwarder
)
{
	if (UE_GET_LOG_VERBOSITY(LogSGMessaging) >= ELogVerbosity::Verbose)
	{
		FString RecipientStr = FString::JoinBy(Context->GetRecipients(), TEXT("+"), &FSGMessageAddress::ToString);

		UE_LOG(LogSGMessaging, Verbose, TEXT("Forwarding %s from %s to %s"),
			*Context->GetMessageType().ToString(),
			*Context->GetSender().ToString(), *RecipientStr);
	}

	Router->RouteMessage(MakeShareable(new FSGMessageContext(
		Context,
		Forwarder->GetSenderAddress(),
		Recipients,
		ESGMessageScope::Process,
		FDateTime::UtcNow() + Delay,
		FTaskGraphInterface::Get().GetCurrentThreadIfKnown()
	)));
}


TSharedRef<ISGMessageTracer, ESPMode::ThreadSafe> FSGMessageBus::GetTracer()
{
	return Router->GetTracer();
}


void FSGMessageBus::Intercept(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
{
	if (MessageType == NAME_None)
	{
		return;
	}

	if (!RecipientAuthorizer.IsValid() || RecipientAuthorizer->AuthorizeInterceptor(Interceptor, MessageType))
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Adding invterceptor %s"), *Interceptor->GetDebugName().ToString());
		Router->AddInterceptor(Interceptor, MessageType);
	}			
}


FOnMessageBusShutdown& FSGMessageBus::OnShutdown()
{
	return ShutdownDelegate;
}


void FSGMessageBus::Publish(
	void* Message,
	UScriptStruct* TypeInfo,
	ESGMessageScope Scope,
	const TMap<FName, FString>& Annotations,
	const FTimespan& Delay,
	const FDateTime& Expiration,
	const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Publisher
)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Publishing %s from sender %s"), *TypeInfo->GetName(), *Publisher->GetSenderAddress().ToString());

	Router->RouteMessage(MakeShared<FSGMessageContext, ESPMode::ThreadSafe>(
		Message,
		TypeInfo,
		Annotations,
		nullptr,
		Publisher->GetSenderAddress(),
		TArray<FSGMessageAddress>(),
		Scope,
		ESGMessageFlags::None,
		FDateTime::UtcNow() + Delay,
		Expiration,
		FTaskGraphInterface::Get().GetCurrentThreadIfKnown()
	));
}

void FSGMessageBus::Publish(
	const FName& MessageTag,
	void* Message,
	ESGMessageScope Scope,
	const TMap<FName, FString>& Annotations,
	const FTimespan& Delay,
	const FDateTime& Expiration,
	const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Publisher)
{
	Router->RouteMessage(MakeShared<FSGMessageContext, ESPMode::ThreadSafe>(
		MessageTag,
		Message,
		Annotations,
		nullptr,
		Publisher->GetSenderAddress(),
		TArray<FSGMessageAddress>(),
		Scope,
		ESGMessageFlags::None,
		FDateTime::UtcNow() + Delay,
		Expiration,
		FTaskGraphInterface::Get().GetCurrentThreadIfKnown()
	));
}

void FSGMessageBus::Register(const FSGMessageAddress& Address, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Registering %s"), *Address.ToString());
	Router->AddRecipient(Address, Recipient);
}


void FSGMessageBus::Send(
	void* Message,
	UScriptStruct* TypeInfo,
	ESGMessageFlags Flags,
	const TMap<FName, FString>& Annotations,
	const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& Attachment,
	const TArray<FSGMessageAddress>& Recipients,
	const FTimespan& Delay,
	const FDateTime& Expiration,
	const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Sender
)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Sending %s to %d recipients"), *TypeInfo->GetName(), Recipients.Num());

	Router->RouteMessage(MakeShared<FSGMessageContext, ESPMode::ThreadSafe>(
		Message,
		TypeInfo,
		Annotations,
		Attachment,
		Sender->GetSenderAddress(),
		Recipients,
		ESGMessageScope::Network,
		Flags,
		FDateTime::UtcNow() + Delay,
		Expiration,
		FTaskGraphInterface::Get().GetCurrentThreadIfKnown()
	));
}

void FSGMessageBus::Send(
	const FName& MessageTag,
	void* Message, ESGMessageFlags Flags,
	const TMap<FName, FString>& Annotations,
	const TSharedPtr<ISGMessageAttachment,
	ESPMode::ThreadSafe>& Attachment,
	const TArray<FSGMessageAddress>& Recipients,
	const FTimespan& Delay,
	const FDateTime& Expiration,
	const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Sender)
{
	Router->RouteMessage(MakeShared<FSGMessageContext, ESPMode::ThreadSafe>(
		MessageTag,
		Message,
		Annotations,
		Attachment,
		Sender->GetSenderAddress(),
		Recipients,
		ESGMessageScope::Network,
		Flags,
		FDateTime::UtcNow() + Delay,
		Expiration,
		FTaskGraphInterface::Get().GetCurrentThreadIfKnown()
	));
}


void FSGMessageBus::Shutdown()
{
	if (RouterThread != nullptr)
	{
		ShutdownDelegate.Broadcast();

		RouterThread->Kill(true);
		delete RouterThread;
		RouterThread = nullptr;
	}
}


TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe> FSGMessageBus::Subscribe(
	const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber,
	const FName& MessageType,
	const FSGMessageScopeRange& ScopeRange
)
{
	if (MessageType != NAME_None)
	{
		if (!RecipientAuthorizer.IsValid() || RecipientAuthorizer->AuthorizeSubscription(Subscriber, MessageType))
		{
			UE_LOG(LogSGMessaging, Verbose, TEXT("Subscribing %s"), *Subscriber->GetDebugName().ToString());
			TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe> Subscription = MakeShareable(new FSGMessageSubscription(Subscriber, MessageType, ScopeRange));
			Router->AddSubscription(Subscription);

			return Subscription;
		}
	}

	return nullptr;
}


void FSGMessageBus::Unintercept(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
{
	if (MessageType != NAME_None)
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Unintercepting %s"), *Interceptor->GetDebugName().ToString());
		Router->RemoveInterceptor(Interceptor, MessageType);
	}
}


void FSGMessageBus::Unregister(const FSGMessageAddress& Address)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Attempting to unregister %s"), *Address.ToString());
	if (!RecipientAuthorizer.IsValid() || RecipientAuthorizer->AuthorizeUnregistration(Address))
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("Unregistered %s"), *Address.ToString());
		Router->RemoveRecipient(Address);
	}
}


void FSGMessageBus::Unsubscribe(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber, const FName& MessageType)
{
	if (MessageType != NAME_None)
	{
		if (!RecipientAuthorizer.IsValid() || RecipientAuthorizer->AuthorizeUnsubscription(Subscriber, MessageType))
		{
			UE_LOG(LogSGMessaging, Verbose, TEXT("Unsubscribing %s"), *Subscriber->GetDebugName().ToString());
			Router->RemoveSubscription(Subscriber, MessageType);
		}
	}
}

void FSGMessageBus::AddNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener)
{
	Router->AddNotificationListener(Listener);
}

void FSGMessageBus::RemoveNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener)
{
	Router->RemoveNotificationListener(Listener);
}

const FString& FSGMessageBus::GetName() const
{
	return Name;
}
