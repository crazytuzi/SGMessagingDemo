// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Bus/SGMessageTracer.h"
#include "Containers/Ticker.h"
#include "HAL/PlatformProcess.h"
#include "Core/Interface/ISGMessageInterceptor.h"
#include "Core/Interface/ISGMessageReceiver.h"
#include "Core/Interface/ISGMessageTracerBreakpoint.h"

/* FSGMessageTracer structors
 *****************************************************************************/

FSGMessageTracer::FSGMessageTracer()
	: Breaking(false)
	, ResetPending(false)
	, Running(false)
{
	ContinueEvent = FPlatformProcess::GetSynchEventFromPool();
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FSGMessageTracer::Tick), 0.0f);
}


FSGMessageTracer::~FSGMessageTracer()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	FPlatformProcess::ReturnSynchEventToPool(ContinueEvent);
	ContinueEvent = nullptr;
}


/* FSGMessageTracer interface
 *****************************************************************************/

void FSGMessageTracer::TraceAddedInterceptor(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
{
	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// create interceptor information
		auto& InterceptorInfo = Interceptors.FindOrAdd(Interceptor->GetInterceptorId());

		if (!InterceptorInfo.IsValid())
		{
			InterceptorInfo = MakeShareable(new FSGMessageTracerInterceptorInfo());
		}

		// initialize interceptor information
		InterceptorInfo->Name = Interceptor->GetDebugName();
		InterceptorInfo->TimeRegistered = Timestamp;
		InterceptorInfo->TimeUnregistered = 0;
	});
}


void FSGMessageTracer::TraceAddedRecipient(const FSGMessageAddress& Address, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient)
{
	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// create endpoint information
		TSharedPtr<FSGMessageTracerEndpointInfo>& EndpointInfo = RecipientsToEndpointInfos.FindOrAdd(Recipient->GetRecipientId());

		if (!EndpointInfo.IsValid())
		{
			EndpointInfo = MakeShareable(new FSGMessageTracerEndpointInfo());
		}

		// initialize endpoint information
		TSharedRef<FSGMessageTracerAddressInfo> AddressInfo = MakeShareable(new FSGMessageTracerAddressInfo());
		{
			AddressInfo->Address = Address;
			AddressInfo->TimeRegistered = Timestamp;
			AddressInfo->TimeUnregistered = 0;
		}

		EndpointInfo->AddressInfos.Add(Address, AddressInfo);
		EndpointInfo->Name = Recipient->GetDebugName();
		EndpointInfo->Remote = Recipient->IsRemote();

		// add to address table
		AddressesToEndpointInfos.Add(Address, EndpointInfo);
	});
}


void FSGMessageTracer::TraceAddedSubscription(const TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe>& Subscription)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// @todo gmp: trace added subscriptions
	});
}


void FSGMessageTracer::TraceDispatchedMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient, bool Async)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// look up message & endpoint info
		TSharedPtr<FSGMessageTracerMessageInfo> MessageInfo = MessageInfos.FindRef(Context);

		if (!MessageInfo.IsValid())
		{
			return;
		}

		TSharedPtr<FSGMessageTracerEndpointInfo>& EndpointInfo = RecipientsToEndpointInfos.FindOrAdd(Recipient->GetRecipientId());

		if (!EndpointInfo.IsValid())
		{
			return;
		}

		// update message information
		TSharedRef<FSGMessageTracerDispatchState> DispatchState = MakeShareable(new FSGMessageTracerDispatchState());
		{
			DispatchState->DispatchLatency = Timestamp - MessageInfo->TimeSent;
			DispatchState->DispatchType = Async ? ESGMessageTracerDispatchTypes::TaskGraph : ESGMessageTracerDispatchTypes::Direct;
			DispatchState->EndpointInfo = EndpointInfo;
			DispatchState->RecipientThread = Recipient->GetRecipientThread();
			DispatchState->TimeDispatched = Timestamp;
			DispatchState->TimeHandled = 0.0;
		}

		MessageInfo->DispatchStates.Add(EndpointInfo, DispatchState);

		// update database
		EndpointInfo->ReceivedMessages.Add(MessageInfo);
	});
}


void FSGMessageTracer::TraceHandledMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// look up message & endpoint info
		TSharedPtr<FSGMessageTracerMessageInfo> MessageInfo = MessageInfos.FindRef(Context);

		if (!MessageInfo.IsValid())
		{
			return;
		}

		TSharedPtr<FSGMessageTracerEndpointInfo> EndpointInfo = RecipientsToEndpointInfos.FindRef(Recipient->GetRecipientId());

		if (!EndpointInfo.IsValid())
		{
			return;
		}

		// update message information
		TSharedPtr<FSGMessageTracerDispatchState> DispatchState = MessageInfo->DispatchStates.FindRef(EndpointInfo);

		if (DispatchState.IsValid())
		{
			DispatchState->TimeHandled = Timestamp;
		}
	});
}


void FSGMessageTracer::TraceInterceptedMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// look up message & interceptor info
		auto MessageInfo = MessageInfos.FindRef(Context);

		if (!MessageInfo.IsValid())
		{
			return;
		}

		MessageInfo->Intercepted = true;

		auto InterceptorInfo = Interceptors.FindRef(Interceptor->GetInterceptorId());

		if (!InterceptorInfo.IsValid())
		{
			return;
		}

		// update interceptor information
		InterceptorInfo->InterceptedMessages.Add(MessageInfo);
	});		
}


void FSGMessageTracer::TraceRemovedInterceptor(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
{
	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		auto InterceptorInfo = Interceptors.FindRef(Interceptor->GetInterceptorId());

		if (!InterceptorInfo.IsValid())
		{
			return;
		}

		// update interceptor information
		InterceptorInfo->TimeUnregistered = Timestamp;
	});
}


void FSGMessageTracer::TraceRemovedRecipient(const FSGMessageAddress& Address)
{
	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		TSharedPtr<FSGMessageTracerEndpointInfo> EndpointInfo = AddressesToEndpointInfos.FindRef(Address);

		if (!EndpointInfo.IsValid())
		{
			return;
		}

		// update endpoint information
		TSharedPtr<FSGMessageTracerAddressInfo> AddressInfo = EndpointInfo->AddressInfos.FindRef(Address);

		if (AddressInfo.IsValid())
		{
			AddressInfo->TimeUnregistered = Timestamp;
		}
	});
}


void FSGMessageTracer::TraceRemovedSubscription(const TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe>& Subscription, const FName& MessageType)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// @todo gmp: trace removed message subscriptions
	});
}


void FSGMessageTracer::TraceRoutedMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	if (!Running)
	{
		return;
	}

	if (ShouldBreak(Context))
	{
		Breaking = true;
		ContinueEvent->Wait();
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// update message information
		TSharedPtr<FSGMessageTracerMessageInfo> MessageInfo = MessageInfos.FindRef(Context);

		if (MessageInfo.IsValid())
		{
			MessageInfo->TimeRouted = Timestamp;
		}
	});
}


void FSGMessageTracer::TraceSentMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	if (!Running)
	{
		return;
	}

	double Timestamp = FPlatformTime::Seconds();

	Traces.Enqueue([=]() {
		// look up endpoint info
		TSharedPtr<FSGMessageTracerEndpointInfo> EndpointInfo = AddressesToEndpointInfos.FindRef(Context->GetSender());

		if (!EndpointInfo.IsValid())
		{
			return;
		}

		// create message info
		TSharedRef<FSGMessageTracerMessageInfo> MessageInfo = MakeShareable(new FSGMessageTracerMessageInfo());
		{
			MessageInfo->Context = Context;
			MessageInfo->Intercepted = false;
			MessageInfo->SenderInfo = EndpointInfo;
			MessageInfo->TimeRouted = 0.0;
			MessageInfo->TimeSent = Timestamp;
			MessageInfos.Add(Context, MessageInfo);
		}

		// add message type
		TSharedPtr<FSGMessageTracerTypeInfo>& TypeInfo = MessageTypes.FindOrAdd(Context->GetMessageType());

		if (!TypeInfo.IsValid())
		{
			TypeInfo = MakeShareable(new FSGMessageTracerTypeInfo());
			TypeInfo->TypeName = Context->GetMessageType();

			TypeAddedDelegate.Broadcast(TypeInfo.ToSharedRef());
		}

		TypeInfo->Messages.Add(MessageInfo);

		// update database
		EndpointInfo->SentMessages.Add(MessageInfo);
		MessageInfo->TypeInfo = TypeInfo;

		MessagesAddedDelegate.Broadcast(MessageInfo);
	});
}


/* ISGMessageTracer interface
 *****************************************************************************/

void FSGMessageTracer::Break()
{
	Breaking = true;
}


void FSGMessageTracer::Continue()
{
	if (!Running)
	{
		Running = true;
	}
	else if (Breaking)
	{
		Breaking = false;
		ContinueEvent->Trigger();
	}
}


int32 FSGMessageTracer::GetEndpoints(TArray<TSharedPtr<FSGMessageTracerEndpointInfo>>& OutEndpoints) const
{
	RecipientsToEndpointInfos.GenerateValueArray(OutEndpoints);

	return OutEndpoints.Num();
}


int32 FSGMessageTracer::GetMessages(TArray<TSharedPtr<FSGMessageTracerMessageInfo>>& OutMessages) const
{
	MessageInfos.GenerateValueArray(OutMessages);

	return OutMessages.Num();
}


int32 FSGMessageTracer::GetMessageTypes(TArray<TSharedPtr<FSGMessageTracerTypeInfo>>& OutTypes) const
{
	MessageTypes.GenerateValueArray(OutTypes);

	return OutTypes.Num();
}


bool FSGMessageTracer::HasMessages() const
{
	return (MessageInfos.Num() > 0);
}


bool FSGMessageTracer::IsBreaking() const
{
	return Breaking;
}


bool FSGMessageTracer::IsRunning() const
{
	return Running;
}


void FSGMessageTracer::Reset()
{
	ResetPending = true;
}


void FSGMessageTracer::Step()
{
	if (!Breaking)
	{
		return;
	}

	ContinueEvent->Trigger();
}


void FSGMessageTracer::Stop()
{
	if (!Running)
	{
		return;
	}

	Running = false;

	if (Breaking)
	{
		Breaking = false;
		ContinueEvent->Trigger();
	}
}


bool FSGMessageTracer::Tick(float DeltaTime)
{
    QUICK_SCOPE_CYCLE_COUNTER(STAT_FSGMessageTracer_Tick);

	if (ResetPending)
	{
		ResetMessages();
		ResetPending = false;
	}

	// process new traces
	if (!Traces.IsEmpty())
	{
		TFunction<void()> Trace;

		while (Traces.Dequeue(Trace))
		{
			Trace();
		}
	}

	return true;
}


/* FSGMessageTracer implementation
 *****************************************************************************/

void FSGMessageTracer::ResetMessages()
{
	MessageInfos.Reset();
	MessageTypes.Reset();

	for (auto& EndpointInfoPair : AddressesToEndpointInfos)
	{
		TSharedPtr<FSGMessageTracerEndpointInfo>& EndpointInfo = EndpointInfoPair.Value;
		{
			EndpointInfo->ReceivedMessages.Reset();
			EndpointInfo->SentMessages.Reset();
		}
	}

	MessagesResetDelegate.Broadcast();
}


bool FSGMessageTracer::ShouldBreak(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) const
{
	if (FPlatformProcess::SupportsMultithreading())
	{
		if (Breaking)
		{
			return true;
		}

		for (const auto& Breakpoint : Breakpoints)
		{
			if (Breakpoint->IsEnabled() && Breakpoint->ShouldBreak(Context))
			{
				return true;
			}
		}
	}

	return false;
}
