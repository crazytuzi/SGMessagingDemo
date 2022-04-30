// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Bus/SGMessageDispatchTask.h"
#include "Core/Interface/ISGMessageReceiver.h"


/* FSGMessageDispatchTask structors
 *****************************************************************************/

FSGMessageDispatchTask::FSGMessageDispatchTask(
	const ENamedThreads::Type InThread,
	const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> InContext,
	const TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> InRecipient,
	const TSharedPtr<FSGMessageTracer, ESPMode::ThreadSafe> InTracer
)
	: Context(InContext)
	  , RecipientPtr(InRecipient)
	  , Thread(InThread)
	  , TracerPtr(InTracer)
{
}


/* FSGMessageDispatchTask interface
 *****************************************************************************/

void FSGMessageDispatchTask::DoTask(ENamedThreads::Type CurrentThread,
                                    const FGraphEventRef& MyCompletionGraphEvent) const
{
	const TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe> Recipient = RecipientPtr.Pin();

	if (!Recipient.IsValid())
	{
		return;
	}

	const auto Tracer = TracerPtr.Pin();

	if (Tracer.IsValid())
	{
		Tracer->TraceDispatchedMessage(Context, Recipient.ToSharedRef(), true);
	}

	Recipient->ReceiveMessage(Context);

	if (TracerPtr.IsValid())
	{
		Tracer->TraceHandledMessage(Context, Recipient.ToSharedRef());
	}
}

TStatId FSGMessageDispatchTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FSGMessageDispatchTask, STATGROUP_TaskGraphTasks);
}

/* FSGBusNotificationDispatchTask interface
 *****************************************************************************/

void FSGBusNotificationDispatchTask::DoTask(ENamedThreads::Type CurrentThread,
                                            const FGraphEventRef& MyCompletionGraphEvent) const
{
	const TSharedPtr<ISGBusListener, ESPMode::ThreadSafe> Listener = ListenerPtr.Pin();

	if (!Listener.IsValid())
	{
		return;
	}

	Listener->NotifyRegistration(Address, Notification);
}

TStatId FSGBusNotificationDispatchTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FSGBusNotificationDispatchTask, STATGROUP_TaskGraphTasks);
}
