// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestForward.h"
#include "EngineUtils.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"

// Sets default values
ASGTestForward::ASGTestForward()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestForward::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus(this);

	MessageEndpoint = FSGMessageEndpoint::Builder("Forward-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_ForwardReply, TopicForwardReply_Request, this, &ASGTestForward::OnForward);
}

// Called every frame
void ASGTestForward::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestForward::OnForward(const FSGMessage& Message,
                               const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestForward::OnForward IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	MessageEndpoint->Send(Topic_ForwardReply, TopicForwardReply_Reply, Context->GetSender(), DEFAULT_SEND_PARAMETER,
	                      "Val", FString("Forward-Request Forward"));
}

