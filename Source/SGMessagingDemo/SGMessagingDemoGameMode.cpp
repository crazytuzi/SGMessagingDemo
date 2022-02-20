// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGMessagingDemoGameMode.h"
#include "SGMessagingDemoHUD.h"
#include "SGMessagingDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASGMessagingDemoGameMode::ASGMessagingDemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASGMessagingDemoHUD::StaticClass();
}
