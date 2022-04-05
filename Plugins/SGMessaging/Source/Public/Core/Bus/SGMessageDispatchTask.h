// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Interface/ISGMessageBusListener.h"
#include "Core/Bus/SGMessageTracer.h"

class ISGMessageReceiver;

/**
 * Implements an asynchronous task for dispatching a message to a recipient.
 */
class FSGMessageDispatchTask
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InThread The name of the thread to dispatch the message on.
	 * @param InContext The context of the message to dispatch.
	 * @param InRecipient The message recipient.
	 * @param InTracer The message tracer to notify.
	 */
	FSGMessageDispatchTask(
		ENamedThreads::Type InThread,
		TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> InContext,
		TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> InRecipient,
		TSharedPtr<FSGMessageTracer, ESPMode::ThreadSafe> InTracer);

public:

	/**
	 * Performs the actual task.
	 *
	 * @param CurrentThread The thread that this task is executing on.
	 * @param MyCompletionGraphEvent The completion event.
	 */
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);
	
	/**
	 * Returns the name of the thread that this task should run on.
	 *
	 * @return Thread name.
	 */
	ENamedThreads::Type GetDesiredThread()
	{
		return Thread;
	}

	/**
	 * Gets the task's stats tracking identifier.
	 *
	 * @return Stats identifier.
	 */
	TStatId GetStatId() const;

	/**
	 * Gets the mode for tracking subsequent tasks.
	 *
	 * @return Always track subsequent tasks.
	 */
	static ESubsequentsMode::Type GetSubsequentsMode() 
	{ 
		return ESubsequentsMode::TrackSubsequents; 
	}

private:

	/** Holds the message context. */
	TSharedRef<ISGMessageContext, ESPMode::ThreadSafe> Context;

	/** Holds a reference to the recipient. */
	TWeakPtr<ISGMessageReceiver, ESPMode::ThreadSafe> RecipientPtr;

	/** Holds the name of the thread that the router is running on. */
	ENamedThreads::Type Thread;

	/** Holds a pointer to the message tracer. */
	TWeakPtr<FSGMessageTracer, ESPMode::ThreadSafe> TracerPtr;
};


/**
 * Implements an asynchronous task for dispatching a registration notification to a listener.
 */
class FSGBusNotificationDispatchTask
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InThread The name of the thread to dispatch the message on.
	 * @param InListener The listener to notify.
	 * @param InAddress The address we are notifying about.
	 * @param InNotification The notification type being notified.
	 */
	FSGBusNotificationDispatchTask(
		ENamedThreads::Type InThread,
		TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> InListener,
		FSGMessageAddress InAddress,
		ESGMessageBusNotification InNotification)
		: Thread(InThread)
		, ListenerPtr(InListener)
		, Address(InAddress)
		, Notification(InNotification)
	{}

public:

	/**
	 * Performs the actual task.
	 *
	 * @param CurrentThread The thread that this task is executing on.
	 * @param MyCompletionGraphEvent The completion event.
	 */
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);

	/**
	 * Returns the name of the thread that this task should run on.
	 *
	 * @return Thread name.
	 */
	ENamedThreads::Type GetDesiredThread()
	{
		return Thread;
	}

	TStatId GetStatId() const;

	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return ESubsequentsMode::TrackSubsequents;
	}

private:
	/** Holds the name of the thread that the router is running on. */
	ENamedThreads::Type Thread;

	/** Holds a reference to the recipient. */
	TWeakPtr<ISGBusListener, ESPMode::ThreadSafe> ListenerPtr;

	/** Holds the address we are notifying about. */
	FSGMessageAddress Address;

	/** Holds the notification type we are notifying about. */
	ESGMessageBusNotification Notification;
};
