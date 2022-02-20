// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ISGMessageSubscription.h"

class ISGMessageReceiver;

/**
 * Implements a message subscription.
 *
 * Message subscriptions are used by the message router to determine where to
 * dispatch published messages to. Message subscriptions are created per message
 * type.
 */
class FSGMessageSubscription
	: public ISGMessageSubscription
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InSubscriber The message subscriber.
	 * @param InMessageType The type of messages to subscribe to.
	 * @param InReceivingThread The thread on which to receive messages on.
	 * @param InScopeRange The message scope range to subscribe to.
	 */
	FSGMessageSubscription(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& InSubscriber, const FName& InMessageType, const FSGMessageScopeRange& InScopeRange)
		: Enabled(true)
		, MessageType(InMessageType)
		, ScopeRange(InScopeRange)
		, Subscriber(InSubscriber)
	{ }

public:

	//~ ISGMessageSubscription interface

	virtual void Disable() override
	{
		Enabled = false;
	}

	virtual void Enable() override
	{
		Enabled = true;
	}

	virtual FName GetMessageType() override
	{
		return MessageType;
	}

	virtual const FSGMessageScopeRange& GetScopeRange() override
	{
		return ScopeRange;
	}

	virtual const TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>& GetSubscriber() override
	{
		return Subscriber;
	}

	virtual bool IsEnabled() override
	{
		return Enabled;
	}

private:

	/** Holds a flag indicating whether this subscription is enabled. */
	bool Enabled;

	/** Holds the type of subscribed messages. */
	FName MessageType;

	/** Holds the range of message scopes to subscribe to. */
	FSGMessageScopeRange ScopeRange;

	/** Holds the subscriber. */
	TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> Subscriber;
};
