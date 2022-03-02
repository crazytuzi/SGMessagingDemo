#pragma once

#include "CoreMinimal.h"
#include "Containers/MapBuilder.h"
#include "Interface/ISGMessageContext.h"

struct FSGMessageParameter
{
	struct FSGSendParameter
	{
		FSGSendParameter(const ESGMessageFlags InFlags = ESGMessageFlags::None,
		                 const TMap<FName, FString>& InAnnotations = TMapBuilder<FName, FString>(),
		                 const TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe>& InAttachment = nullptr,
		                 const FTimespan& InDelay = FTimespan::Zero(),
		                 const FDateTime& InExpiration = FDateTime::MaxValue()):
			Flags(InFlags),
			Annotations(InAnnotations),
			Attachment(InAttachment),
			Delay(InDelay),
			Expiration(InExpiration)
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
		FSGPublishParameter(
			const ESGMessageScope InScope = ESGMessageScope::Network,
			const TMap<FName, FString>& InAnnotations = TMapBuilder<FName, FString>(),
			const FTimespan& InDelay = FTimespan::Zero(),
			const FDateTime& InExpiration = FDateTime::MaxValue()):
			Scope(InScope),
			Annotations(InAnnotations),
			Delay(InDelay),
			Expiration(InExpiration)
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

	FORCEINLINE static const FPublishParameter& GetDefaultPublishParameter()
	{
		static auto DefaultPublish = FPublishParameter();

		return DefaultPublish;
	}
};

#define MESSAGE_PARAMETER Parameter
#define CONST_SEND_PARAMETER const FSGMessageParameter::FSendParameter& MESSAGE_PARAMETER
#define SEND_PARAMETER_FORWARD Parameter.Flags, Parameter.Annotations, Parameter.Attachment, Parameter.Delay, Parameter.Expiration
#define CONST_PUBLISH_PARAMETER const FSGMessageParameter::FPublishParameter& MESSAGE_PARAMETER
#define PUBLISH_PARAMETER_FORWARD MESSAGE_PARAMETER.Scope, MESSAGE_PARAMETER.Annotations, MESSAGE_PARAMETER.Delay, MESSAGE_PARAMETER.Expiration
#define DEFAULT_PUBLISH_PARAMETER FSGMessageParameter::GetDefaultPublishParameter()
#define DEFAULT_SEND_PARAMETER FSGMessageParameter::GetDefaultSendParameter()
