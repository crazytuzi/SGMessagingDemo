// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/Interface/ISGMessageBridge.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageReceiver.h"
#include "Core/Interface/ISGMessageSender.h"
#include "Core/Interface/ISGMessageTransportHandler.h"
#include "Misc/Guid.h"
#include "Templates/SharedPointer.h"
#include "Core/Bridge/SGMessageAddressBook.h"

class ISGMessageBus;
class ISGMessageSubscription;
class ISGMessageTransport;


/**
 * Implements a message bridge.
 *
 * A message bridge is a special message endpoint that connects multiple message buses
 * running in different processes or on different devices. This allows messages that are
 * available in one system to also be available on other systems.
 *
 * Message bridges use an underlying transport layer to channel the messages between two
 * or more systems. Such layers may utilize system specific technologies, such as network
 * sockets or shared memory to communicate with remote bridges. The bridge acts as a map
 * from message addresses to remote nodes and vice versa.
 *
 * @see ISGMessageBus, ISGMessageTransport
 */
class FSGMessageBridge final
	: public TSharedFromThis<FSGMessageBridge, ESPMode::ThreadSafe>
	  , public ISGMessageBridge
	  , public ISGMessageReceiver
	  , public ISGMessageSender
	  , protected ISGMessageTransportHandler
{
public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InAddress The message address for this bridge.
	 * @param InBus The message bus that this node is connected to.
	 * @param InTransport The transport mechanism to use.
	 */
	FSGMessageBridge(
		const FSGMessageAddress InAddress,
		const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& InBus,
		const TSharedRef<ISGMessageTransport, ESPMode::ThreadSafe>& InTransport
	);

	/** Virtual destructor. */
	virtual ~FSGMessageBridge() override;

public:
	//~ ISGMessageBridge interface

	virtual void Disable() override;
	virtual void Enable() override;
	virtual bool IsEnabled() const override;

public:
	//~ ISGMessageReceiver interface

	virtual FName GetDebugName() const override;
	virtual const FGuid& GetRecipientId() const override;
	virtual ENamedThreads::Type GetRecipientThread() const override;
	virtual bool IsLocal() const override;
	virtual void ReceiveMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override;

public:
	//~ ISGMessageSender interface

	virtual FSGMessageAddress GetSenderAddress() override;
	virtual void NotifyMessageError(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	                                const FString& Error) override;

protected:
	//~ ISGMessageTransportHandler interface

	virtual void DiscoverTransportNode(const FGuid& NodeId) override;
	virtual void ForgetTransportNode(const FGuid& NodeId) override;
	virtual void ReceiveTransportMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	                                     const FGuid& NodeId) override;

private:
	/** Callback for message bus shutdowns. */
	void HandleMessageBusShutdown();

private:
	/** Holds the bridge's address. */
	FSGMessageAddress Address;

	/** Holds the address book. */
	FSGMessageAddressBook AddressBook;

	/** Holds a reference to the bus that this bridge is attached to. */
	TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus;

	/** Hold a flag indicating whether this endpoint is active. */
	bool Enabled;

	/** Holds the bridge's unique identifier (for debugging purposes). */
	const FGuid Id;

	/** Holds the message subscription for outbound messages. */
	TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe> MessageSubscription;

	/** Holds the message transport object. */
	TSharedPtr<ISGMessageTransport, ESPMode::ThreadSafe> Transport;
};
