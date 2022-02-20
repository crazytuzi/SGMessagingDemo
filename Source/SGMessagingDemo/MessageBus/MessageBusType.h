// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageBusType.generated.h"

USTRUCT()
struct FTestPublishMessage
{
	GENERATED_USTRUCT_BODY()

	FString Val;

	FTestPublishMessage() = default;

	explicit FTestPublishMessage(FString InVal): Val(InVal)
	{
	};
};

USTRUCT()
struct FTestRequestMessage
{
	GENERATED_USTRUCT_BODY()

	FString Val;

	FTestRequestMessage() = default;

	explicit FTestRequestMessage(FString InVal): Val(InVal)
	{
	};
};

USTRUCT()
struct FTestReplyMessage
{
	GENERATED_USTRUCT_BODY()

	FString Val;

	FTestReplyMessage() = default;

	explicit FTestReplyMessage(FString InVal): Val(InVal)
	{
	};
};
