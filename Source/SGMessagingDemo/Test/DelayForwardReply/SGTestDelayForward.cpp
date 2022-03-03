// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestDelayForward.h"
#include "EngineUtils.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"

// Sets default values
ASGTestDelayForward::ASGTestDelayForward()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestDelayForward::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("DelayForward-Reply", MessageBus.ToSharedRef());

	MessageEndpoint->Subscribe(Topic_DelayForwardReply, TopicDelayForwardReply_Request, this, &ASGTestDelayForward::OnForward);
}

// Called every frame
void ASGTestDelayForward::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestDelayForward::OnForward(const FSGMessage& Message,
                               const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestForward::OnForward IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Get<FString>("Val"));

	const auto Parameter = FSGMessageParameter::FSendParameter(ESGMessageFlags::None, TMapBuilder<FName, FString>(),
	                                                           nullptr, FTimespan(0, 0, 5));


	MessageEndpoint->Send(Topic_DelayForwardReply, TopicDelayForwardReply_Reply, Context->GetSender(), Parameter,
	                      "Val", FString("DelayForward-Request Forward"));
}

