// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MessagingFramework/Components/SGMessageEndpointComponent.h"
#include "SGTestRequestReply.generated.h"

UCLASS()
class SGMESSAGINGDEMO_API ASGTestRequestReply : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASGTestRequestReply();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnDelegateBroadcast();

private:
	void OnRequest(const FSGMessage& Message, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context);

	void OnReply(const FSGMessage& Message, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context);

private:
	UPROPERTY()
	USGMessageEndpointComponent* MessageEndpointComponent;
};
