// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestDelayReply.h"
#include "EngineUtils.h"
#include "SGTestDelayForward.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"

// Sets default values
ASGTestDelayReply::ASGTestDelayReply()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MessageEndpointComponent = CreateDefaultSubobject<USGMessageEndpointComponent>(TEXT("MessageEndpointComponent"));
}

// Called when the game starts or when spawned
void ASGTestDelayReply::BeginPlay()
{
	Super::BeginPlay();

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Subscribe(Topic_DelayForwardReply, TopicDelayForwardReply_Request, this,
		                                    &ASGTestDelayReply::OnRequest);
	}
}

// Called every frame
void ASGTestDelayReply::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestDelayReply::OnRequest(const FSGMessage& Message,
                                  const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestReply::OnRequest IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestDelayForward> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		if (const auto Component = Cast<USGMessageEndpointComponent>(
			Iterator->GetComponentByClass(USGMessageEndpointComponent::StaticClass())))
		{
			Recipients.Add(Component->GetAddress());
		}
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Forward(Context, Recipients, FTimespan(0, 0, 5));
	}
}
