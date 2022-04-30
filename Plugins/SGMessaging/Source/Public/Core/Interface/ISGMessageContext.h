// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Async/TaskGraphInterfaces.h"
#include "Containers/Array.h"
#include "Misc/Crc.h"
#include "Misc/Guid.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class ISGMessageAttachment;

struct FDateTime;


/**
 * Structure for message endpoint addresses.
 */
struct FSGMessageAddress
{
public:
	/** Default constructor (no initialization). */
	FSGMessageAddress()
	{
	}

public:
	/**
	 * Compares two message addresses for equality.
	 *
	 * @param X The first address to compare.
	 * @param Y The second address to compare.
	 * @return true if the addresses are equal, false otherwise.
	 */
	friend bool operator==(const FSGMessageAddress& X, const FSGMessageAddress& Y)
	{
		return (X.UniqueId == Y.UniqueId);
	}

	/**
	 * Compares two message addresses for inequality.
	 *
	 * @param X The first address to compare.
	 * @param Y The second address to compare.
	 * @return true if the addresses are not equal, false otherwise.
	 */
	friend bool operator!=(const FSGMessageAddress& X, const FSGMessageAddress& Y)
	{
		return (X.UniqueId != Y.UniqueId);
	}

	/**
	 * Serializes a message address from or into an archive.
	 *
	 * @param Ar The archive to serialize from or into.
	 * @param A The address to serialize.
	 */
	friend FArchive& operator<<(FArchive& Ar, FSGMessageAddress& A)
	{
		return Ar << A.UniqueId;
	}

public:
	/**
	 * Invalidates the GUID.
	 *
	 * @see IsValid
	 */
	void Invalidate()
	{
		UniqueId.Invalidate();
	}

	/**
	 * Checks whether this message address is valid or not.
	 *
	 * @return true if valid, false otherwise.
	 * @see Invalidate
	 */
	bool IsValid() const
	{
		return UniqueId.IsValid();
	}

	/**
	 * Converts this GUID to its string representation.
	 *
	 * @return The string representation.
	 * @see Parse
	 */
	FString ToString() const
	{
		return UniqueId.ToString();
	}

public:
	/**
	 * Calculates the hash for a message address.
	 *
	 * @param Address The address to calculate the hash for.
	 * @return The hash.
	 */
	friend uint32 GetTypeHash(const FSGMessageAddress& Address)
	{
		return FCrc::MemCrc_DEPRECATED(&Address.UniqueId, sizeof(FGuid));
	}

public:
	/**
	 * Returns a new message address.
	 *
	 * @return A new address.
	 */
	static FSGMessageAddress NewAddress()
	{
		FSGMessageAddress Result;
		Result.UniqueId = FGuid::NewGuid();

		return Result;
	}

	/**
	 * Converts a string to a message address.
	 *
	 * @param String The string to convert.
	 * @param OutAddress Will contain the parsed address.
	 * @return true if the string was converted successfully, false otherwise.
	 * @see ToString
	 */
	static bool Parse(const FString& String, FSGMessageAddress& OutAddress)
	{
		return FGuid::Parse(String, OutAddress.UniqueId);
	}

private:
	/** Holds a unique identifier. */
	FGuid UniqueId;
};


/**
 * Enumerates scopes for published messages.
 *
 * The scope determines to which endpoints a published message will be delivered.
 * By default, messages will be published to everyone on the local network (Subnet),
 * but it is often useful to restrict the group of recipients to more local scopes,
 * or to widen it to a larger audience outside the local network.
 *
 * For example, if a message is to be handled only by subscribers in the same application,
 * the scope should be set to Process. If messages need to be published between
 * different networks (i.e. between LAN and WLAN), it should be set to Network instead.
 * 
 * Scopes only apply to published messages. Messages that are being sent to specific
 * recipients will always be delivered, regardless of the endpoint locations.
 */
enum class ESGMessageScope : uint8
{
	/** Deliver to subscribers in the same thread. */
	Thread,

	/** Deliver to subscribers in the same process. */
	Process,

	/** Deliver to subscribers on the network. */
	Network,

	/**
	 * Deliver to all subscribers.
	 *
	 * Note: This must be the last value in this enumeration.
	 */
	All
};

enum class ESGMessageFlags : uint32
{
	/** No special flags */
	None = 0,
	/** Guarantee that this message is delivered */
	Reliable = 1 << 0,
};

ENUM_CLASS_FLAGS(ESGMessageFlags);


/** Type definition for message scope ranges. */
typedef TRange<ESGMessageScope> FSGMessageScopeRange;

/** Type definition for message scope range bounds. */
typedef TRangeBound<ESGMessageScope> FSGMessageScopeRangeBound;


/**
 * Interface for message contexts.
 *
 * Messages are delivered inside message contexts, which store the message itself plus additional data
 * associated with the message. Recipients of a message are usually interested in data that describes the
 * message, such as its origin or when it expires. They may also be interested in optional out-of-band
 * binary data that is attached to the message.
 *
 * The sender's address (ISGMessageContext.GetSender) is often needed to send a reply message to a message sender, i.e.
 * in response to a published message. The message attachment (ISGMessageContext.GetAttachment) is an optional bundle
 * of binary bulk data that is transferred independently from the message itself and allows for transferring
 * larger amounts of data that would otherwise clog up the messaging system.
 *
 * In case a message was forwarded by another endpoint, the context of the original sender can be accessed
 * using the ISGMessageContext.GetOriginalContext method.
 *
 * @see ISGMessageAttachment
 */
class ISGMessageContext
{
public:
	/**
	 * Gets the optional message annotations.
	 *
	 * @return Message header collection.
	 */
	virtual const TMap<FName, FString>& GetAnnotations() const = 0;

	/**
	 * Gets the message attachment, if present.
	 *
	 * @return A pointer to the message attachment, or nullptr if no attachment is present.
	 */
	virtual TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> GetAttachment() const = 0;

	/**
	 * Gets the date and time at which the message expires.
	 *
	 * @return Expiration time.
	 */
	virtual const FDateTime& GetExpiration() const = 0;

	/**
	 * Gets the message data.
	 *
	 * @return A pointer to the message data.
	 * @see GetAttachment, GetMessageTag, GetMessageTypeInfo
	 */
	virtual const void* GetMessage() const = 0;

	/**
	 * Returns the original message context in case the message was forwarded.
	 *
	 * @return The original message context, or nullptr if the message wasn't forwarded.
	 */
	virtual TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> GetOriginalContext() const = 0;

	/**
	 * Gets the list of message recipients.
	 *
	 * @return Message recipients.
	 * @see GetSender
	 */
	virtual const TArray<FSGMessageAddress>& GetRecipients() const = 0;

	/**
	 * Gets the scope to which the message was sent.
	 *
	 * @return The message scope.
	 */
	virtual ESGMessageScope GetScope() const = 0;

	/**
	* Gets the scope to which the message was sent.
	*
	* @return The message scope.
	*/
	virtual ESGMessageFlags GetFlags() const = 0;

	/**
	 * Gets the sender's address.
	 *
	 * @return Sender address.
	 * @see GetRecipients, GetSenderThread
	 */
	virtual const FSGMessageAddress& GetSender() const = 0;

	/**
	 * Gets the forwarder's address.
	 * @note Identical to GetSender if the message wasn't forwarded.
	 * @return Forwarder address.
	 * @see GetSender, GetRecipients, GetSenderThread
	 */
	virtual const FSGMessageAddress& GetForwarder() const = 0;

	/**
	 * Gets the name of the thread from which the message was sent.
	 *
	 * @return Sender threat name.
	 * @see GetSender
	 */
	virtual ENamedThreads::Type GetSenderThread() const = 0;

	/**
	 * Gets the time at which the message was forwarded.
	 *
	 * @return Time forwarded.
	 * @see GetTimeSent, IsForwarded
	 */
	virtual const FDateTime& GetTimeForwarded() const = 0;

	/**
	 * Gets the time at which the message was sent.
	 *
	 * @return Time sent.
	 * @see GetTimeForwarded
	 */
	virtual const FDateTime& GetTimeSent() const = 0;

	/**
	 * Gets the name of the message type.
	 *
	 * @return Message type name.
	 * @see GetMessage, GetMessageTypeInfo
	 */
	virtual FName GetMessageTag() const = 0;

public:
	/**
	 * Checks whether this is a forwarded message.
	 *
	 * @return true if the message was forwarded, false otherwise.
	 * @see GetOriginalContext, GetTimeForwarded, IsValid
	 */
	bool IsForwarded() const
	{
		return GetOriginalContext().IsValid();
	}

	/**
	 * Checks whether this context is valid.
	 *
	 * @return true if the context is valid, false otherwise.
	 * @see IsForwarded
	 */
	bool IsValid() const
	{
		return GetMessage() != nullptr;
	}

public:
	/** Virtual destructor. */
	virtual ~ISGMessageContext()
	{
	}
};
