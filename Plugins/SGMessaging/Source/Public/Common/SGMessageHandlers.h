// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ISGMessageContext.h"
#include "Interface/ISGMessageHandler.h"


/**
 * Template for catch-all handlers (via raw function pointers).
 *
 * @param HandlerType The type of the handler class.
 */
template<typename HandlerType>
class TRawSGMessageCatchall
	: public ISGMessageHandler
{
public:

	/** Type definition for function pointers that are compatible with this TRawSGMessageCatchall. */
	typedef void (HandlerType::*FuncType)(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&);

public:

	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InHandler The object that will handle the messages.
	 * @param InFunc The object's message handling function.
	 */
	TRawSGMessageCatchall(HandlerType* InHandler, FuncType InFunc)
		: Handler(InHandler)
		, Func(InFunc)
	{
		check(InHandler != nullptr);
	}

	/** Virtual destructor. */
	~TRawSGMessageCatchall() { }

public:

	//~ ISGMessageHandler interface
	
	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		(Handler->*Func)(Context);
	}
	
private:

	/** Holds a pointer to the object handling the messages. */
	HandlerType* Handler;

	/** Holds a pointer to the actual handler function. */
	FuncType Func;
};


/**
 * Template for handlers of one specific message type (via raw function pointers).
 *
 * @param MessageType The type of message to handle.
 * @param HandlerType The type of the handler class.
 */
template<typename MessageType, typename HandlerType>
class TRawSGMessageHandler
	: public ISGMessageHandler
{
public:

	/** Type definition for function pointers that are compatible with this TRawSGMessageHandler. */
	typedef void (HandlerType::*FuncType)(const MessageType&, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&);

public:

	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InHandler The object that will handle the messages.
	 * @param InFunc The object's message handling function.
	 */
	TRawSGMessageHandler(HandlerType* InHandler, FuncType InFunc)
		: Handler(InHandler)
		, Func(InFunc)
	{
		check(InHandler != nullptr);
	}

	/** Virtual destructor. */
	~TRawSGMessageHandler() { }

public:

	//~ ISGMessageHandler interface
	
	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		(Handler->*Func)(*static_cast<const MessageType*>(Context->GetMessage()), Context);
	}
	
private:

	/** Holds a pointer to the object handling the messages. */
	HandlerType* Handler;

	/** Holds a pointer to the actual handler function. */
	FuncType Func;
};


/**
 * Implements a catch-all handlers (via function objects).
 */
class FFunctionSGMessageCatchall
	: public ISGMessageHandler
{
public:

	/** Type definition for function objects that are compatible with this TFunctionSGMessageHandler. */
	typedef TFunction<void(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&)> FuncType;

public:

	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InFunc The object's message handling function.
	 */
	FFunctionSGMessageCatchall(FuncType InFunc)
		: Func(MoveTemp(InFunc))
	{ }

	/** Virtual destructor. */
	~FFunctionSGMessageCatchall() { }

public:

	//~ ISGMessageHandler interface
	
	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		Func(Context);
	}
	
private:

	/** Holds a pointer to the actual handler function. */
	FuncType Func;
};


/**
 * Template for handlers of one specific message type (via function objects).
 *
 * @param MessageType The type of message to handle.
 */
template<typename MessageType>
class TFunctionSGMessageHandler
	: public ISGMessageHandler
{
public:

	/** Type definition for function objects that are compatible with this TFunctionSGMessageHandler. */
	typedef TFunction<void(const MessageType&, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&)> FuncType;

public:

	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InHandlerFunc The object's message handling function.
	 */
	TFunctionSGMessageHandler(FuncType InFunc)
		: Func(MoveTemp(InFunc))
	{ }

	/** Virtual destructor. */
	~TFunctionSGMessageHandler() { }

public:

	//~ ISGMessageHandler interface
	
	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		Func(*static_cast<const MessageType*>(Context->GetMessage()), Context);
	}
	
private:

	/** Holds a pointer to the actual handler function. */
	FuncType Func;
};
