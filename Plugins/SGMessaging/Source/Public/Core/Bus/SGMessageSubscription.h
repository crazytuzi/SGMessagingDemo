// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageSubscription.h"

class ISGMessageReceiver;

/**
 * Implements a message subscription.
 *
 * Message subscriptions are used by the message router to determine where to
 * dispatch published messages to. Message subscriptions are created per message
 * type.
 */
class FSGMessageSubscription final
	: public ISGMessageSubscription
{
public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InSubscriber The message subscriber.
	 * @param InMessageTag The type of messages to subscribe to.
	 * @param InScopeRange The message scope range to subscribe to.
	 */
	FSGMessageSubscription(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& InSubscriber,
	                       const FName& InMessageTag, const FSGMessageScopeRange& InScopeRange)
		: Enabled(true)
		  , MessageTag(InMessageTag)
		  , ScopeRange(InScopeRange)
		  , Subscriber(InSubscriber)
	{
	}

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

	virtual FName GetMessageTag() override
	{
		return MessageTag;
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
	FName MessageTag;

	/** Holds the range of message scopes to subscribe to. */
	FSGMessageScopeRange ScopeRange;

	/** Holds the subscriber. */
	TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> Subscriber;
};
