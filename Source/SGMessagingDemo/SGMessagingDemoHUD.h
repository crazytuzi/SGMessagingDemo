// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SGMessagingDemoHUD.generated.h"

UCLASS()
class ASGMessagingDemoHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASGMessagingDemoHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

