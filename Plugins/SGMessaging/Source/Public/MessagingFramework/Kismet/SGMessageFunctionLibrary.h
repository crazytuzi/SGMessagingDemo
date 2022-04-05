// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/Bus/SGBlueprintMessageBus.h"
#include "Blueprint/Common/SGBlueprintMessageEndpoint.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGMessageFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SGMESSAGING_API USGMessageFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static USGBlueprintMessageBus* GetDefaultBus(UObject* WorldContextObject = nullptr);

	UFUNCTION(BlueprintCallable)
	static USGBlueprintMessageEndpoint* GetDefaultMessageEndpoint(UObject* WorldContextObject = nullptr);

	UFUNCTION(BlueprintCallable)
	static FSGBlueprintMessage BuildBlueprintMessage();

	UFUNCTION(BlueprintCallable)
	static FSGBlueprintMessageAddress GetSender(const FSGBlueprintMessageContext& InContent);

	UFUNCTION(BlueprintCallable, CustomThunk,
		meta = (CustomStructureParam = "Variable", AutoCreateRefTerm = "Variable"))
	static FSGBlueprintMessage Set(const FSGBlueprintMessage& Message, FName Key, const int32& Variable);
	DECLARE_FUNCTION(execSet)
	{
		P_GET_STRUCT(FSGBlueprintMessage, Message);

		P_GET_PROPERTY(FNameProperty, Key);

		Stack.StepCompiledIn<FStructProperty>(nullptr);

		FProperty* Property = CastField<FProperty>(Stack.MostRecentProperty);

		const void* PropertyAddress = Stack.MostRecentPropertyAddress;

		P_FINISH;

		P_NATIVE_BEGIN;
			*static_cast<FSGBlueprintMessage*>(RESULT_PARAM) = ExecSet(Message, Key, Property, PropertyAddress);
		P_NATIVE_END;
	}

	UFUNCTION(BlueprintCallable, CustomThunk,
		meta = (CustomStructureParam = "Variable", AutoCreateRefTerm = "Variable"))
	static FSGBlueprintMessage Get(const FSGBlueprintMessage& Message, FName Key, int32 Variable);
	DECLARE_FUNCTION(execGet)
	{
		P_GET_STRUCT(FSGBlueprintMessage, Message);

		P_GET_PROPERTY(FNameProperty, Key);

		Stack.StepCompiledIn<FStructProperty>(nullptr);

		FProperty* Property = CastField<FProperty>(Stack.MostRecentProperty);

		void* PropertyAddress = Stack.MostRecentPropertyAddress;

		P_FINISH;

		P_NATIVE_BEGIN;
			*static_cast<FSGBlueprintMessage*>(RESULT_PARAM) = ExecGet(Message, Key, Property, PropertyAddress);
		P_NATIVE_END;
	}

public:
	static FSGBlueprintMessage ExecSet(const FSGBlueprintMessage& Message, const FName& Key, FProperty* InProperty,
	                                   const void* PropertyAddress);

	static FSGBlueprintMessage ExecGet(const FSGBlueprintMessage& Message, const FName& Key, FProperty* InProperty,
	                                   void* PropertyAddress);
};
