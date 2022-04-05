#pragma once

#include "CoreMinimal.h"
#include "Blueprint/Bus/SGBlueprintMessageContext.h"
#include "Core/Message/SGMessageParameter.h"
#include "SGBlueprintMessageParameter.generated.h"

USTRUCT(BlueprintType)
struct FSGBlueprintSendParameter
{
	GENERATED_BODY()

	FSGBlueprintSendParameter() = default;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<ESGBlueprintMessageFlags> Flags;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> Annotations;

	TSharedPtr<ISGMessageAttachment, ESPMode::ThreadSafe> Attachment;

	UPROPERTY(BlueprintReadWrite)
	FTimespan Delay;

	UPROPERTY(BlueprintReadWrite)
	FDateTime Expiration;

	explicit operator FSGMessageParameter::FSendParameter() const
	{
		return FSGMessageParameter::FSendParameter(
			static_cast<ESGMessageFlags>(Flags.GetValue()),
			Annotations,
			Attachment,
			Delay,
			Expiration);
	}
};

USTRUCT(BlueprintType)
struct FSGBlueprintPublishParameter
{
	GENERATED_BODY()

	FSGBlueprintPublishParameter() = default;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<ESGBlueprintMessageScope> Scope;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> Annotations;

	UPROPERTY(BlueprintReadWrite)
	FTimespan Delay;

	UPROPERTY(BlueprintReadWrite)
	FDateTime Expiration;

	explicit operator FSGMessageParameter::FPublishParameter() const
	{
		return FSGMessageParameter::FPublishParameter(
			static_cast<ESGMessageScope>(Scope.GetValue()),
			Annotations,
			Delay,
			Expiration);
	}
};
