// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestDelayPublishSubscribe.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestDelayPublishSubscribe::ASGTestDelayPublishSubscribe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestDelayPublishSubscribe::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestDelayPublishSubscribeDelegate.AddDynamic(
			this, &ASGTestDelayPublishSubscribe::OnDelegateBroadcast);
	}

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("DelayPublish-Subscribe", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_DelayPublishSubscribe, TopicDelayPublishSubscribe_Publish, this,
	                           &ASGTestDelayPublishSubscribe::OnPublish);
}

// Called every frame
void ASGTestDelayPublishSubscribe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestDelayPublishSubscribe::OnDelegateBroadcast()
{
	MessageEndpoint->Publish(Topic_DelayPublishSubscribe, TopicDelayPublishSubscribe_Publish,
	                         DELAY_PUBLISH_PARAMETER(FTimespan(0, 0, 5)), "Val",
	                         FString("DelayPublish-Subscribe Publish"));
}

void ASGTestDelayPublishSubscribe::OnPublish(const FSGMessage& Message,
                                        const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestDelayPublishSubscribe::OnPublish IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}

