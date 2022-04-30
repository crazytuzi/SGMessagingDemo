// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"

class ISGMessageContext;


/**
 * Interface for message handlers.
 */
class ISGMessageHandler
{
public:
	virtual uint32 GetHash() const = 0;

	/**
	 * Handles the specified message.
	 *
	 * @param Context The context of the message to handle.
	 */
	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) = 0;

public:
	/** Virtual destructor. */
	virtual ~ISGMessageHandler()
	{
	}
};
