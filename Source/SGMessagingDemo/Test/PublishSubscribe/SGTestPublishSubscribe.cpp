// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestPublishSubscribe.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestPublishSubscribe::ASGTestPublishSubscribe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestPublishSubscribe::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestPublishSubscribeDelegate.AddDynamic(
			this, &ASGTestPublishSubscribe::OnDelegateBroadcast);
	}

	MessageBus = ISGMessagingModule::Get().GetDefaultBus(this);

	MessageEndpoint = FSGMessageEndpoint::Builder("Publish-Subscribe", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_PublishSubscribe, TopicPublishSubscribe_Publish, this,
	                           &ASGTestPublishSubscribe::OnPublish);
}

// Called every frame
void ASGTestPublishSubscribe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestPublishSubscribe::OnDelegateBroadcast()
{
	MessageEndpoint->Publish(Topic_PublishSubscribe, TopicPublishSubscribe_Publish, DEFAULT_PUBLISH_PARAMETER, "Val",
	                         FString("Publish-Subscribe Publish"));
}

void ASGTestPublishSubscribe::OnPublish(const FSGMessage& Message,
                                        const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestPublishSubscribe::OnPublish IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}

