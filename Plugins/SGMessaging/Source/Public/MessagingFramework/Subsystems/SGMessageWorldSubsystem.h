// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/Bus/SGBlueprintMessageBus.h"
#include "Blueprint/Common/SGBlueprintMessageEndpoint.h"
#include "Subsystems/WorldSubsystem.h"
#include "SGMessageWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SGMESSAGING_API USGMessageWorldSubsystem final : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable)
	USGBlueprintMessageBus* GetDefaultBus() const;

	UFUNCTION(BlueprintCallable)
	USGBlueprintMessageEndpoint* GetDefaultMessageEndpoint() const;

private:
	UPROPERTY()
	USGBlueprintMessageBus* DefaultBus;

	UPROPERTY()
	USGBlueprintMessageEndpoint* DefaultMessageEndpoint;
};
