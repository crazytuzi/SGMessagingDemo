// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestCpp2BPParameter.h"
#include "SGTestParameterMacro.h"
#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

// Sets default values
ASGTestCpp2BPParameter::ASGTestCpp2BPParameter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGTestCpp2BPParameter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestCpp2BPParameterDelegate.AddDynamic(
			this, &ASGTestCpp2BPParameter::OnDelegateBroadcast);
	}
}

// Called every frame
void ASGTestCpp2BPParameter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGTestCpp2BPParameter::OnDelegateBroadcast()
{
	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
			USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
		{
			if (const auto Case = MessagingTestSubsystem->Case)
			{
				MessageEndpoint->Publish(Topic_Parameter, TopicParameter_Cpp2BP_Publish,DEFAULT_PUBLISH_PARAMETER,
				                         ADD_CASE(UInt8),
				                         ADD_CASE(Int32),
				                         ADD_CASE(Int64),
				                         ADD_CASE(Float),
				                         ADD_CASE(Double),
				                         ADD_CASE(UEnumClass),
				                         ADD_CASE(TEnumAsByte),
				                         ADD_CASE(TSubclassOf),
				                         ADD_CASE(Bool),
				                         ADD_CASE(UObject),
				                         ADD_CASE(TObjectPtr),
				                         ADD_CASE(TSoftObjectPtr),
				                         ADD_CASE(TSoftClassPtr),
				                         ADD_CASE(TScriptInterface),
				                         ADD_CASE(FName),
				                         ADD_CASE(FString),
				                         ADD_CASE(FText),
				                         ADD_CASE(TArray),
				                         ADD_CASE(TMap),
				                         ADD_CASE(TSet),
				                         ADD_CASE(UStruct)
				);
			}
		}
	}
}

