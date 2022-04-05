// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageContext.h"
#include "SGBlueprintMessageContext.generated.h"

USTRUCT(BlueprintType)
struct FSGBlueprintMessageAddress
{
	GENERATED_BODY()

	FSGBlueprintMessageAddress() = default;

	FSGBlueprintMessageAddress(const FSGMessageAddress& InAddress)
	{
		Address = InAddress;
	}

	operator FSGMessageAddress() const
	{
		return Address;
	}

	FSGMessageAddress Address;
};

UENUM(BlueprintType)
enum ESGBlueprintMessageFlags
{
	/** No special flags */
	None = 0,
	/** ESGMessageFlags::None */
	/** Guarantee that this message is delivered */
	Reliable = ESGMessageFlags::Reliable,
};

UENUM(BlueprintType)
enum ESGBlueprintMessageScope
{
	/** Deliver to subscribers in the same thread. */
	Thread = 0,
	/** ESGMessageScope::Thread */

	/** Deliver to subscribers in the same process. */
	Process = ESGMessageScope::Process,

	/** Deliver to subscribers on the network. */
	Network = ESGMessageScope::Network,

	/**
	 * Deliver to all subscribers.
	 *
	 * Note: This must be the last value in this enumeration.
	 */
	All = ESGMessageScope::All,
};

USTRUCT(BlueprintType)
struct FSGBlueprintMessageContext
{
	GENERATED_BODY()

	FSGBlueprintMessageContext() = default;

	~FSGBlueprintMessageContext()
	{
		if (Context.IsValid())
		{
			Context.Reset();
		}
	}

	FSGBlueprintMessageContext(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> InContext)
	{
		Context = InContext;
	}

	FSGMessageAddress GetSender() const
	{
		if (Context.IsValid())
		{
			return Context->GetSender();
		}

		return FSGMessageAddress();
	}

	operator TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>() const
	{
		return Context.ToSharedRef();
	}

private:
	TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> Context;
};
