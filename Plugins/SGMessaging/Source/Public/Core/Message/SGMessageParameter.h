#pragma once

#include "CoreMinimal.h"
#include "Containers/MapBuilder.h"
#include "Core/Interface/ISGMessageContext.h"

struct FSGMessageParameter
{
	struct FSGSendParameter
	{
		explicit FSGSendParameter(const ESGMessageFlags InFlags = ESGMessageFlags::None,
		                          const TMap<FName, FString>& InAnnotations = TMapBuilder<FName, FString>(),
		                          const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& InAttachment = nullptr,
		                          const FTimespan& InDelay = FTimespan::Zero(),
		                          const FDateTime& InExpiration = FDateTime::MaxValue())
			: Flags(InFlags)
			  , Annotations(InAnnotations)
			  , Attachment(InAttachment)
			  , Delay(InDelay)
			  , Expiration(InExpiration)
		{
		}

		ESGMessageFlags Flags;

		TMap<FName, FString> Annotations;

		TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> Attachment;

		FTimespan Delay;

		FDateTime Expiration;
	};

	struct FSGPublishParameter
	{
		explicit FSGPublishParameter(
			const ESGMessageScope InScope = ESGMessageScope::Network,
			const TMap<FName, FString>& InAnnotations = TMapBuilder<FName, FString>(),
			const FTimespan& InDelay = FTimespan::Zero(),
			const FDateTime& InExpiration = FDateTime::MaxValue())
			: Scope(InScope)
			  , Annotations(InAnnotations)
			  , Delay(InDelay)
			  , Expiration(InExpiration)
		{
		}

		ESGMessageScope Scope;

		TMap<FName, FString> Annotations;

		FTimespan Delay;

		FDateTime Expiration;
	};

	typedef FSGSendParameter FSendParameter;

	typedef FSGPublishParameter FPublishParameter;

	FORCEINLINE static const FSendParameter& GetDefaultSendParameter()
	{
		static auto DefaultSend = FSendParameter();

		return DefaultSend;
	}

	FORCEINLINE static const FSendParameter& GetDelaySendParameter(const FTimespan& InDelay)
	{
		static auto DelaySend = FSendParameter();

		DelaySend.Delay = InDelay;

		return DelaySend;
	}

	FORCEINLINE static const FPublishParameter& GetDefaultPublishParameter()
	{
		static auto DefaultPublish = FPublishParameter();

		return DefaultPublish;
	}

	FORCEINLINE static const FPublishParameter& GetDelayPublishParameter(const FTimespan& InDelay)
	{
		static auto DelayPublish = FPublishParameter();

		DelayPublish.Delay = InDelay;

		return DelayPublish;
	}
};

#define MESSAGE_PARAMETER InParameter
#define CONST_SEND_PARAMETER_SIGNATURE const FSGMessageParameter::FSendParameter& MESSAGE_PARAMETER
#define SEND_PARAMETER_FORWARD MESSAGE_PARAMETER.Flags, MESSAGE_PARAMETER.Annotations, MESSAGE_PARAMETER.Attachment, MESSAGE_PARAMETER.Delay, MESSAGE_PARAMETER.Expiration
#define CONST_PUBLISH_PARAMETER_SIGNATURE const FSGMessageParameter::FPublishParameter& MESSAGE_PARAMETER
#define PUBLISH_PARAMETER_FORWARD MESSAGE_PARAMETER.Scope, MESSAGE_PARAMETER.Annotations, MESSAGE_PARAMETER.Delay, MESSAGE_PARAMETER.Expiration
#define DEFAULT_SEND_PARAMETER FSGMessageParameter::GetDefaultSendParameter()
#define DEFAULT_PUBLISH_PARAMETER FSGMessageParameter::GetDefaultPublishParameter()
#define DELAY_SEND_PARAMETER(InDelay) FSGMessageParameter::GetDelaySendParameter(InDelay)
#define DELAY_PUBLISH_PARAMETER(InDelay) FSGMessageParameter::GetDelayPublishParameter(InDelay)
