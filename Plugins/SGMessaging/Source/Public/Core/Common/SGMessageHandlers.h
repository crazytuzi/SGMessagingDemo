// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageHandler.h"
#include "Misc/HashBuilder.h"


/**
 * Template for catch-all handlers (via raw function pointers).
 *
 * @param HandlerType The type of the handler class.
 */
template <typename HandlerType>
class TSGRawMessageCatchall
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
	TSGRawMessageCatchall(HandlerType* InHandler, FuncType InFunc)
		: Handler(InHandler)
		  , Func(InFunc)
		  , HashBuilder(FHashBuilder().Append(static_cast<void*>(InHandler)).Append(Pointer))
	{
		check(InHandler != nullptr);
	}

	/** Virtual destructor. */
	~TSGRawMessageCatchall()
	{
	}

public:
	//~ ISGMessageHandler interface

	virtual uint32 GetHash() const override
	{
		return HashBuilder.GetHash();
	}

	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		(Handler->*Func)(Context);
	}

private:
	/** Holds a pointer to the object handling the messages. */
	HandlerType* Handler;

	/** Holds a pointer to the actual handler function. */
	union
	{
		FuncType Func;

		void* Pointer;
	};

	FHashBuilder HashBuilder;
};


/**
 * Template for handlers of one specific message type (via raw function pointers).
 *
 * @param MessageType The type of message to handle.
 * @param HandlerType The type of the handler class.
 */
template <typename MessageType, typename HandlerType>
class TSGRawMessageHandler
	: public ISGMessageHandler
{
public:
	/** Type definition for function pointers that are compatible with this TRawSGMessageHandler. */
	typedef void (HandlerType::*FuncType)(const MessageType&,
	                                      const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&);

public:
	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InHandler The object that will handle the messages.
	 * @param InFunc The object's message handling function.
	 */
	TSGRawMessageHandler(HandlerType* InHandler, FuncType InFunc)
		: Handler(InHandler)
		  , Func(InFunc)
		  , HashBuilder(FHashBuilder().Append(static_cast<void*>(InHandler)).Append(Pointer))
	{
		check(InHandler != nullptr);
	}

	/** Virtual destructor. */
	~TSGRawMessageHandler()
	{
	}

public:
	//~ ISGMessageHandler interface

	virtual uint32 GetHash() const override
	{
		return HashBuilder.GetHash();
	}

	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		(Handler->*Func)(*static_cast<const MessageType*>(Context->GetMessage()), Context);
	}

private:
	/** Holds a pointer to the object handling the messages. */
	HandlerType* Handler;

	/** Holds a pointer to the actual handler function. */
	union
	{
		FuncType Func;

		void* Pointer;
	};

	FHashBuilder HashBuilder;
};


/**
 * Implements a catch-all handlers (via function objects).
 */
class FSGFunctionMessageCatchall
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
	FSGFunctionMessageCatchall(FuncType InFunc)
		: Func(MoveTemp(InFunc))
		  , HashBuilder(FHashBuilder().Append(Pointer))
	{
	}

	/** Virtual destructor. */
	~FSGFunctionMessageCatchall()
	{
	}

public:
	//~ ISGMessageHandler interface

	virtual uint32 GetHash() const override
	{
		return HashBuilder.GetHash();
	}

	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		Func(Context);
	}

private:
	/** Holds a pointer to the actual handler function. */
	union
	{
		FuncType Func;

		void* Pointer;
	};

	FHashBuilder HashBuilder;
};


/**
 * Template for handlers of one specific message type (via function objects).
 *
 * @param MessageType The type of message to handle.
 */
template <typename MessageType>
class TSGFunctionMessageHandler
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
	TSGFunctionMessageHandler(FuncType InFunc)
		: Func(MoveTemp(InFunc))
		  , HashBuilder(FHashBuilder().Append(Pointer))
	{
	}

	/** Virtual destructor. */
	~TSGFunctionMessageHandler()
	{
	}

public:
	//~ ISGMessageHandler interface

	virtual uint32 GetHash() const override
	{
		return HashBuilder.GetHash();
	}

	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		Func(*static_cast<const MessageType*>(Context->GetMessage()), Context);
	}

private:
	/** Holds a pointer to the actual handler function. */
	union
	{
		FuncType Func;

		void* Pointer;
	};

	FHashBuilder HashBuilder;
};


/**
 * Template for handlers of one specific message type (via delegate).
 */
template <typename MessageType, typename ContextType>
class TSGDelegateMessageHandler
	: public ISGMessageHandler
{
public:
	/**
	 * Creates and initializes a new message handler.
	 *
	 * @param InObject The delegate's Object.
	 * @param InFunctionName The delegate's function name.
	 */
	TSGDelegateMessageHandler(const UObject* InObject, const FName& InFunctionName)
		: Object(const_cast<UObject*>(InObject))
		  , FunctionName(InFunctionName)
		  , HashBuilder(FHashBuilder().Append(InObject).Append(InFunctionName))
	{
	}

	/** Virtual destructor. */
	~TSGDelegateMessageHandler()
	{
	}

private:
	struct FDelegateParams
	{
		MessageType Message;

		ContextType Context;
	};

public:
	//~ ISGMessageHandler interface

	virtual uint32 GetHash() const override
	{
		return HashBuilder.GetHash();
	}

	virtual void HandleMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context) override
	{
		if (Object.IsValid())
		{
			if (const auto Function = Object->FindFunctionChecked(FunctionName))
			{
				FDelegateParams DelegateParams{MessageType(Context->GetMessage()), ContextType(Context)};

				Object->ProcessEvent(Function, &DelegateParams);
			}
		}
	}

private:
	TWeakObjectPtr<UObject> Object;

	FName FunctionName;

	FHashBuilder HashBuilder;
};
