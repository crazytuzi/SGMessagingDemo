// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"

class FName;
class ISGMessageInterceptor;
class ISGMessageReceiver;

struct FSGMessageAddress;


/**
 * Interface for classes that authorize message subscriptions.
 */
class ISGAuthorizeMessageRecipients
{
public:
	/**
	 * Authorizes a request to intercept messages of the specified type.
	 *
	 * @param Interceptor The message interceptor to authorize.
	 * @param MessageTag The type of messages to intercept.
	 * @return true if the request was authorized, false otherwise.
	 */
	virtual bool AuthorizeInterceptor(const TSharedRef<ISGMessageInterceptor, ESPMode::ThreadSafe>& Interceptor,
	                                  const FName& MessageTag) = 0;

	/**
	 * Authorizes a request to register the specified recipient.
	 *
	 * @param Recipient The recipient to register.
	 * @param Address The recipient's address.
	 * @return true if the request was authorized, false otherwise.
	 */
	virtual bool AuthorizeRegistration(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Recipient,
	                                   const FSGMessageAddress& Address) = 0;

	/**
	 * Authorizes a request to add a subscription for the specified topic pattern.
	 *
	 * @param Subscriber The subscriber.
	 * @param MessageTag The message topic pattern to subscribe to.
	 * @return true if the request is authorized, false otherwise.
	 */
	virtual bool AuthorizeSubscription(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber,
	                                   const FName& MessageTag) = 0;

	/**
	 * Authorizes a request to unregister the specified recipient.
	 *
	 * @param Address The address of the recipient to unregister.
	 * @return true if the request was authorized, false otherwise.
	 */
	virtual bool AuthorizeUnregistration(const FSGMessageAddress& Address) = 0;

	/**
	 * Authorizes a request to remove a subscription for the specified topic pattern.
	 *
	 * @param Subscriber The subscriber.
	 * @param TopicPattern The message topic pattern to unsubscribe from.
	 * @return true if the request is authorized, false otherwise.
	 */
	virtual bool AuthorizeUnsubscription(const TSharedRef<ISGMessageReceiver, ESPMode::ThreadSafe>& Subscriber,
	                                     const FName& TopicPattern) = 0;

public:
	/** Virtual destructor. */
	virtual ~ISGAuthorizeMessageRecipients()
	{
	}
};
