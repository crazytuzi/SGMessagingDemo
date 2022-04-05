// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestDelayRequestReply.h"
#include "EngineUtils.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestDelayRequestReply::ASGTestDelayRequestReply()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MessageEndpointComponent = CreateDefaultSubobject<USGMessageEndpointComponent>(TEXT("MessageEndpointComponent"));
}

// Called when the game starts or when spawned
void ASGTestDelayRequestReply::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestDelayRequestReplyDelegate.AddDynamic(
			this, &ASGTestDelayRequestReply::OnDelegateBroadcast);
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Subscribe(Topic_DelayRequestReply, TopicDelayRequestReply_Request, this,
		                                    &ASGTestDelayRequestReply::OnRequest);

		MessageEndpointComponent->Subscribe(Topic_DelayRequestReply, TopicDelayRequestReply_Reply, this,
		                                    &ASGTestDelayRequestReply::OnReply);
	}
}

// Called every frame
void ASGTestDelayRequestReply::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestDelayRequestReply::OnDelegateBroadcast()
{
	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestDelayRequestReply> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		if (const auto Component = Cast<USGMessageEndpointComponent>(
			Iterator->GetComponentByClass(USGMessageEndpointComponent::StaticClass())))
		{
			Recipients.Add(Component->GetAddress());
		}
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Send(Topic_DelayRequestReply, TopicDelayRequestReply_Request, Recipients,
		                               DELAY_SEND_PARAMETER(FTimespan(0, 0, 5)), "Val",
		                               FString("DelayRequest-Reply Request"));
	}
}

void ASGTestDelayRequestReply::OnRequest(const FSGMessage& Message,
                                         const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestDelayRequestReply::OnRequest IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Send(Topic_DelayRequestReply, TopicDelayRequestReply_Reply, Context->GetSender(),
		                               DELAY_SEND_PARAMETER(FTimespan(0, 0, 5)), "Val",
		                               FString("DelayRequest-Reply Reply"));
	}
}

void ASGTestDelayRequestReply::OnReply(const FSGMessage& Message,
                                       const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestDelayRequestReply::OnReply IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}
