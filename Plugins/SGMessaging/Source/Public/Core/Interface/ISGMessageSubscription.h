// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class ISGMessageReceiver;
enum class ESGMessageScope : uint8;

/**
 * Interface for message subscriptions.
 *
 * This interface provides access to a message subscription that was previously created with the
 * ISGMessageBus.Subscribe method. It can be used to query the subscription's details using the
 * various getters and modify its enabled state using the ISGMessageSubscription.Enable and
 * ISGMessageSubscription.Disable methods.
 *
 * A subscription that is disabled will cause a message endpoint to stop receiving messages for
 * the subscribed message type. However, the subscription is merely disabled and not removed.
 */
class ISGMessageSubscription
{
public:
	/**
	 * Disables the subscription.
	 *
	 * @see Enable, IsEnabled
	 */
	virtual void Disable() = 0;

	/**
	 * Enables the subscription.
	 *
	 * @see Disable, IsEnabled
	 */
	virtual void Enable() = 0;

	/**
	 * Gets the tag of subscribed messages.
	 *
	 * @return Message type.
	 * @see GetScopeRange, GetSubscriber
	 */
	virtual FName GetMessageTag() = 0;

	/**
	 * Gets the range of subscribed message scopes.
	 *
	 * @return Message scope range.
	 * @see GetMessageTag, GetSubscriber
	 */
	virtual const TRange<ESGMessageScope>& GetScopeRange() = 0;

	/**
	 * Gets the subscriber.
	 *
	 * @return The subscriber.
	 * @see GetMessageTag, GetScopeRange
	 */
	virtual const TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe>& GetSubscriber() = 0;

	/**
	 * Checks whether the subscription is enabled.
	 *
	 * @return true if the subscription is enabled, false otherwise.
	 * @see Enable, Disable
	 */
	virtual bool IsEnabled() = 0;

public:
	/** Virtual destructor. */
	virtual ~ISGMessageSubscription()
	{
	}
};
