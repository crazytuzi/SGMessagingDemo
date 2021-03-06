// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestDelayRequest.h"
#include "EngineUtils.h"
#include "SGTestDelayReply.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestDelayRequest::ASGTestDelayRequest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MessageEndpointComponent = CreateDefaultSubobject<USGMessageEndpointComponent>(TEXT("MessageEndpointComponent"));
}

// Called when the game starts or when spawned
void ASGTestDelayRequest::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestDelayForwardReplyDelegate.AddDynamic(
			this, &ASGTestDelayRequest::OnDelegateBroadcast);
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Subscribe(Topic_DelayForwardReply, TopicDelayForwardReply_Reply, this,
		                                    &ASGTestDelayRequest::OnReply);
	}
}

// Called every frame
void ASGTestDelayRequest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestDelayRequest::OnDelegateBroadcast()
{
	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestDelayReply> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		if (const auto Component = Cast<USGMessageEndpointComponent>(
			Iterator->GetComponentByClass(USGMessageEndpointComponent::StaticClass())))
		{
			Recipients.Add(Component->GetAddress());
		}
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Send(Topic_DelayForwardReply, TopicDelayForwardReply_Request, Recipients,
		                               DELAY_SEND_PARAMETER(FTimespan(0, 0, 5)), "Val",
		                               FString("DelayForward-Reply Request"));
	}
}

void ASGTestDelayRequest::OnReply(const FSGMessage& Message,
                                  const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestDelayRequest::OnReply IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}
