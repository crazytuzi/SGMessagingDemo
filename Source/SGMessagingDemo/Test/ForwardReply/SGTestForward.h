// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MessagingFramework/Components/SGMessageEndpointComponent.h"
#include "SGTestForward.generated.h"

UCLASS()
class SGMESSAGINGDEMO_API ASGTestForward : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASGTestForward();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void OnForward(const FSGMessage& Message, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context);

private:
	UPROPERTY()
	USGMessageEndpointComponent* MessageEndpointComponent;
};
