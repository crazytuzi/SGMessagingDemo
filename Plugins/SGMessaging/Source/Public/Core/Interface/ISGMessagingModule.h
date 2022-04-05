// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Delegates/Delegate.h"

class ISGAuthorizeMessageRecipients;
class ISGMessageBridge;
class ISGMessageBus;
class ISGMessageTransport;

struct FSGMessageAddress;

/** Log category for message bus code */
DECLARE_LOG_CATEGORY_EXTERN(LogSGMessaging, Log, All);

/**
 * Interface for messaging modules.
 *
 * @see ISGMessageBridge, ISGMessageBus
 */
class ISGMessagingModule
	: public IModuleInterface
{
	using FSGMessageBusWeakPtr = TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe>;

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMessageBusStartupOrShutdown, FSGMessageBusWeakPtr);
	
	/** Event triggered when a message bus is started. */
	virtual FOnMessageBusStartupOrShutdown& OnMessageBusStartup() = 0;

	/** Event triggered when a message bus is shutdown. */
	virtual FOnMessageBusStartupOrShutdown& OnMessageBusShutdown() = 0;

	/**
	 * Creates a new message bridge.
	 *
	 * Message bridges translate messages between a message bus and another means of
	 * message transportation, such as network sockets.
	 *
	 * @param Address The bridge's address on the message bus.
	 * @param Bus The message bus to attach the bridge to.
	 * @param Transport The message transport technology to use.
	 * @return The new message bridge, or nullptr if the bridge couldn't be created.
	 * @see CreateBus
	 */
	virtual TSharedPtr<ISGMessageBridge, ESPMode::ThreadSafe> CreateBridge(const FSGMessageAddress& Address, const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& Bus, const TSharedRef<ISGMessageTransport, ESPMode::ThreadSafe>& Transport) = 0;

	/**
	 * Creates a new message bus.
	 *
	 * @param RecipientAuthorizer An optional recipient authorizer.
	 * @return The new message bus, or nullptr if the bus couldn't be created.
	 * @see CreateBridge
	 */
	virtual TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> CreateBus(const TSharedPtr<ISGAuthorizeMessageRecipients>& RecipientAuthorizer = nullptr) = 0;

	/**
	 * Creates a new message bus.
	 *
	 * @param InName The name of this message bus.
	 * @param RecipientAuthorizer An optional recipient authorizer.
	 * @return The new message bus, or nullptr if the bus couldn't be created.
	 * @see CreateBridge
	 */
	virtual TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> CreateBus(FString InName, const TSharedPtr<ISGAuthorizeMessageRecipients>& RecipientAuthorizer = nullptr) = 0;

	/**
	 * Gets all message buses that were created by this module via the #CreateBus method
	 * that are still valid.
	 *
	 * @return All message buses.
	 */
	virtual TArray<TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>> GetAllBuses() const = 0;

public:

	/**
	 * Gets a reference to the messaging module instance.
	 *
	 * @return A reference to the Messaging module.
	 * @todo gmp: better implementation using dependency injection.
	 */
	static ISGMessagingModule& Get()
	{
#if PLATFORM_IOS
        static ISGMessagingModule& MessageModule = FModuleManager::LoadModuleChecked<ISGMessagingModule>("SGMessaging");
        return MessageModule;
#else
        return FModuleManager::LoadModuleChecked<ISGMessagingModule>("SGMessaging");
#endif
	}

public:

	/** Virtual destructor. */
	virtual ~ISGMessagingModule() { }
};
