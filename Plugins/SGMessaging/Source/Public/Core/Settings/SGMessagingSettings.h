// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SGMessagingSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, Defaultconfig)
class SGMESSAGING_API USGMessagingSettings final : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	bool bAllowDelayedMessaging = false;
};
