// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "HAL/Runnable.h"
#include "Misc/SingleThreadRunnable.h"
#include "Templates/Atomic.h"

#include "Interface/ISGMessageContext.h"
#include "Interface/ISGMessageTracer.h"
#include "Bus/SGMessageTracer.h"

class ISGMessageInterceptor;
class ISGMessageReceiver;
class ISGMessageSubscription;
class ISGBusListener;

enum class ESGMessageBusNotification : uint8;

/**
 * Implements a topic-based message router.
 */
class FSGMessageRouter
	: public FRunnable
	, private FSingleThreadRunnable
{
	DECLARE_DELEGATE(CommandDelegate)

public:

	/** Default constructor. */
	FSGMessageRouter();

	/** Destructor. */
	~FSGMessageRouter();

public:

	/**
	 * Adds a message interceptor.
	 *
	 * @param Interceptor The interceptor to add.
	 * @param MessageType The type of messages to intercept.
	 */
	FORCEINLINE void AddInterceptor(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleAddInterceptor, Interceptor, MessageType));
	}

	/**
	 * Adds a recipient.
	 *
	 * @param Address The address of the recipient to add.
	 * @param Recipient The recipient.
	 */
	FORCEINLINE void AddRecipient(const FSGMessageAddress& Address, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleAddRecipient, Address, TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>(Recipient)));
	}

	/**
	 * Adds a subscription.
	 *
	 * @param Subscription The subscription to add.
	 */
	FORCEINLINE void AddSubscription(const TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe>& Subscription)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleAddSubscriber, Subscription));
	}

	/**
	 * Gets the message tracer.
	 *
	 * @return Weak pointer to the message tracer.
	 */
	FORCEINLINE TSharedRef<ISGMessageTracer, ESPMode::ThreadSafe> GetTracer()
	{
		return Tracer;
	}

	/**
	 * Removes a message interceptor.
	 *
	 * @param Interceptor The interceptor to remove.
	 * @param MessageType The type of messages to stop intercepting.
	 */
	FORCEINLINE void RemoveInterceptor(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleRemoveInterceptor, Interceptor, MessageType));
	}

	/**
	 * Removes a recipient.
	 *
	 * @param Address The address of the recipient to remove.
	 */
	FORCEINLINE void RemoveRecipient(const FSGMessageAddress& Address)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleRemoveRecipient, Address));
	}

	/**
	 * Removes a subscription.
	 *
	 * @param Subscriber The subscriber to stop routing messages to.
	 * @param MessageType The type of message to unsubscribe from (NAME_None = all types).
	 */
	FORCEINLINE void RemoveSubscription(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber, const FName& MessageType)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleRemoveSubscriber, TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>(Subscriber), MessageType));
	}

	/**
	 * Routes a message to the specified recipients.
	 *
	 * @param Context The context of the message to route.
	 */
	FORCEINLINE void RouteMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
	{
		Tracer->TraceSentMessage(Context);
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleRouteMessage, Context));
	}

	/**
	 * Add a listener to the bus registration events
	 * 
	 * @param Listener The listener to as to the registration notifications
	 */
	FORCEINLINE void AddNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleAddListener, TWeakPtr<ISGBusListener, ESPMode::ThreadSafe>(Listener)));
	}

	/**
	 * Remove a listener to the bus registration events
	 *
	 * @param Listener The listener to remove from the registration notifications
	 */
	FORCEINLINE void RemoveNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener)
	{
		EnqueueCommand(FSimpleDelegate::CreateRaw(this, &FSGMessageRouter::HandleRemoveListener, TWeakPtr<ISGBusListener, ESPMode::ThreadSafe>(Listener)));
	}

public:

	//~ FRunnable interface

	virtual FSingleThreadRunnable* GetSingleThreadInterface() override;
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

protected:

	/**
	 * Calculates the time that the thread will wait for new work.
	 *
	 * @return Wait time.
	 */
	FTimespan CalculateWaitTime();

	/**
	 * Queues up a router command.
	 *
	 * @param Command The command to queue up.
	 * @return true if the command was enqueued, false otherwise.
	 */
	FORCEINLINE bool EnqueueCommand(CommandDelegate Command)
	{
		if (!Commands.Enqueue(Command))
		{
			return false;
		}

		WorkEvent->Trigger();

		return true;
	}

	/**
	 * Filters a collection of subscriptions using the given message context.
	 *
	 * @param Subscriptions The subscriptions to filter.
	 * @param Context The message context to filter by.
	 * @param OutRecipients Will hold the collection of recipients.
	 */
	void FilterSubscriptions(
		TArray<TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe>>& Subscriptions,
		const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
		TArray<TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe>>& OutRecipients);

	/**
	 * Filters recipients from the given message context to gather actual recipients.
	 *
	 * @param Context The message context to filter by.
	 * @param OutRecipients Will hold the collection of recipients.
	 */
	void FilterRecipients(
		const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
		TArray<TSharedPtr<ISGMessageReceiver, ESPMode::ThreadSafe>>& OutRecipients);

	/**
	 * Dispatches a single message to its recipients.
	 *
	 * @param Message The message to dispatch.
	 */
	void DispatchMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Message);

	/**
	 * Process all queued commands.
	 *
	 * @see ProcessDelayedMessages
	 */
	void ProcessCommands();

	/**
	 * Processes all delayed messages.
	 *
	 * @see ProcessCommands
	 */
	void ProcessDelayedMessages();

protected:

	//~ FSingleThreadRunnable interface

	virtual void Tick() override;

private:

	/** Structure for delayed messages. */
	struct FSGDelayedMessage
	{
		/** Holds the context of the delayed message. */
		TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> Context;

		/** Holds a sequence number used by the delayed message queue. */
		int64 Sequence;


		/** Default constructor. */
		FSGDelayedMessage() { }

		/** Creates and initializes a new instance. */
		FSGDelayedMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& InContext, int64 InSequence)
			: Context(InContext)
			, Sequence(InSequence)
		{ }

		/** Comparison operator for heap sorting. */
		bool operator<(const FSGDelayedMessage& Other) const
		{
			const FTimespan Difference = Other.Context->GetTimeSent() - Context->GetTimeSent();

			if (Difference.IsZero())
			{
				return (Sequence < Other.Sequence);
			}

			return (Difference > FTimespan::Zero());
		}
	};

private:

	/** Handles adding message interceptors. */
	void HandleAddInterceptor(TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe> Interceptor, FName MessageType);

	/** Handles adding message recipients. */
	void HandleAddRecipient(FSGMessageAddress Address, TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> RecipientPtr);

	/** Handles adding of subscriptions. */
	void HandleAddSubscriber(TSharedRef<ISGMessageSubscription, ESPMode::ThreadSafe> Subscription);

	/** Handles the removal of message interceptors. */
	void HandleRemoveInterceptor(TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe> Interceptor, FName MessageType);

	/** Handles the removal of message recipients. */
	void HandleRemoveRecipient(FSGMessageAddress Address);

	/** Handles the removal of subscribers. */
	void HandleRemoveSubscriber(TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> SubscriberPtr, FName MessageType);

	/** Handles the routing of messages. */
	void HandleRouteMessage(TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> Context);

	/** Handles the addition of a listener. */
	void HandleAddListener(TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> ListenerPtr);

	/** Handles the removal of a listener. */
	void HandleRemoveListener(TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> ListenerPtr);

	/** Notify listeners about registration */
	void NotifyRegistration(const FSGMessageAddress& Address, ESGMessageBusNotification Notification);

private:

	/** Maps message types to interceptors. */
	TMap<FName, TArray<TSharedPtr<ISGMessageInterceptor, ESPMode::ThreadSafe>>> ActiveInterceptors;

	/** Maps message addresses to recipients. */
	TMap<FSGMessageAddress, TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>> ActiveRecipients;

	/** Maps message types to subscriptions. */
	TMap<FName, TArray<TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe>>> ActiveSubscriptions;

	/** Array of active registration listeners. */
	TArray<TWeakPtr<ISGBusListener, ESPMode::ThreadSafe>> ActiveRegistrationListeners;

	/** Holds the router command queue. */
	TQueue<CommandDelegate, EQueueMode::Mpsc> Commands;

	/** Holds the current time. */
	FDateTime CurrentTime;

	/** Holds the collection of delayed messages. */
	TArray<FSGDelayedMessage> DelayedMessages;

	/** Holds a sequence number for delayed messages. */
	int64 DelayedMessagesSequence;

	/** Holds a flag indicating that the thread is stopping. */
	TAtomic<bool> Stopping;

	/** Holds the message tracer. */
	TSharedRef<FSGMessageTracer, ESPMode::ThreadSafe> Tracer;

	/** Holds an event signaling that work is available. */
	FEvent* WorkEvent;

	/** Whether or not to allow delayed messaging */
	bool bAllowDelayedMessaging;
};
