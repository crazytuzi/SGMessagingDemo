// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestReply.h"
#include "EngineUtils.h"
#include "SGTestForward.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"

// Sets default values
ASGTestReply::ASGTestReply()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MessageEndpointComponent = CreateDefaultSubobject<USGMessageEndpointComponent>(TEXT("MessageEndpointComponent"));
}

// Called when the game starts or when spawned
void ASGTestReply::BeginPlay()
{
	Super::BeginPlay();

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Subscribe(Topic_ForwardReply, TopicForwardReply_Request, this,
		                                    &ASGTestReply::OnRequest);
	}
}

// Called every frame
void ASGTestReply::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestReply::OnRequest(const FSGMessage& Message,
                             const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestReply::OnRequest IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestForward> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		if (const auto Component = Cast<USGMessageEndpointComponent>(
			Iterator->GetComponentByClass(USGMessageEndpointComponent::StaticClass())))
		{
			Recipients.Add(Component->GetAddress());
		}
	}

	if (MessageEndpointComponent != nullptr)
	{
		MessageEndpointComponent->Forward(Context, Recipients);
	}
}
