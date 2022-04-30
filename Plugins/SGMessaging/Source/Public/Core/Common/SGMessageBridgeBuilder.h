// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageBridge.h"
#include "Core/Interface/ISGMessageBus.h"
#include "Core/Interface/ISGMessageTransport.h"
#include "Core/Interface/ISGMessagingModule.h"


/**
 * Implements a message bridge builder.
 */
class FSGMessageBridgeBuilder
{
public:
	/** Default constructor. */
	FSGMessageBridgeBuilder()
		: Address(FSGMessageAddress::NewAddress())
		  , BusPtr(nullptr)
		  , Disabled(false)
		  , Transport(nullptr)
	{
	}

public:
	/**
	 * Disables the bridge.
	 *
	 * @return This instance (for method chaining).
	 */
	FSGMessageBridgeBuilder& ThatIsDisabled()
	{
		Disabled = true;

		return *this;
	}

	/**
	 * Configures the bridge to use a specific message transport technology.
	 *
	 * @param InTransport The transport technology to use.
	 * @return This instance (for method chaining).
	 */
	FSGMessageBridgeBuilder& UsingTransport(const TSharedRef<ISGMessageTransport, ESPMode::ThreadSafe>& InTransport)
	{
		Transport = InTransport;

		return *this;
	}

	/**
	 * Sets the bridge's address.
	 *
	 * If no address is specified, one will be generated automatically.
	 *
	 * @param InAddress The address to set.
	 * @return This instance (for method chaining).
	 */
	FSGMessageBridgeBuilder& WithAddress(const FSGMessageAddress& InAddress)
	{
		Address = InAddress;

		return *this;
	}

public:
	/**
	 * Builds the message bridge as configured.
	 *
	 * @return A new message bridge, or nullptr if it couldn't be built.
	 */
	TSharedPtr<ISGMessageBridge, ESPMode::ThreadSafe> Build() const
	{
		TSharedPtr<ISGMessageBridge, ESPMode::ThreadSafe> Bridge;

		check(Transport.IsValid());

		if (const auto Bus = BusPtr.Pin())
		{
			Bridge = ISGMessagingModule::Get().CreateBridge(Address, Bus.ToSharedRef(), Transport.ToSharedRef());

			if (Bridge.IsValid())
			{
				if (Disabled)
				{
					Bridge->Disable();
				}
				else
				{
					Bridge->Enable();
				}
			}
		}

		return Bridge;
	}

	/**
	 * Implicit conversion operator to build the message bridge as configured.
	 *
	 * @return A new message bridge, or nullptr if it couldn't be built.
	 */
	explicit operator TSharedPtr<ISGMessageBridge, ESPMode::ThreadSafe>() const
	{
		return Build();
	}

private:
	/** Holds the bridge's address. */
	FSGMessageAddress Address;

	/** Holds a weak pointer to the message bus to attach to. */
	TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> BusPtr;

	/** Holds a flag indicating whether the bridge should be disabled. */
	bool Disabled;

	/** Holds a reference to the message transport technology. */
	TSharedPtr<ISGMessageTransport, ESPMode::ThreadSafe> Transport;
};
