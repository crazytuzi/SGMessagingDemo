// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessageBus.h"
#include "SGBlueprintMessageBus.generated.h"

/**
 * 
 */
UCLASS()
class SGMESSAGING_API USGBlueprintMessageBus : public UObject
{
	GENERATED_BODY()

public:
	USGBlueprintMessageBus();

	virtual ~USGBlueprintMessageBus() override;

	operator TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>() const
	{
		return MessageBus.ToSharedRef();
	}

public:
	TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> GetMessageBus();

private:
	TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> MessageBus;
};
