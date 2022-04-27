// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestCppUnsubscribe.h"
#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestCppUnsubscribe::ASGTestCppUnsubscribe()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASGTestCppUnsubscribe::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestCppSubscribeDelegate.AddDynamic(
			this, &ASGTestCppUnsubscribe::OnCppSubscribeDelegateBroadcast);

		MessagingTestSubsystem->TestCppUnsubscribeDelegate.AddDynamic(
			this, &ASGTestCppUnsubscribe::OnCppUnsubscribeDelegateBroadcast);
	}
}

// Called every frame
void ASGTestCppUnsubscribe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestCppUnsubscribe::OnCppSubscribeDelegateBroadcast()
{
	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		MessageEndpoint->Subscribe(Topic_Unsubscribe, TopicUnsubscribe_Cpp, this,
		                           &ASGTestCppUnsubscribe::OnPublish);

		MessageEndpoint->Publish(Topic_Unsubscribe, TopicUnsubscribe_Cpp, DEFAULT_PUBLISH_PARAMETER,
		                         "Val",
		                         FString("Publish-Subscribe Publish"));
	}
}

void ASGTestCppUnsubscribe::OnCppUnsubscribeDelegateBroadcast()
{
	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		MessageEndpoint->Unsubscribe(Topic_Unsubscribe, TopicUnsubscribe_Cpp, this, &ASGTestCppUnsubscribe::OnPublish);

		MessageEndpoint->Publish(Topic_Unsubscribe, TopicUnsubscribe_Cpp, DEFAULT_PUBLISH_PARAMETER,
		                         "Val",
		                         FString("Publish-Subscribe Publish"));
	}
}

void ASGTestCppUnsubscribe::OnPublish(const FSGMessage& Message,
                                      const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestCppUnsubscribe::OnPublish IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}
