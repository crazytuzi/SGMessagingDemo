// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/Message/SGBlueprintMessageParameter.h"
#include "Core/Common/SGMessageEndpoint.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/Message/SGBlueprintMessage.h"
#include "Blueprint/Bus/SGBlueprintMessageContext.h"
#include "SGBlueprintMessageEndpoint.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSGBlueprintMessageDelegate, const FSGBlueprintMessage&, Message,
                                   const FSGBlueprintMessageContext&, Context);

/**
 * 
 */
UCLASS(BlueprintType)
class SGMESSAGING_API USGBlueprintMessageEndpoint : public UObject
{
	GENERATED_BODY()

public:
	typedef struct FSGBlueprintMessageEndpointBuilder Builder;

	USGBlueprintMessageEndpoint();

	virtual ~USGBlueprintMessageEndpoint() override;

public:
	UFUNCTION(BlueprintCallable)
	void Subscribe(const int32 InTopicID, const int32 InMessageID, const FSGBlueprintMessageDelegate& InDelegate);

	template <typename HandlerType>
	void Subscribe(MESSAGE_TAG_PARAM_SIGNATURE, HandlerType* Handler,
	               typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc)
	{
		if (MessageEndpoint.IsValid())
		{
			MessageEndpoint->Subscribe(MESSAGE_TAG_PARAM_VALUE, Handler, HandlerFunc);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Publish(const int32 InTopicID, const int32 InMessageID, const FSGBlueprintPublishParameter InParameter,
	             const FSGBlueprintMessage InMessage);

	template <typename ...Args>
	void Publish(MESSAGE_TAG_PARAM_SIGNATURE, CONST_PUBLISH_PARAMETER_SIGNATURE, Args&&... Params)
	{
		if (MessageEndpoint.IsValid())
		{
			MessageEndpoint->Publish(MESSAGE_TAG_PARAM_VALUE,MESSAGE_PARAMETER, Params...);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Send(const int32 InTopicID, const int32 InMessageID, const TArray<FSGBlueprintMessageAddress>& InRecipients,
	          const FSGBlueprintSendParameter InParameter, const FSGBlueprintMessage InMessage);

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const TArray<FSGMessageAddress>& InRecipients,
	          CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params)
	{
		if (MessageEndpoint.IsValid())
		{
			MessageEndpoint->Send(MESSAGE_TAG_PARAM_VALUE, InRecipients, MESSAGE_PARAMETER, Params...);
		}
	}

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const FSGMessageAddress& InRecipient, CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params)
	{
		if (MessageEndpoint.IsValid())
		{
			MessageEndpoint->Send(MESSAGE_TAG_PARAM_VALUE, InRecipient, MESSAGE_PARAMETER, Params...);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Forward(const FSGBlueprintMessageContext& InContext, const TArray<FSGBlueprintMessageAddress>& InRecipients,
	             const FTimespan InDelay);

	void Forward(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& InContext,
	             const TArray<FSGMessageAddress>& InRecipients, const FTimespan InDelay = FTimespan()) const
	{
		if (MessageEndpoint.IsValid())
		{
			MessageEndpoint->Forward(InContext, InRecipients, InDelay);
		}
	}

public:
	UFUNCTION(BlueprintCallable)
	FSGBlueprintMessageAddress GetAddress() const;

private:
	TSharedPtr<FSGMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;

	friend struct FSGBlueprintMessageEndpointBuilder;
};
