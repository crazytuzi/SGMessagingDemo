// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/ISGMessageContext.h"
#include "Core/Message/SGMessage.h"
#include "SGTestCppUnsubscribe.generated.h"

UCLASS()
class SGMESSAGINGDEMO_API ASGTestCppUnsubscribe : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASGTestCppUnsubscribe();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnCppSubscribeDelegateBroadcast();

	UFUNCTION()
	void OnCppUnsubscribeDelegateBroadcast();

private:
	void OnPublish(const FSGMessage& Message, const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context);
};
