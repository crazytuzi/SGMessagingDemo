// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Bus/SGMessageContext.h"


/* FSGMessageContext structors
 *****************************************************************************/

FSGMessageContext::~FSGMessageContext()
{
	if (Message != nullptr)
	{
		FMemory::Free(Message);
	}
}


/* ISGMessageContext interface
 *****************************************************************************/

const TMap<FName, FString>& FSGMessageContext::GetAnnotations() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetAnnotations();
	}

	return Annotations;
}


TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> FSGMessageContext::GetAttachment() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetAttachment();
	}

	return Attachment;
}


const FDateTime& FSGMessageContext::GetExpiration() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetExpiration();
	}

	return Expiration;
}


const void* FSGMessageContext::GetMessage() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetMessage();
	}

	return Message;
}


TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> FSGMessageContext::GetOriginalContext() const
{
	return OriginalContext;
}

const TArray<FSGMessageAddress>& FSGMessageContext::GetRecipients() const
{
	return Recipients;
}


ESGMessageScope FSGMessageContext::GetScope() const
{
	return Scope;
}

ESGMessageFlags FSGMessageContext::GetFlags() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetFlags();
	}

	return Flags;
}

const FSGMessageAddress& FSGMessageContext::GetSender() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetSender();
	}

	return Sender;
}


const FSGMessageAddress& FSGMessageContext::GetForwarder() const
{
	return Sender;
}


ENamedThreads::Type FSGMessageContext::GetSenderThread() const
{
	return SenderThread;
}


const FDateTime& FSGMessageContext::GetTimeForwarded() const
{
	return TimeSent;
}


const FDateTime& FSGMessageContext::GetTimeSent() const
{
	return TimeSent;
}

FName FSGMessageContext::GetMessageTag() const
{
	if (OriginalContext.IsValid())
	{
		return OriginalContext->GetMessageTag();
	}

	return MessageTag;
}
