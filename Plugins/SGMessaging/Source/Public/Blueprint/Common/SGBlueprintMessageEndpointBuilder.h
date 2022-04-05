// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Async/TaskGraphInterfaces.h"
#include "Core/Interface/ISGMessageBus.h"
#include "Core/Interface/ISGMessagingModule.h"
#include "SGBlueprintMessageEndpoint.h"


/**
 * Implements a message endpoint builder.
 */
struct FSGBlueprintMessageEndpointBuilder
{
public:
	/**
	 * Creates and initializes a new builder using the default message bus.
	 *
	 * WARNING: This constructor must be called from the Game thread.
	 *
	 * @param InName The endpoint's name (for debugging purposes).
	 * @param InBus The message bus to attach the endpoint to.
	 */
	FSGBlueprintMessageEndpointBuilder(const FName& InName)
		: Outer(nullptr)
		  , BusPtr(nullptr)
		  , Disabled(false)
		  , InboxEnabled(false)
		  , Name(InName)
		  , RecipientThread(FTaskGraphInterface::Get().GetCurrentThreadIfKnown())
	{
	}

	/**
	 * Creates and initializes a new builder using the specified message bus.
	 *
	 * @param InName The endpoint's name (for debugging purposes).
	 * @param InBus The message bus to attach the endpoint to.
	 */
	FSGBlueprintMessageEndpointBuilder(UObject* InOuter, const FName& InName,
	                                   const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& InBus)
		: Outer(InOuter)
		  , BusPtr(InBus)
		  , Disabled(false)
		  , InboxEnabled(false)
		  , Name(InName)
		  , RecipientThread(FTaskGraphInterface::Get().GetCurrentThreadIfKnown())
	{
	}

public:
	FSGBlueprintMessageEndpointBuilder& NotificationHandling(FOnBusNotification&& InHandler)
	{
		OnNotification = MoveTemp(InHandler);
		return *this;
	}

	/**
	 * Configures the endpoint to receive messages on any thread.
	 *
	 * By default, the builder initializes the message endpoint to receive on the
	 * current thread. Use this method to receive on any available thread instead.
	 *
	 * ThreadAny is the fastest way to receive messages. It should be used if the receiving
	 * code is completely thread-safe and if it is sufficiently fast. ThreadAny MUST NOT
	 * be used if the receiving code is not thread-safe. It also SHOULD NOT be used if the
	 * code includes time consuming operations, because it will block the message router,
	 * causing no other messages to be delivered in the meantime.
	 *
	 * @return This instance (for method chaining).
	 * @see ReceivingOnThread
	 */
	FSGBlueprintMessageEndpointBuilder& ReceivingOnAnyThread()
	{
		RecipientThread = ENamedThreads::AnyThread;

		return *this;
	}

	/**
	 * Configured the endpoint to receive messages on a specific thread.
	 *
	 * By default, the builder initializes the message endpoint to receive on the
	 * current thread. Use this method to receive on a different thread instead.
	 *
	 * Also see the additional notes for ReceivingOnAnyThread().
	 *
	 * @param NamedThread The name of the thread to receive messages on.
	 * @return This instance (for method chaining).
	 * @see ReceivingOnAnyThread
	 */
	FSGBlueprintMessageEndpointBuilder& ReceivingOnThread(ENamedThreads::Type NamedThread)
	{
		RecipientThread = NamedThread;

		return *this;
	}

	/**
	 * Disables the endpoint.
	 *
	 * @return This instance (for method chaining).
	 */
	FSGBlueprintMessageEndpointBuilder& ThatIsDisabled()
	{
		Disabled = true;

		return *this;
	}

	/**
	 * Enables the endpoint's message inbox.
	 *
	 * The inbox is disabled by default.
	 *
	 * @return This instance (for method chaining).
	 */
	FSGBlueprintMessageEndpointBuilder& WithInbox()
	{
		InboxEnabled = true;

		return *this;
	}

public:
	/**
	 * Builds the message endpoint as configured.
	 *
	 * @return A new message endpoint, or nullptr if it couldn't be built.
	 */
	USGBlueprintMessageEndpoint* Build()
	{
		const auto ScriptMessageEndpoint = NewObject<USGBlueprintMessageEndpoint>(Outer);

		// TSharedPtr<FSGMessageEndpoint, ESPMode::ThreadSafe> Endpoint = MakeShared<FSGMessageEndpoint, ESPMode::ThreadSafe>();

		ScriptMessageEndpoint->MessageEndpoint = nullptr;

		TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = BusPtr.Pin();

		if (Bus.IsValid())
		{
			// TSharedPtr<FSGMessageEndpoint, ESPMode::ThreadSafe> Endpoint;

			TSharedPtr<FSGMessageEndpoint, ESPMode::ThreadSafe> Endpoint = MakeShared<
				FSGMessageEndpoint, ESPMode::ThreadSafe>(Name, Bus.ToSharedRef(), OnNotification);

			ScriptMessageEndpoint->MessageEndpoint = Endpoint;

			Bus->Register(Endpoint->GetAddress(), Endpoint.ToSharedRef());

			if (OnNotification.IsBound())
			{
				Bus->AddNotificationListener(Endpoint.ToSharedRef());
			}

			if (Disabled)
			{
				Endpoint->Disable();
			}

			if (InboxEnabled)
			{
				Endpoint->EnableInbox();
				Endpoint->SetRecipientThread(ENamedThreads::AnyThread);
			}
			else
			{
				Endpoint->SetRecipientThread(RecipientThread);
			}
		}

		return ScriptMessageEndpoint;
	}

	/**
	 * Implicit conversion operator to build the message endpoint as configured.
	 *
	 * @return The message endpoint.
	 */
	operator USGBlueprintMessageEndpoint*()
	{
		return Build();
	}

private:
	UObject* Outer;

	/** Holds a reference to the message bus to attach to. */
	TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> BusPtr;

	/** Holds a flag indicating whether the endpoint should be disabled. */
	bool Disabled;

	/** Holds a delegate to invoke on disconnection event. */
	FOnBusNotification OnNotification;

	/** Holds a flag indicating whether the inbox should be enabled. */
	bool InboxEnabled;

	/** Holds the endpoint's name (for debugging purposes). */
	FName Name;

	/** Holds the name of the thread on which to receive messages. */
	ENamedThreads::Type RecipientThread;
};
