// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGMessagingType.generated.h"

UENUM(BlueprintType)
enum ETopicID
{
	Topic_PublishSubscribe,
	Topic_RequestReply,
	Topic_ForwardReply
};

UENUM(BlueprintType)
enum ETopicPublishSubscribe_MessageID
{
	TopicPublishSubscribe_Publish
};

UENUM(BlueprintType)
enum ETopicRequestReply_MessageID
{
	TopicRequestReply_Request,
	TopicRequestReply_Reply
};

UENUM(BlueprintType)
enum ETopicForwardReply_MessageID
{
	TopicForwardReply_Request,
	TopicForwardReply_Reply
};
