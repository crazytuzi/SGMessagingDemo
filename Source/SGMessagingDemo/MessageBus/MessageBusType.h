// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageBusType.generated.h"

UENUM(BlueprintType)
enum ETopicID
{
	TopicA,
	TopicB
};

UENUM(BlueprintType)
enum ETopicA_MessageID
{
	TopicA_MessageID1,
	TopicA_MessageID2
};

UENUM(BlueprintType)
enum ETopicB_MessageID
{
	TopicB_MessageID1,
	TopicB_MessageID2
};
