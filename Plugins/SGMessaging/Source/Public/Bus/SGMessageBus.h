// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ISGMessageContext.h"
#include "Interface/ISGMessageSubscription.h"
#include "Interface/ISGMessageAttachment.h"
#include "Interface/ISGMessageInterceptor.h"
#include "Interface/ISGAuthorizeMessageRecipients.h"
#include "Interface/ISGMessageTracer.h"
#include "Interface/ISGMessageBus.h"

class FSGMessageRouter;
class ISGMessageReceiver;
class ISGMessageSender;

/**
 * Implements a message bus.
 */
class FSGMessageBus
	: public TSharedFromThis<FSGMessageBus, ESPMode::ThreadSafe>
	, public ISGMessageBus
{
public:
	
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InDebugName The debug name of this message bus.
	 * @param InRecipientAuthorizer An optional recipient authorizer.
	 */
	FSGMessageBus(FString InName, const TSharedPtr<ISGAuthorizeMessageRecipients>& InRecipientAuthorizer);

	/** Virtual destructor. */
	virtual ~FSGMessageBus();

public:

	//~ ISGMessageBus interface

	virtual void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const TArray<FSGMessageAddress>& Recipients, const FTimespan& Delay, const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Forwarder) override;
	virtual TSharedRef<ISGMessageTracer, ESPMode::ThreadSafe> GetTracer() override;
	virtual void Intercept(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType) override;
	virtual FOnMessageBusShutdown& OnShutdown() override;
	virtual void Publish(void* Message, UScriptStruct* TypeInfo, ESGMessageScope Scope, const TMap<FName, FString>& Annotations, const FTimespan& Delay, const FDateTime& Expiration, const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Publisher) override;
	virtual void Register(const FSGMessageAddress& Address, const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient) override;
	virtual void Send(void* Message, UScriptStruct* TypeInfo, ESGMessageFlags Flags, const TMap<FName, FString>& Annotations, const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& Attachment, const TArray<FSGMessageAddress>& Recipients, const FTimespan& Delay, const FDateTime& Expiration, const TSharedRef<ISGMessageSender, ESPMode::ThreadSafe>& Sender) override;
	virtual void Shutdown() override;
	virtual TSharedPtr<ISGMessageSubscription, ESPMode::ThreadSafe> Subscribe(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber, const FName& MessageType, const FSGMessageScopeRange& ScopeRange) override;
	virtual void Unintercept(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor, const FName& MessageType) override;
	virtual void Unregister(const FSGMessageAddress& Address) override;
	virtual void Unsubscribe(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber, const FName& MessageType) override;

	virtual void AddNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener) override;
	virtual void RemoveNotificationListener(const TSharedRef<ISGBusListener, ESPMode::ThreadSafe>& Listener) override;
	virtual const FString& GetName() const override;

private:
	/** The message bus debugging name. */
	const FString Name;

	/** Holds the message router. */
	FSGMessageRouter* Router;

	/** Holds the message router thread. */
	FRunnableThread* RouterThread;

	/** Holds the recipient authorizer. */
	TSharedPtr<ISGAuthorizeMessageRecipients> RecipientAuthorizer;

	/** Holds bus shutdown delegate. */
	FOnMessageBusShutdown ShutdownDelegate;
};
