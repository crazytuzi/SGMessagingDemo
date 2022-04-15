// Fill out your copyright notice in the Description page of Project Settings.


#include "MessagingFramework/Subsystems/SGMessageWorldSubsystem.h"
#include "Blueprint/Common/SGBlueprintMessageEndpointBuilder.h"

void USGMessageWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DefaultBus = NewObject<USGBlueprintMessageBus>(this);

	DefaultMessageEndpoint = USGBlueprintMessageEndpoint::Builder(this, *GetWorld()->GetName(),
	                                                              DefaultBus->GetMessageBus().ToSharedRef());
}

void USGMessageWorldSubsystem::Deinitialize()
{
	DefaultBus->MarkAsGarbage();

	DefaultMessageEndpoint->MarkAsGarbage();

	Super::Deinitialize();
}

USGBlueprintMessageBus* USGMessageWorldSubsystem::GetDefaultBus() const
{
	return DefaultBus;
}

USGBlueprintMessageEndpoint* USGMessageWorldSubsystem::GetDefaultMessageEndpoint() const
{
	return DefaultMessageEndpoint;
}
