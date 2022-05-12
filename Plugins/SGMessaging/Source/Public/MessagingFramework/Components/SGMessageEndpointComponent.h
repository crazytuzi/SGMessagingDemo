// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/Common/SGBlueprintMessageEndpoint.h"
#include "Components/ActorComponent.h"
#include "Core/Common/SGMessageEndpoint.h"
#include "SGMessageEndpointComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SGMESSAGING_API USGMessageEndpointComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USGMessageEndpointComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void Subscribe(const int32 InTopicID, const int32 InMessageID, const FSGBlueprintMessageDelegate& InDelegate,
	               const ESGBlueprintMessageScope InScope = ESGBlueprintMessageScope::Thread);

	template <typename HandlerType>
	void Subscribe(MESSAGE_TAG_PARAM_SIGNATURE, HandlerType* Handler,
	               typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc,
	               const ESGBlueprintMessageScope InScope = ESGBlueprintMessageScope::Thread) const
	{
		if (IsValid(MessageEndpoint))
		{
			MessageEndpoint->Subscribe(MESSAGE_TAG_PARAM_VALUE, Handler, HandlerFunc, InScope);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Publish(const int32 InTopicID, const int32 InMessageID, const FSGBlueprintPublishParameter InParameter,
	             const FSGBlueprintMessage InMessage);

	template <typename ...Args>
	void Publish(MESSAGE_TAG_PARAM_SIGNATURE, CONST_PUBLISH_PARAMETER_SIGNATURE, Args&&... Params) const
	{
		if (IsValid(MessageEndpoint))
		{
			MessageEndpoint->Publish(MESSAGE_TAG_PARAM_VALUE,MESSAGE_PARAMETER, Params...);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Send(const int32 InTopicID, const int32 InMessageID, const TArray<FSGBlueprintMessageAddress>& InRecipients,
	          const FSGBlueprintSendParameter InParameter, const FSGBlueprintMessage InMessage);

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const TArray<FSGMessageAddress>& InRecipients,CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params) const
	{
		if (IsValid(MessageEndpoint))
		{
			MessageEndpoint->Send(MESSAGE_TAG_PARAM_VALUE, InRecipients, MESSAGE_PARAMETER, Params...);
		}
	}

	template <typename ...Args>
	void Send(MESSAGE_TAG_PARAM_SIGNATURE, const FSGMessageAddress& InRecipient, CONST_SEND_PARAMETER_SIGNATURE,
	          Args&&... Params) const
	{
		if (IsValid(MessageEndpoint))
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
		if (IsValid(MessageEndpoint))
		{
			MessageEndpoint->Forward(InContext, InRecipients, InDelay);
		}
	}

	UFUNCTION(BlueprintCallable)
	void Unsubscribe(const int32 InTopicID, const int32 InMessageID, const FSGBlueprintMessageDelegate& InDelegate);

	template <typename HandlerType>
	void Unsubscribe(MESSAGE_TAG_PARAM_SIGNATURE, HandlerType* Handler,
	                 typename TSGRawMessageHandler<FSGMessage, HandlerType>::FuncType HandlerFunc) const
	{
		if (IsValid(MessageEndpoint))
		{
			MessageEndpoint->Unsubscribe(MESSAGE_TAG_PARAM_VALUE, Handler, HandlerFunc);
		}
	}

public:
	UFUNCTION(BlueprintCallable)
	FSGBlueprintMessageAddress GetAddress() const;

private:
	UPROPERTY()
	USGBlueprintMessageEndpoint* MessageEndpoint;
};
