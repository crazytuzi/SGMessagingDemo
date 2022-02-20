// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusSubscribe.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMessageBusSubscribe::AMessageBusSubscribe()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMessageBusSubscribe::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Publish-Subscribe", MessageBus.ToSharedRef()).Handling<
		FTestPublishMessage>(this, &AMessageBusSubscribe::OnReceive);

	MessageEndpoint->Subscribe<FTestPublishMessage>();
}

// Called every frame
void AMessageBusSubscribe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMessageBusSubscribe::OnReceive(const FTestPublishMessage& Message,
                                     const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("AMessageBusSubscribe::OnReceive IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Val);
}
