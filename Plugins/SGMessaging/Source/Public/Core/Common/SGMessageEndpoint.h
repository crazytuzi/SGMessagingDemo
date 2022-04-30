// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Async/TaskGraphInterfaces.h"
#include "Containers/Array.h"
#include "Containers/ArrayBuilder.h"
#include "Containers/Queue.h"
#include "Core/Interface/ISGMessageBus.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageHandler.h"
#include "Core/Interface/ISGMessageReceiver.h"
#include "Core/Interface/ISGMessageSender.h"
#include "Core/Interface/ISGMessageBusListener.h"
#include "SGMessageHandlers.h"
#include "Core/Message/SGMessage.h"
#include "Core/Message/SGMessageBuilder.h"
#include "Core/Message/SGMessageParameter.h"
#include "Core/Message/SGMessageTagBuilder.h"
#include "Misc/Guid.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"
#include "Misc/ScopeLock.h"


/**
 * DEPRECATED: Delegate type for error notifications.
 *
 * The first parameter is the context of the sent message (only valid for the duration of the callback).
 * The second parameter is the error string.
 */
DECLARE_DELEGATE_TwoParams(FOnMessageEndpointError, const ISGMessageContext&, const FString&);


/**
 * Struct to propagate message bus notifications
 */
struct FSGMessageBusNotification
{
	/** Notification type. */
	ESGMessageBusNotification NotificationType;

	/** Address of the un/registered. */
	FSGMessageAddress RegistrationAddress;
};

/** Delegate type for SGMessageBus notifications. */
DECLARE_DELEGATE_OneParam(FOnBusNotification, const FSGMessageBusNotification&);

/**
 * Implements a message endpoint for sending and receiving messages on a message bus.
 *
 * This class provides a convenient implementation of the ISGMessageReceiver and ISGMessageSender interfaces,
 * which allow consumers to send and receive messages on a message bus. The endpoint allows for receiving
 * messages asynchronously as they arrive, as well as synchronously through an inbox that can be polled.
 *
 * By default, messages are received synchronously on the thread that the endpoint was created on.
 * If the message consumer is thread-safe, a more efficient message dispatch can be enabled by calling
 * the SetRecipientThread() method with ENamedThreads::AnyThread.
 *
 * Endpoints that are destroyed or receive messages on non-Game threads should use the static function
 * FMessageEndpoint::SafeRelease() to dispose of the endpoint. This will ensure that there are no race
 * conditions between endpoint destruction and the receiving of messages.
 *
 * The underlying message bus will take ownership of all sent and published message objects. The memory
 * held by the messages must therefore NOT be freed by the caller.
 */
class FSGMessageEndpoint final
	: public TSharedFromThis<FSGMessageEndpoint, ESPMode::ThreadSafe>
	  , public ISGMessageReceiver
	  , public ISGMessageSender
	  , public ISGBusListener
{
public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InName The endpoint's name (for debugging purposes).
	 * @param InBus The message bus to attach this endpoint to.
	 * @param InNotificationDelegate The collection of message handlers to register.
	 */
	FSGMessageEndpoint(const FName& InName, const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& InBus,
	                   const FOnBusNotification InNotificationDelegate)
		: Address(FSGMessageAddress::NewAddress())
		  , BusPtr(InBus)
		  , Enabled(true)
		  , NotificationDelegate(InNotificationDelegate)
		  , Id(FGuid::NewGuid())
		  , InboxEnabled(false)
		  , Name(InName)
	{
		SetRecipientThread(FTaskGraphInterface::Get().GetCurrentThreadIfKnown());
	}

	/** Destructor. */
	virtual ~FSGMessageEndpoint() override
	{
		if (const auto Bus = BusPtr.Pin())
		{
			Bus->Unregister(Address);
		}
	}

public:
	/**
	 * Disables this endpoint.
	 *
	 * A disabled endpoint will not receive any subscribed messages until it is enabled again.
	 * Endpoints should be created in an enabled state by default.
	 *
	 * @see Enable, IsEnabled
	 */
	void Disable()
	{
		Enabled = false;
	}

	/**
	 * Enables this endpoint.
	 *
	 * An activated endpoint will receive subscribed messages.
	 * Endpoints should be created in an enabled state by default.
	 *
	 * @see Disable, IsEnabled
	 */
	void Enable()
	{
		Enabled = true;
	}

	/**
	 * Gets the endpoint's message address.
	 *
	 * @return Message address.
	 */
	const FSGMessageAddress& GetAddress() const
	{
		return Address;
	}

	/**
	 * Checks whether this endpoint is connected to the bus.
	 *
	 * @return true if connected, false otherwise.
	 * @see IsEnabled
	 */
	bool IsConnected() const
	{
		return BusPtr.IsValid();
	}

	/**
	 * Checks whether this endpoint is enabled.
	 *
	 * @return true if the endpoint is enabled, false otherwise.
	 * @see Disable, Enable, IsConnected
	 */
	bool IsEnabled() const
	{
		return Enabled;
	}

	/**
	 * Sets the name of the thread to receive messages on.
	 *
	 * Use this method to receive messages on a particular thread, for example, if the
	 * consumer owning this endpoint is not thread-safe. The default value is ThreadAny.
	 *
	 * ThreadAny is the fastest way to receive messages. It should be used if the receiving
	 * code is completely thread-safe and if it is sufficiently fast. ThreadAny MUST NOT
	 * be used if the receiving code is not thread-safe. It also SHOULD NOT be used if the
	 * code includes time consuming operations, because it will block the message router,
	 * causing no other messages to be delivered in the meantime.
	 *
	 * @param NamedThread The name of the thread to receive messages on.
	 */
	void SetRecipientThread(const ENamedThreads::Type& NamedThread)
	{
		RecipientThread = ENamedThreads::GetThreadIndex(NamedThread);
	}

public:
	/**
	 * Defers processing of the given message by the specified time delay.
	 *
	 * The message is effectively delivered again to this endpoint after the
	 * original sent time plus the time delay have elapsed.
	 *
	 * @param Context The context of the message to defer.
	 * @param Delay The time delay.
	 */
	void Defer(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const FTimespan& Delay)
	{
		if (const TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = GetBusIfEnabled())
		{
			Bus->Forward(Context, TArrayBuilder<FSGMessageAddress>().Add(Address), Delay, AsShared());
		}
	}

	/**
	 * Forwards a previously received message.
	 *
	 * Messages can only be forwarded to endpoints within the same process.
	 *
	 * @param Context The context of the message to forward.
	 * @param Recipients The list of message recipients to forward the message to.
	 * @param Delay The time delay.
	 */
	void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	             const TArray<FSGMessageAddress>& Recipients, const FTimespan& Delay)
	{
		if (const TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = GetBusIfEnabled())
		{
			Bus->Forward(Context, Recipients, Delay, AsShared());
		}
	}

	/**
	 * Subscribes a message handler.
	 *
	 * @param MessageTag The type name of the messages to subscribe to.
	 * @param ScopeRange The range of message scopes to include in the subscription.
	 */
	void Subscribe(const FName& MessageTag, const FSGMessageScopeRange& ScopeRange)
	{
		if (HandlerMap.FindOrAdd(MessageTag).IsEmpty())
		{
			if (const TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = GetBusIfEnabled())
			{
				Bus->Subscribe(AsShared(), MessageTag, ScopeRange);
			}
		}
	}

	/**
	 * Unsubscribes this endpoint from the specified message type.
	 *
	 * @param MessageTag The type of message to unsubscribe (NAME_All = all types).
	 * @see Subscribe
	 */
	void Unsubscribe(const FName& MessageTag)
	{
		if (const TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = GetBusIfEnabled())
		{
			Bus->Unsubscribe(AsShared(), MessageTag);
		}
	}

	void Unsubscribe(MESSAGE_TAG_PARAM_SIGNATURE, const TSharedRef<ISGMessageHandler, ESPMode::ThreadSafe>& InHandler)
	{
		if (InHandler.IsUnique())
		{
			const auto MessageTag = FSGMessageTagBuilder::Builder(MESSAGE_TAG_PARAM_VALUE);

			if (auto Handlers = HandlerMap.Find(MessageTag))
			{
				for (const auto Handler : *Handlers)
				{
					if (Handler->GetHash() == InHandler->GetHash())
					{
						Handlers->Remove(Handler);

						if (Handlers->IsEmpty())
						{
							HandlerMap.Remove(MessageTag);

							Unsubscribe(MessageTag);
						}

						break;
					}
				}
			}
		}
	}

	template <typename HandlerType>
	void Unsubscribe(MESSAGE_TAG_PARAM_SIGNATURE, HandlerType* Handler,
	                 typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc)
	{
		Unsubscribe(
			MESSAGE_TAG_PARAM_VALUE,
			MakeShareable(new TSGRawMessageHandler<FSGMessage, HandlerType>(Handler, MoveTemp(HandlerFunc))));
	}

	template <typename MessageType, typename ContextType>
	void Unsubscribe(MESSAGE_TAG_PARAM_SIGNATURE, const UObject* Object, const FName& FunctionName)
	{
		Unsubscribe(
			MESSAGE_TAG_PARAM_VALUE,
			MakeShareable(new TSGDelegateMessageHandler<MessageType, ContextType>(Object, FunctionName)));
	}

public:
	/**
	 * Disables the inbox for unhandled messages.
	 *
	 * The inbox is disabled by default.
	 *
	 * @see EnableInbox, IsInboxEmpty, IsInboxEnabled, ProcessInbox, ReceiveFromInbox
	 */
	void DisableInbox()
	{
		InboxEnabled = false;
	}

	/**
	 * Enables the inbox for unhandled messages.
	 *
	 * If enabled, the inbox will queue up all received messages. Use ProcessInbox() to synchronously
	 * invoke the registered message handlers for all queued up messages, or ReceiveFromInbox() to
	 * manually receive one message from the inbox at a time. The inbox is disabled by default.
	 *
	 * @see DisableInbox, IsInboxEmpty, IsInboxEnabled, ProcessInbox, ReceiveFromInbox
	 */
	void EnableInbox()
	{
		InboxEnabled = true;
	}

	/**
	 * Checks whether the inbox is empty.
	 *
	 * @return true if the inbox is empty, false otherwise.
	 * @see DisableInbox, EnableInbox, IsInboxEnabled, ProcessInbox, ReceiveFromInbox
	 */
	bool IsInboxEmpty() const
	{
		return Inbox.IsEmpty();
	}

	/**
	 * Checks whether the inbox is enabled.
	 *
	 * @see DisableInbox, EnableInbox, IsInboxEmpty, ProcessInbox, ReceiveFromInbox
	 */
	bool IsInboxEnabled() const
	{
		return InboxEnabled;
	}

	/**
	 * Calls the matching message handlers for all messages queued up in the inbox.
	 *
	 * Note that an incoming message will only be queued up in the endpoint's inbox if the inbox has
	 * been enabled and no matching message handler handled it. The inbox is disabled by default and
	 * must be enabled using the EnableInbox() method.
	 *
	 * @see IsInboxEmpty, ReceiveFromInbox
	 */
	void ProcessInbox()
	{
		TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe> Context;

		while (Inbox.Dequeue(Context))
		{
			ProcessMessage(Context.ToSharedRef());
		}
	}

	/**
	 * Receives a single message from the endpoint's inbox.
	 *
	 * Note that an incoming message will only be queued up in the endpoint's inbox if the inbox has
	 * been enabled and no matching message handler handled it. The inbox is disabled by default and
	 * must be enabled using the EnableInbox() method.
	 *
	 * @param OutContext Will hold the context of the received message.
	 * @return true if a message was received, false if the inbox was empty.
	 * @see DisableInbox, EnableInbox, IsInboxEnabled, ProcessInbox
	 */
	bool ReceiveFromInbox(TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe>& OutContext)
	{
		return Inbox.Dequeue(OutContext);
	}

public:
	//~ ISGMessageReceiver interface

	virtual FName GetDebugName() const override
	{
		return Name;
	}

	virtual const FGuid& GetRecipientId() const override
	{
		return Id;
	}

	virtual ENamedThreads::Type GetRecipientThread() const override
	{
		return RecipientThread;
	}

	virtual bool IsLocal() const override
	{
		return true;
	}

	virtual void ReceiveMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		if (!Enabled)
		{
			return;
		}

		if (InboxEnabled)
		{
			Inbox.Enqueue(Context);
		}
		else
		{
			ProcessMessage(Context);
		}
	}

	//~ ISGBusListener interface

	virtual ENamedThreads::Type GetListenerThread() const override
	{
		return RecipientThread;
	}

	virtual void
	NotifyRegistration(const FSGMessageAddress& InAddress, ESGMessageBusNotification InNotification) override
	{
		if (!Enabled)
		{
			return;
		}

		if (NotificationDelegate.IsBound())
		{
			NotificationDelegate.Execute(FSGMessageBusNotification{InNotification, InAddress});
		}
	}

public:
	//~ ISGMessageSender interface

	virtual FSGMessageAddress GetSenderAddress() override
	{
		return Address;
	}

	virtual void NotifyMessageError(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	                                const FString& Error) override
	{
		if (ErrorDelegate.IsBound())
		{
			ErrorDelegate.Execute(Context.Get(), Error);
		}
	}

public:
	/**
	 * Immediately forwards a previously received message to the specified recipient.
	 *
	 * Messages can only be forwarded to endpoints within the same process.
	 *
	 * @param Context The context of the message to forward.
	 * @param Recipient The address of the recipient to forward the message to.
	 */
	void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const FSGMessageAddress& Recipient)
	{
		Forward(Context, TArrayBuilder<FSGMessageAddress>().Add(Recipient), FTimespan::Zero());
	}

	/**
	 * Forwards a previously received message to the specified recipient after a given delay.
	 *
	 * Messages can only be forwarded to endpoints within the same process.
	 *
	 * @param Context The context of the message to forward.
	 * @param Recipient The address of the recipient to forward the message to.
	 * @param Delay The delay after which to publish the message.
	 */
	void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context, const FSGMessageAddress& Recipient,
	             const FTimespan& Delay)
	{
		Forward(Context, TArrayBuilder<FSGMessageAddress>().Add(Recipient), Delay);
	}

	/**
	 * Immediately forwards a previously received message to the specified list of recipients.
	 *
	 * Messages can only be forwarded to endpoints within the same process.
	 *
	 * @param Context The context of the message to forward.
	 * @param Recipients The list of message recipients to forward the message to.
	 */
	void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
	             const TArray<FSGMessageAddress>& Recipients)
	{
		Forward(Context, Recipients, FTimespan::Zero());
	}

	template <typename MessageType>
	void Publish(const FName& MessageTag, MessageType* Message, CONST_PUBLISH_PARAMETER_SIGNATURE)
	{
		if (const auto Bus = GetBusIfEnabled())
		{
			Bus->Publish(MessageTag, Message, PUBLISH_PARAMETER_FORWARD, AsShared());
		}
	}

	template <typename MessageType>
	void PublishWithMessage(MESSAGE_TAG_PARAM_SIGNATURE, CONST_PUBLISH_PARAMETER_SIGNATURE, MessageType* Message)
	{
		Publish(FSGMessageTagBuilder::Builder(MESSAGE_TAG_PARAM_VALUE), Message, MESSAGE_PARAMETER);
	}

	template <typename ...Args>
	void Publish(MESSAGE_TAG_PARAM_SIGNATURE, CONST_PUBLISH_PARAMETER_SIGNATURE, Args&&... Params)
	{
		auto Message = FSGMessageBuilder::Builder<FSGMessage>(Params...);

		PublishWithMessage(MESSAGE_TAG_PARAM_VALUE, MESSAGE_PARAMETER, Message);
	}

	template <typename MessageType>
	void Send(const FName& MessageTag, MessageType* Message, const TArray<FSGMessageAddress>& Recipients,
	          CONST_SEND_PARAMETER_SIGNATURE)
	{
		const auto Bus = GetBusIfEnabled();

		if (Bus.IsValid())
		{
			Bus->Send(MessageTag, Message, Recipients, SEND_PARAMETER_FORWARD, AsShared());
		}
	}

	template <typename MessageType>
	void Send(const FName& MessageTag, MessageType* Message, const FSGMessageAddress& Recipient,
	          CONST_SEND_PARAMETER_SIGNATURE)
	{
		Send(MessageTag, Message, TArrayBuilder<FSGMessageAddress>().Add(Recipient), MESSAGE_PARAMETER);
	}

	template <typename MessageType>
	void SendWithMessage(MESSAGE_TAG_PARAM_SIGNATURE, const TArray<FSGMessageAddress>& Recipients,
	                     CONST_SEND_PARAMETER_SIGNATURE, MessageType* Message)
	{
		Send(FSGMessageTagBuilder::Builder(MESSAGE_TAG_PARAM_VALUE), Message, Recipients, MESSAGE_PARAMETER);
	}

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const TArray<FSGMessageAddress>& Recipients, CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params)
	{
		auto Message = FSGMessageBuilder::Builder<FSGMessage>(Params...);

		SendWithMessage(MESSAGE_TAG_PARAM_VALUE, Recipients, MESSAGE_PARAMETER, Message);
	}

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const FSGMessageAddress& Recipient, CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params)
	{
		Send(MESSAGE_TAG_PARAM_VALUE, TArrayBuilder<FSGMessageAddress>().Add(Recipient), MESSAGE_PARAMETER, Params...);
	}

	template <typename HandlerType>
	void Subscribe(MESSAGE_TAG_PARAM_SIGNATURE, HandlerType* Handler,
	               typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc)
	{
		const auto MessageTag = FSGMessageTagBuilder::Builder(MESSAGE_TAG_PARAM_VALUE);

		Subscribe(MessageTag, FSGMessageScopeRange::AtLeast(ESGMessageScope::Thread));

		WithRawMessageHandler(MessageTag, Handler, HandlerFunc);
	}

	template <typename MessageType, typename ContextType>
	void Subscribe(MESSAGE_TAG_PARAM_SIGNATURE, const UObject* Object, const FName& FunctionName)
	{
		const auto MessageTag = FSGMessageTagBuilder::Builder(MESSAGE_TAG_PARAM_VALUE);

		Subscribe(MessageTag, FSGMessageScopeRange::AtLeast(ESGMessageScope::Thread));

		WithDelegateMessageHandler<MessageType, ContextType>(MessageTag, Object, FunctionName);
	}

	/**
	 * Unsubscribes this endpoint from all message types.
	 *
	 * @see Subscribe
	 */
	void Unsubscribe()
	{
		Unsubscribe(NAME_All);
	}

public:
	/**
	 * Safely releases a message endpoint that is receiving messages on AnyThread.
	 *
	 * When an object that owns a message endpoint receiving on AnyThread is being destroyed,
	 * it is possible that the endpoint can outlive the object for a brief period of time if
	 * the Messaging system is dispatching messages to it. This helper function
	 * is to block the calling thread while any messages are being dispatched, so that the
	 * endpoint does not invoke any message handlers after the object has been destroyed.
	 *
	 * @param Endpoint The message endpoint to release.
	 */
	static void SafeRelease(TSharedPtr<FSGMessageEndpoint, ESPMode::ThreadSafe>& Endpoint)
	{
		if (Endpoint.IsValid())
		{
			Endpoint->ClearHandlers();
			Endpoint.Reset();
		}
	}

protected:
	/**
	 * Adds a message handler for the given type of messages (via raw function pointers).
	 *
	 * It is legal to configure multiple handlers for the same message type. Each
	 * handler will be executed when a message of the specified type is received.
	 *
	 * This overload is used to register raw class member functions.
	 *
	 * @param MessageTag The type of messages to handle.
	 * @param Handler The class handling the messages.
	 * @param HandlerFunc The class function handling the messages.
	 * @return This instance (for method chaining).
	 * @see WithHandler
	 */
	template <typename HandlerType>
	void WithRawMessageHandler(const FName& MessageTag, HandlerType* Handler,
	                           typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc)
	{
		WithHandler(
			MessageTag,
			MakeShareable(new TSGRawMessageHandler<FSGMessage, HandlerType>(Handler, MoveTemp(HandlerFunc))));
	}

	template <typename MessageType, typename ContextType>
	void WithDelegateMessageHandler(const FName& MessageTag, const UObject* Object, const FName& FunctionName)
	{
		WithHandler(
			MessageTag,
			MakeShareable(new TSGDelegateMessageHandler<MessageType, ContextType>(Object, FunctionName)));
	}

	/**
	 * Adds a message handler for the given type of messages (via TFunction object).
	 *
	 * It is legal to configure multiple handlers for the same message type. Each
	 * handler will be executed when a message of the specified type is received.
	 *
	 * This overload is used to register functions that are compatible with TFunction
	 * function objects, such as global and static functions, as well as lambdas.
	 *
	 * @param MessageTag The type of messages to handle.
	 * @param HandlerFunc The function object handling the messages.
	 * @return This instance (for method chaining).
	 * @see WithHandler
	 */
	void WithFunctionMessageHandler(const FName& MessageTag,
	                                TSGFunctionMessageHandler<FSGMessage>::FuncType HandlerFunc)
	{
		WithHandler(MessageTag, MakeShareable(new TSGFunctionMessageHandler<FSGMessage>(MoveTemp(HandlerFunc))));
	}

	/**
	 * Registers a message handler with the endpoint.
	 *
	 * It is legal to configure multiple handlers for the same message type. Each
	 * handler will be executed when a message of the specified type is received.
	 *
	 * @param InMessageTag
	 * @param InHandler The handler to add.
	 * @return This instance (for method chaining).
	 * @see Handling, WithCatchall
	 */
	void WithHandler(const FName& InMessageTag, const TSharedRef<ISGMessageHandler, ESPMode::ThreadSafe>& InHandler)
	{
		auto& Handlers = HandlerMap.FindOrAdd(InMessageTag);

		for (const auto& Handler : Handlers)
		{
			if (Handler->GetHash() == InHandler->GetHash())
			{
				return;
			}
		}

		Handlers.Add(InHandler);
	}

	/**
	 * Clears all handlers in a way that guarantees it won't overlap with message processing. This preserves internal integrity
	 * of the array and cases where our owner may be shutting down while receiving messages.
	*/
	void ClearHandlers()
	{
		FScopeLock Lock(&HandlersCS);
		HandlerMap.Empty();
	}

	/**
	 * Gets a shared pointer to the message bus if this endpoint is enabled.
	 *
	 * @return The message bus.
	 */
	FORCEINLINE TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> GetBusIfEnabled() const
	{
		if (Enabled)
		{
			return BusPtr.Pin();
		}

		return nullptr;
	}

	/**
	 * Forwards the given message context to matching message handlers.
	 *
	 * @param Context The context of the message to handle.
	 */
	void ProcessMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
	{
		if (!Context->IsValid())
		{
			return;
		}

		FScopeLock Lock(&HandlersCS);

		if (const auto Handlers = HandlerMap.Find(Context->GetMessageTag()))
		{
			for (int32 HandlerIndex = 0; HandlerIndex < Handlers->Num(); ++HandlerIndex)
			{
				(*Handlers)[HandlerIndex]->HandleMessage(Context);
			}
		}
	}

private:
	/** Holds the endpoint's identifier. */
	const FSGMessageAddress Address;

	/** Holds a weak pointer to the message bus. */
	TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> BusPtr;

	/** Hold a flag indicating whether this endpoint is active. */
	bool Enabled;

	/** Holds the registered message handlers. */
	TMap<FName, TArray<TSharedPtr<ISGMessageHandler, ESPMode::ThreadSafe>>> HandlerMap;

	/** Holds a delegate that is invoked on disconnection events. */
	FOnBusNotification NotificationDelegate;

	/** Holds the endpoint's unique identifier (for debugging purposes). */
	const FGuid Id;

	/** Holds the endpoint's message inbox. */
	TQueue<TSharedPtr<ISGMessageContext, ESPMode::ThreadSafe>, EQueueMode::Mpsc> Inbox;

	/** Holds a flag indicating whether the inbox is enabled. */
	bool InboxEnabled;

	/** Holds the endpoint's name (for debugging purposes). */
	const FName Name;

	/** Holds the name of the thread on which to receive messages. */
	ENamedThreads::Type RecipientThread;

private:
	/** Holds a delegate that is invoked in case of messaging errors. */
	FOnMessageEndpointError ErrorDelegate;

	/** Signifies that the handler array is being accessed and other threads should wait or skip */
	FCriticalSection HandlersCS;
};
