// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ISGMessageContext.h"
#include "Templates/SharedPointer.h"

class FName;
class FString;
class ISGMessageAttachment;

enum class ESGMessageScope : uint8;

struct FDateTime;
struct FSGMessageAddress;


/**
 * Interface for mutable message contexts.
 */
class ISGMutableMessageContext
	: public ISGMessageContext
{
public:
	/**
	 * Adds a message address to the recipient list.
	 *
	 * @param Recipient The address of the recipient to add.
	 */
	virtual void AddRecipient(const FSGMessageAddress& Recipient) = 0;

	/**
	 * Sets the optional message attachment.
	 *
	 * @param InAttachment The attachment to set.
	 */
	virtual void SetAttachment(const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& InAttachment) = 0;

	/**
	 * Sets the date and time at which the message expires.
	 *
	 * @param InExpiration Expiration date and time.
	 */
	virtual void SetExpiration(const FDateTime& InExpiration) = 0;

	/**
	 * Sets the value of the header with the specified key.
	 *
	 * @param Key The header key.
	 * @param Value The header value.
	 */
	virtual void SetHeader(const FName& Key, const FString& Value) = 0;

	/**
	 * Sets the message scope.
	 *
	 * @param InScope The message scope.
	 */
	virtual void SetScope(ESGMessageScope InScope) = 0;

	/**
	 * Sets the address of the message's sender.
	 *
	 * @param InSender The message sender's address.
	 */
	virtual void SetSender(const FSGMessageAddress& InSender) = 0;

	/**
	 * Sets the date and time at which the message was sent.
	 *
	 * @param InTimeSent Send date and time.
	 */
	virtual void SetTimeSent(const FDateTime& InTimeSent) = 0;

public:
	/** Virtual destructor. */
	virtual ~ISGMutableMessageContext() override
	{
	}
};
