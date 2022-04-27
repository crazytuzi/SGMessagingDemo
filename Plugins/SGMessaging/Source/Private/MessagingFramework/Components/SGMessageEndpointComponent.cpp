// Fill out your copyright notice in the Description page of Project Settings.


#include "MessagingFramework/Components/SGMessageEndpointComponent.h"
#include "Blueprint/Common/SGBlueprintMessageEndpointBuilder.h"
#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"

// Sets default values for this component's properties
USGMessageEndpointComponent::USGMessageEndpointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USGMessageEndpointComponent::BeginPlay()
{
	Super::BeginPlay();

	MessageEndpoint = USGBlueprintMessageEndpoint::Builder(this, GetFName(),
	                                                       *USGMessageFunctionLibrary::GetDefaultBus(this));
}


// Called every frame
void USGMessageEndpointComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USGMessageEndpointComponent::Subscribe(MESSAGE_TAG_PARAM_SIGNATURE,
                                            const FSGBlueprintMessageDelegate& InDelegate)
{
	if (IsValid(MessageEndpoint))
	{
		MessageEndpoint->Subscribe(MESSAGE_TAG_PARAM_VALUE, InDelegate);
	}
}

void USGMessageEndpointComponent::Publish(MESSAGE_TAG_PARAM_SIGNATURE,
                                          const FSGBlueprintPublishParameter MESSAGE_PARAMETER,
                                          const FSGBlueprintMessage InMessage)
{
	if (IsValid(MessageEndpoint))
	{
		MessageEndpoint->Publish(MESSAGE_TAG_PARAM_VALUE, MESSAGE_PARAMETER, InMessage);
	}
}

void USGMessageEndpointComponent::Send(MESSAGE_TAG_PARAM_SIGNATURE,
                                       const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                       const FSGBlueprintSendParameter MESSAGE_PARAMETER,
                                       const FSGBlueprintMessage InMessage)
{
	if (IsValid(MessageEndpoint))
	{
		MessageEndpoint->Send(MESSAGE_TAG_PARAM_VALUE, InRecipients, MESSAGE_PARAMETER, InMessage);
	}
}

void USGMessageEndpointComponent::Forward(const FSGBlueprintMessageContext& InContext,
                                          const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                          const FTimespan InDelay)
{
	if (IsValid(MessageEndpoint))
	{
		MessageEndpoint->Forward(InContext, InRecipients, InDelay);
	}
}

void USGMessageEndpointComponent::Unsubscribe(
	MESSAGE_TAG_PARAM_SIGNATURE, const FSGBlueprintMessageDelegate& InDelegate) const
{
	if (IsValid(MessageEndpoint))
	{
		return MessageEndpoint->Unsubscribe(MESSAGE_TAG_PARAM_VALUE, InDelegate);
	}
}

FSGBlueprintMessageAddress USGMessageEndpointComponent::GetAddress() const
{
	if (IsValid(MessageEndpoint))
	{
		return MessageEndpoint->GetAddress();
	}

	return FSGBlueprintMessageAddress();
}
