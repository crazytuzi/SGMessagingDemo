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

void USGMessageEndpointComponent::Subscribe(const int32 InTopicID, const int32 InMessageID,
                                            const FSGBlueprintMessageDelegate& InDelegate)
{
	if (MessageEndpoint != nullptr)
	{
		MessageEndpoint->Subscribe(InTopicID, InMessageID, InDelegate);
	}
}

void USGMessageEndpointComponent::Publish(const int32 InTopicID, const int32 InMessageID,
                                          const FSGBlueprintPublishParameter InParameter,
                                          const FSGBlueprintMessage InMessage)
{
	if (MessageEndpoint != nullptr)
	{
		MessageEndpoint->Publish(InTopicID, InMessageID, InParameter, InMessage);
	}
}

void USGMessageEndpointComponent::Send(const int32 InTopicID, const int32 InMessageID,
                                       const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                       const FSGBlueprintSendParameter InParameter,
                                       const FSGBlueprintMessage InMessage)
{
	if (MessageEndpoint != nullptr)
	{
		MessageEndpoint->Send(InTopicID, InMessageID, InRecipients, InParameter, InMessage);
	}
}

void USGMessageEndpointComponent::Forward(const FSGBlueprintMessageContext& InContext,
                                          const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                          const FTimespan InDelay)
{
	if (MessageEndpoint != nullptr)
	{
		MessageEndpoint->Forward(InContext, InRecipients, InDelay);
	}
}

FSGBlueprintMessageAddress USGMessageEndpointComponent::GetAddress() const
{
	if (MessageEndpoint != nullptr)
	{
		return MessageEndpoint->GetAddress();
	}

	return FSGBlueprintMessageAddress();
}
