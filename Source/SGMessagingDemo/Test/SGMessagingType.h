// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGMessagingType.generated.h"

UENUM(BlueprintType)
enum ETopicID
{
	Topic_PublishSubscribe,
	Topic_RequestReply,
	Topic_ForwardReply,
	Topic_DelayPublishSubscribe,
	Topic_DelayRequestReply,
	Topic_DelayForwardReply,
	Topic_BlueprintPublishSubscribe,
	Topic_BlueprintRequestReply,
	Topic_BlueprintForwardReply,
	Topic_BlueprintDelayPublishSubscribe,
	Topic_BlueprintDelayRequestReply,
	Topic_BlueprintDelayForwardReply,
	Topic_Parameter
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

UENUM(BlueprintType)
enum ETopicDelayPublishSubscribe_MessageID
{
	TopicDelayPublishSubscribe_Publish
};

UENUM(BlueprintType)
enum ETopicDelayRequestReply_MessageID
{
	TopicDelayRequestReply_Request,
	TopicDelayRequestReply_Reply
};

UENUM(BlueprintType)
enum ETopicDelayForwardReply_MessageID
{
	TopicDelayForwardReply_Request,
	TopicDelayForwardReply_Reply
};

UENUM(BlueprintType)
enum ETopicBlueprintPublishSubscribe_MessageID
{
	TopicBlueprintPublishSubscribe_Publish
};

UENUM(BlueprintType)
enum ETopicBlueprintRequestReply_MessageID
{
	TopicBlueprintRequestReply_Request,
	TopicBlueprintRequestReply_Reply
};

UENUM(BlueprintType)
enum ETopicBlueprintForwardReply_MessageID
{
	TopicBlueprintForwardReply_Request,
	TopicBlueprintForwardReply_Reply
};

UENUM(BlueprintType)
enum ETopicBlueprintDelayPublishSubscribe_MessageID
{
	TopicBlueprintDelayPublishSubscribe_Publish
};

UENUM(BlueprintType)
enum ETopicBlueprintDelayRequestReply_MessageID
{
	TopicBlueprintDelayRequestReply_Request,
	TopicBlueprintDelayRequestReply_Reply
};

UENUM(BlueprintType)
enum ETopicBlueprintDelayForwardReply_MessageID
{
	TopicBlueprintDelayForwardReply_Request,
	TopicBlueprintDelayForwardReply_Reply
};

UENUM(BlueprintType)
enum ETopicParameter_MessageID
{
	TopicParameter_Cpp2Cpp_Publish,
	TopicParameter_Cpp2BP_Publish,
	TopicParameter_BP2BP_Publish,
};
