// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusPublish.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "MessageBusType.h"

// Sets default values
AMessageBusPublish::AMessageBusPublish()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMessageBusPublish::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Publish-Subscribe", MessageBus.ToSharedRef());
}

// Called every frame
void AMessageBusPublish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MessageEndpoint->Publish(TopicA, TopicA_MessageID1, "Val", FString("Publish-Subscribe"));
}

