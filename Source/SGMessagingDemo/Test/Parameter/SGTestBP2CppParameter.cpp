// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestBP2CppParameter.h"
#include "SGTestParameterMacro.h"
#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestBP2CppParameter::ASGTestBP2CppParameter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASGTestBP2CppParameter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		MessageEndpoint->Subscribe(Topic_Parameter, TopicParameter_BP2Cpp_Publish, this,
		                           &ASGTestBP2CppParameter::OnPublish);
	}
}

// Called every frame
void ASGTestBP2CppParameter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestBP2CppParameter::OnPublish(const FSGMessage& Message,
                                       const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestBP2CppParameter::OnPublish IsDedicatedServer:%s Name:%s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName());

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		if (const auto Case = MessagingTestSubsystem->Case)
		{
			LOG_TYPE_VALIDATE_CASE(UInt8, uint8)
			LOG_TYPE_VALIDATE_CASE(Int32, int32)
			LOG_TYPE_VALIDATE_CASE(Int64, int64)
			LOG_TYPE_VALIDATE_CASE(Float, float)
			LOG_TYPE_VALIDATE_CASE(Double, double)
			LOG_TYPE_VALIDATE_CASE(UEnumClass, ESGTestUEnumClassCase)
			LOG_TYPE_VALIDATE_CASE(TSubclassOf, TSubclassOf<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(Bool, bool)
			LOG_TYPE_VALIDATE_CASE(UObject, UObject*)
			LOG_TYPE_VALIDATE_CASE(TObjectPtr, TObjectPtr<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(TSoftObjectPtr, TSoftObjectPtr<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(TSoftClassPtr, TSoftClassPtr<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(TScriptInterface, TScriptInterface<ISGTestInterfaceCase>)
			LOG_TYPE_VALIDATE_CASE(FName, FName)
			LOG_TYPE_VALIDATE_CASE(FString, FString)
			LOG_CONDITION_VALIDATE_CASE(FText, Case->FTextValue.EqualTo(Message.Get<FText>(Case->FTextKey)))
			LOG_TYPE_VALIDATE_CASE(TArray, TArray<int32>)
			LOG_CONDITION_VALIDATE_CASE(TMap, Case->IsEqualTMap(Message.Get<TMap<int32, int32>>(Case->TMapKey)))
			LOG_CONDITION_VALIDATE_CASE(TSet, Case->IsEqualTSet(Message.Get<TSet<int32>>(Case->TSetKey)))
			LOG_CONDITION_VALIDATE_CASE(
				UStruct, Case->UStructValue.Int32Val == Message.Get<FSGTestUStructCase>(Case->UStructKey).Int32Val)
		}
	}
}
