// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestRequest.h"
#include "EngineUtils.h"
#include "SGTestReply.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestRequest::ASGTestRequest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestRequest::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestForwardReplyDelegate.AddDynamic(
			this, &ASGTestRequest::OnDelegateBroadcast);
	}

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Forward-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_ForwardReply, TopicForwardReply_Reply, this, &ASGTestRequest::OnReply);
}

// Called every frame
void ASGTestRequest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestRequest::OnDelegateBroadcast()
{
	TArray<FSGMessageAddress> Recipients;

	for (TActorIterator<ASGTestReply> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		Recipients.Add(Iterator->MessageEndpoint->GetAddress());
	}

	MessageEndpoint->Send(Topic_ForwardReply, TopicForwardReply_Request, Recipients, DEFAULT_SEND_PARAMETER, "Val",
	                      FString("Forward-Reply Request"));
}

void ASGTestRequest::OnReply(const FSGMessage& Message,
                             const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestRequest::OnReply IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));
}

