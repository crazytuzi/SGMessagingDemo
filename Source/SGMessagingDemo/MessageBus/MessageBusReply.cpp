// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusReply.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "EngineUtils.h"
#include "MessageBusForward.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MessageBusType.h"

// Sets default values
AMessageBusReply::AMessageBusReply()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMessageBusReply::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Request-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(TopicB, TopicB_MessageID1, this, &AMessageBusReply::OnRequest);
}

// Called every frame
void AMessageBusReply::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMessageBusReply::OnRequest(const FSGMessage& Message,
                                 const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("AMessageBusReply::OnRequest IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	MessageEndpoint->Send(TopicB, TopicB_MessageID2, Context->GetSender(), DEFAULT_SEND_PARAMETER, "Val",
	                      FString("Reply-Request"));

	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<AMessageBusForward> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		Recipients.Add(Iterator->MessageEndpoint->GetAddress());
	}

	MessageEndpoint->Forward(Context, Recipients);
}

