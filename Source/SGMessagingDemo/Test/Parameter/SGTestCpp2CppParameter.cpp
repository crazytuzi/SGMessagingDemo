// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestCpp2CppParameter.h"
#include "SGTestParameterMacro.h"
#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SGMessagingDemo/Test/SGMessagingType.h"
#include "SGMessagingDemo/Test/Subsystem/SGMessagingTestSubsystem.h"

// Sets default values
ASGTestCpp2CppParameter::ASGTestCpp2CppParameter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASGTestCpp2CppParameter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		MessagingTestSubsystem->TestCpp2CppParameterDelegate.AddDynamic(
			this, &ASGTestCpp2CppParameter::OnDelegateBroadcast);
	}

	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		MessageEndpoint->Subscribe(Topic_Parameter, TopicParameter_Cpp2Cpp_Publish, this,
		                           &ASGTestCpp2CppParameter::OnPublish);
	}
}

// Called every frame
void ASGTestCpp2CppParameter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGTestCpp2CppParameter::OnDelegateBroadcast()
{
	if (const auto MessageEndpoint = USGMessageFunctionLibrary::GetDefaultMessageEndpoint(this))
	{
		if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
			USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
		{
			if (const auto Case = MessagingTestSubsystem->Case)
			{
				MessageEndpoint->Publish(Topic_Parameter, TopicParameter_Cpp2Cpp_Publish,DEFAULT_PUBLISH_PARAMETER,
				                         ADD_CASE(Int8),
				                         ADD_CASE(UInt8),
				                         ADD_CASE(Int16),
				                         ADD_CASE(UInt16),
				                         ADD_CASE(Int32),
				                         ADD_CASE(UInt32),
				                         ADD_CASE(Int64),
				                         ADD_CASE(UInt64),
				                         ADD_CASE(Float),
				                         ADD_CASE(Double),
				                         ADD_CASE(Enum),
				                         ADD_CASE(UEnum),
				                         ADD_CASE(EnumClass),
				                         ADD_CASE(UEnumClass),
				                         ADD_CASE(TEnumAsByte),
				                         ADD_CASE(TSubclassOf),
				                         ADD_CASE(Bool),
				                         ADD_CASE(UObject),
				                         ADD_CASE(TObjectPtr),
				                         ADD_CASE(TWeakObjectPtr),
				                         ADD_CASE(TLazyObjectPtr),
				                         ADD_CASE(TSoftObjectPtr),
				                         ADD_CASE(TSoftClassPtr),
				                         ADD_CASE(TScriptInterface),
				                         ADD_CASE(FName),
				                         ADD_CASE(FString),
				                         ADD_CASE(FText),
				                         ADD_CASE(TArray),
				                         ADD_CASE(TMap),
				                         ADD_CASE(TSet),
				                         ADD_CASE(UStruct),
				                         ADD_CASE(UStructPtr),
				                         ADD_CASE(Char),
				                         ADD_CASE(Ansichar),
				                         ADD_CASE(Class),
				                         ADD_CASE(Struct)
				);
			}
		}
	}
}

void ASGTestCpp2CppParameter::OnPublish(const FSGMessage& Message,
                                        const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogTemp, Log, TEXT("ASGTestCpp2CppParameter::OnPublish IsDedicatedServer:%s Name:%s"),
	       *UKismetStringLibrary::Conv_BoolToString(UKismetSystemLibrary::IsDedicatedServer(GetWorld())), *GetName());

	if (const auto MessagingTestSubsystem = Cast<USGMessagingTestSubsystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorld(), USGMessagingTestSubsystem::StaticClass())))
	{
		if (const auto Case = MessagingTestSubsystem->Case)
		{
			LOG_TYPE_VALIDATE_CASE(Int8, int8)
			LOG_TYPE_VALIDATE_CASE(UInt8, uint8)
			LOG_TYPE_VALIDATE_CASE(Int16, int16)
			LOG_TYPE_VALIDATE_CASE(UInt16, uint16)
			LOG_TYPE_VALIDATE_CASE(Int32, int32)
			LOG_TYPE_VALIDATE_CASE(UInt32, uint32)
			LOG_TYPE_VALIDATE_CASE(Int64, int64)
			LOG_TYPE_VALIDATE_CASE(UInt64, uint64)
			LOG_TYPE_VALIDATE_CASE(Float, float)
			LOG_TYPE_VALIDATE_CASE(Double, double)
			LOG_TYPE_VALIDATE_CASE(Enum, ESGTestEnumCase)
			LOG_TYPE_VALIDATE_CASE(UEnum, ESGTestUEnumCase)
			LOG_TYPE_VALIDATE_CASE(EnumClass, ESGTestEnumClassCase)
			LOG_TYPE_VALIDATE_CASE(UEnumClass, ESGTestUEnumClassCase)
			LOG_TYPE_VALIDATE_CASE(TSubclassOf, TSubclassOf<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(Bool, bool)
			LOG_TYPE_VALIDATE_CASE(UObject, UObject*)
			LOG_TYPE_VALIDATE_CASE(TObjectPtr, TObjectPtr<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(TWeakObjectPtr, TWeakObjectPtr<USGTestParameterCase>)
			LOG_TYPE_VALIDATE_CASE(TLazyObjectPtr, TLazyObjectPtr<USGTestParameterCase>)
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
			LOG_CONDITION_VALIDATE_CASE(
				UStructPtr,
				Case->UStructPtrValue->Int32Val == Message.Get<FSGTestUStructCase*>(Case->UStructPtrKey)->Int32Val)
			LOG_TYPE_VALIDATE_CASE(Char, const char*)
			LOG_TYPE_VALIDATE_CASE(Ansichar, ANSICHAR*)
			LOG_CONDITION_VALIDATE_CASE(
				Class, Case->ClassValue.Int32Val == Message.Get<SGTestClassCase>(Case->ClassKey).Int32Val)
			LOG_CONDITION_VALIDATE_CASE(
				Struct, Case->StructValue.Int32Val == Message.Get<SGTestStructCase>(Case->StructKey).Int32Val)
		}
	}
}
