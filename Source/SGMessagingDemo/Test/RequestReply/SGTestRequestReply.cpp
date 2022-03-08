// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestRequestReply.h"
#include "EngineUtils.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestRequestReply::ASGTestRequestReply()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestRequestReply::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestRequestReplyDelegate.AddDynamic(this, &ASGTestRequestReply::OnDelegateBroadcast);
	}

	MessageBus = ISGMessagingModule::Get().GetDefaultBus(this);

	MessageEndpoint = FSGMessageEndpoint::Builder("Request-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_RequestReply, TopicRequestReply_Request, this, &ASGTestRequestReply::OnRequest);

	MessageEndpoint->Subscribe(Topic_RequestReply, TopicRequestReply_Reply, this, &ASGTestRequestReply::OnReply);
}

// Called every frame
void ASGTestRequestReply::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestRequestReply::OnDelegateBroadcast()
{
	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestRequestReply> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		Recipients.Add(Iterator->MessageEndpoint->GetAddress());
	}

	MessageEndpoint->Send(Topic_RequestReply, TopicRequestReply_Request, Recipients, DEFAULT_SEND_PARAMETER, "Val",
	                      FString("Request-Reply Request"));
}

void ASGTestRequestReply::OnRequest(const FSGMessage& Message,
                                    const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestRequestReply::OnRequest IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	MessageEndpoint->Send(Topic_RequestReply, TopicRequestReply_Reply, Context->GetSender(), DEFAULT_SEND_PARAMETER,
	                      "Val", FString("Request-Reply Reply"));
}

void ASGTestRequestReply::OnReply(const FSGMessage& Message,
                                  const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestRequestReply::OnReply IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}

