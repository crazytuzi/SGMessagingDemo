// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SGMessagingTestSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestForwardReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayForwardReply);

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
	void TestForwardReply();

	UFUNCTION(BlueprintCallable)
	void TestDelayPublishSubscribe();

	UFUNCTION(BlueprintCallable)
	void TestDelayRequestReply();

	UFUNCTION(BlueprintCallable)
	void TestDelayForwardReply();

public:
	UPROPERTY(BlueprintAssignable)
	FTestPublishSubscribe TestPublishSubscribeDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestRequestReply TestRequestReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestForwardReply TestForwardReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestDelayPublishSubscribe TestDelayPublishSubscribeDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestDelayRequestReply TestDelayRequestReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestDelayForwardReply TestDelayForwardReplyDelegate;
};
