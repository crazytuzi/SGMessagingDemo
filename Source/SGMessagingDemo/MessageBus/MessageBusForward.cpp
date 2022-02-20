// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusForward.h"
#include "Common/SGMessageEndpointBuilder.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMessageBusForward::AMessageBusForward()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMessageBusForward::BeginPlay()
{
	Super::BeginPlay();

	MessageBus = ISGMessagingModule::Get().GetDefaultBus();

	MessageEndpoint = FSGMessageEndpoint::Builder("Request-Reply", MessageBus.ToSharedRef()).Handling<
		FTestRequestMessage>(this, &AMessageBusForward::OnForward);
}

// Called every frame
void AMessageBusForward::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMessageBusForward::OnForward(const FTestRequestMessage& Message,
                                   const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("AMessageBusForward::OnForward IsDedicatedServer:%s Name:%s => %s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName(),
	       *Message.Val);

	MessageEndpoint->Send<FTestReplyMessage>(new FTestReplyMessage("OnForward-Reply"), Context->GetSender());
}

