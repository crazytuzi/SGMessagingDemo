// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageAttachment.h"

/**
 * Implements a message context for messages sent through the message bus.
 *
 * Message contexts contain a message and additional data about that message,
 * such as when the message was sent, who sent it and where it is being sent to.
 */

class FSGMessageContext final
	: public ISGMessageContext
{
public:
	/** Default constructor. */
	FSGMessageContext()
		: Message(nullptr)
		  , Scope()
		  , Flags()
		  , SenderThread()
	{
	}

	FSGMessageContext(
		const FName& InMessageTag,
		void* InMessage,
		const TMap<FName, FString>& InAnnotations,
		const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& InAttachment,
		const FSGMessageAddress& InSender,
		const TArray<FSGMessageAddress>& InRecipients,
		const ESGMessageScope InScope,
		const ESGMessageFlags InFlags,
		const FDateTime& InTimeSent,
		const FDateTime& InExpiration,
		const ENamedThreads::Type InSenderThread
	)
		: Annotations(InAnnotations)
		  , Attachment(InAttachment)
		  , Expiration(InExpiration)
		  , MessageTag(InMessageTag)
		  , Message(InMessage)
		  , Recipients(InRecipients)
		  , Scope(InScope)
		  , Flags(InFlags)
		  , Sender(InSender)
		  , SenderThread(InSenderThread)
		  , TimeSent(InTimeSent)
	{
	}

	/**
	 * Creates and initializes a new message context from an existing context.
	 *
	 * This constructor overload is used for forwarded messages.
	 *
	 * @param InContext The existing context.
	 * @param InForwarder The forwarder's address.
	 * @param NewRecipients The recipients of the new context.
	 * @param NewScope The message's new scope.
	 * @param InTimeForwarded The time at which the message was forwarded.
	 * @param InForwarderThread The name of the thread from which the message was forwarded.
	 */
	FSGMessageContext(
		const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& InContext,
		const FSGMessageAddress& InForwarder,
		const TArray<FSGMessageAddress>& NewRecipients,
		const ESGMessageScope NewScope,
		const FDateTime& InTimeForwarded,
		const ENamedThreads::Type InForwarderThread
	)
		: Message(nullptr)
		  , OriginalContext(InContext)
		  , Recipients(NewRecipients)
		  , Scope(NewScope)
		  , Flags(ESGMessageFlags::None)
		  , Sender(InForwarder)
		  , SenderThread(InForwarderThread)
		  , TimeSent(InTimeForwarded)
	{
	}

	/** Destructor. */
	virtual ~FSGMessageContext() override;

public:
	//~ ISGMessageContext interface

	virtual const TMap<FName, FString>& GetAnnotations() const override;
	virtual TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> GetAttachment() const override;
	virtual const FDateTime& GetExpiration() const override;
	virtual const void* GetMessage() const override;
	virtual TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> GetOriginalContext() const override;
	virtual const TArray<FSGMessageAddress>& GetRecipients() const override;
	virtual ESGMessageScope GetScope() const override;
	virtual ESGMessageFlags GetFlags() const override;
	virtual const FSGMessageAddress& GetSender() const override;
	virtual const FSGMessageAddress& GetForwarder() const override;
	virtual ENamedThreads::Type GetSenderThread() const override;
	virtual const FDateTime& GetTimeForwarded() const override;
	virtual const FDateTime& GetTimeSent() const override;
	virtual FName GetMessageTag() const override;

private:
	/** Holds the optional message annotations. */
	TMap<FName, FString> Annotations;

	/** Holds a pointer to attached binary data. */
	TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> Attachment;

	/** Holds the expiration time. */
	FDateTime Expiration;

	FName MessageTag;

	/** Holds the message. */
	void* Message;

	/** Holds the original message context. */
	TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> OriginalContext;

	/** Holds the message recipients. */
	TArray<FSGMessageAddress> Recipients;

	/** Holds the message's scope. */
	ESGMessageScope Scope;

	/** Holds the message's scope. */
	ESGMessageFlags Flags;

	/** Holds the sender's identifier. */
	FSGMessageAddress Sender;

	/** Holds the name of the thread from which the message was sent. */
	ENamedThreads::Type SenderThread;

	/** Holds the time at which the message was sent. */
	FDateTime TimeSent;
};
