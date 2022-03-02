// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusRequest.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "EngineUtils.h"
#include "MessageBusReply.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MessageBusType.h"

// Sets default values
AMessageBusRequest::AMessageBusRequest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMessageBusRequest::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Request-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(TopicB, TopicB_MessageID2, this, &AMessageBusRequest::OnReply);
}

// Called every frame
void AMessageBusRequest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<AMessageBusReply> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		Recipients.Add(Iterator->MessageEndpoint->GetAddress());
	}

	MessageEndpoint->Send(TopicB, TopicB_MessageID1, Recipients, DEFAULT_SEND_PARAMETER, "Val",
	                      FString("Request-Reply"));
}

void AMessageBusRequest::OnReply(const FSGMessage& Message,
                                 const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("AMessageBusRequest::OnReply IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}

