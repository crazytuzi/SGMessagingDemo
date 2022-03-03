// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SGMessagingTestSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestForwardReply);

/**
 * 
 */
UCLASS()
class SGMESSAGINGDEMO_API USGMessagingTestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void TestPublishSubscribe();

	UFUNCTION(BlueprintCallable)
	void TestRequestReply();

	UFUNCTION(BlueprintCallable)
	void TestForward();

public:
	UPROPERTY(BlueprintAssignable)
	FTestPublishSubscribe TestPublishSubscribeDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestRequestReply TestRequestReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestForwardReply TestForwardReplyDelegate;
};
