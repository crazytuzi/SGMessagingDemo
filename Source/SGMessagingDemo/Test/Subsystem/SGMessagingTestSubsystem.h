// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGMessagingDemo/Test/Parameter/SGTestParameterCase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SGMessagingTestSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestForwardReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelayForwardReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintForwardReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintDelayPublishSubscribe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintDelayRequestReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestBlueprintDelayForwardReply);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestCpp2CppParameter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestCpp2BPParameter);

/**
 * 
 */
UCLASS()
class SGMESSAGINGDEMO_API USGMessagingTestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

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

	UFUNCTION(BlueprintCallable)
	void TestBlueprintPublishSubscribe();

	UFUNCTION(BlueprintCallable)
	void TestBlueprintRequestReply();

	UFUNCTION(BlueprintCallable)
	void TestBlueprintForwardReply();

	UFUNCTION(BlueprintCallable)
	void TestBlueprintDelayPublishSubscribe();

	UFUNCTION(BlueprintCallable)
	void TestBlueprintDelayRequestReply();

	UFUNCTION(BlueprintCallable)
	void TestBlueprintDelayForwardReply();

	UFUNCTION(BlueprintCallable)
	void TestCpp2CppParameter();

	UFUNCTION(BlueprintCallable)
	void TestCpp2BPParameter();

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

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintPublishSubscribe TestBlueprintPublishSubscribeDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintRequestReply TestBlueprintRequestReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintForwardReply TestBlueprintForwardReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintDelayPublishSubscribe TestBlueprintDelayPublishSubscribeDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintDelayRequestReply TestBlueprintDelayRequestReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestBlueprintDelayForwardReply TestBlueprintDelayForwardReplyDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestCpp2CppParameter TestCpp2CppParameterDelegate;

	UPROPERTY(BlueprintAssignable)
	FTestCpp2CppParameter TestCpp2BPParameterDelegate;

public:
	UPROPERTY(BlueprintReadOnly)
	USGTestParameterCase* Case;
};
