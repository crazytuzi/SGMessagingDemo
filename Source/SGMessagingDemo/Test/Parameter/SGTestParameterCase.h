// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGTestInterfaceCase.h"
#include "SGTestParameterCase.generated.h"

UENUM(BlueprintType)
enum ESGTestUEnumCase
{
	UEnumZero,
	UEnumOne
};

enum ESGTestEnumCase
{
	EnumZero,
	EnumOne
};

UENUM(BlueprintType)
enum class ESGTestUEnumClassCase:uint8
{
	UEnumClassZero,
	UEnumClassOne
};

enum class ESGTestEnumClassCase
{
	EnumClassZero,
	EnumClassOne
};

USTRUCT(BlueprintType)
struct FSGTestUStructCase
{
	GENERATED_BODY()

	FSGTestUStructCase() = default;

	FSGTestUStructCase(const int32 InInt32Val): Int32Val(InInt32Val)
	{
	}

	int32 Int32Val;
};

class SGTestClassCase
{
public:
	SGTestClassCase() = default;

	SGTestClassCase(const int32 InInt32Val): Int32Val(InInt32Val)
	{
	}

public:
	int32 Int32Val;
};

struct SGTestStructCase
{
public:
	SGTestStructCase() = default;

	SGTestStructCase(const int32 InInt32Val): Int32Val(InInt32Val)
	{
	}

public:
	int32 Int32Val;
};

/**
 * 
 */
UCLASS()
class SGMESSAGINGDEMO_API USGTestParameterCase : public UObject, public ISGTestInterfaceCase
{
	GENERATED_BODY()

public:
	void Initialize();

	// MulticastInlineDelegate,
	// MulticastSparseDelegate

public:
	UFUNCTION(BlueprintCallable)
	bool IsEqualTMap(const TMap<int, int>& InMap);

	UFUNCTION(BlueprintCallable)
	bool IsEqualTSet(const TSet<int>& InSet);

public:
	UPROPERTY(BlueprintReadOnly)
	FString Int8Key = "Int8";

	int8 Int8Value;

	UPROPERTY(BlueprintReadOnly)
	FString UInt8Key = "UInt8";

	UPROPERTY(BlueprintReadOnly)
	uint8 UInt8Value;

	UPROPERTY(BlueprintReadOnly)
	FString Int16Key = "Int16";

	int16 Int16Value;

	UPROPERTY(BlueprintReadOnly)
	FString UInt16Key = "UInt16";

	uint16 UInt16Value;

	UPROPERTY(BlueprintReadOnly)
	FString Int32Key = "Int32";

	UPROPERTY(BlueprintReadOnly)
	int32 Int32Value;

	UPROPERTY(BlueprintReadOnly)
	FString UInt32Key = "UInt32";

	uint32 UInt32Value;

	UPROPERTY(BlueprintReadOnly)
	FString Int64Key = "Int64";

	UPROPERTY(BlueprintReadOnly)
	int64 Int64Value;

	UPROPERTY(BlueprintReadOnly)
	FString UInt64Key = "UInt64";

	uint64 UInt64Value;

	UPROPERTY(BlueprintReadOnly)
	FString FloatKey = "Float";

	UPROPERTY(BlueprintReadOnly)
	float FloatValue;

	UPROPERTY(BlueprintReadOnly)
	FString DoubleKey = "Double";

	UPROPERTY(BlueprintReadOnly)
	double DoubleValue;

	UPROPERTY(BlueprintReadOnly)
	FString EnumKey = "Enum";

	ESGTestEnumCase EnumValue;

	UPROPERTY(BlueprintReadOnly)
	FString UEnumKey = "UEnum";

	ESGTestUEnumCase UEnumValue;

	UPROPERTY(BlueprintReadOnly)
	FString EnumClassKey = "EnumClass";

	ESGTestEnumClassCase EnumClassValue;

	UPROPERTY(BlueprintReadOnly)
	FString UEnumClassKey = "UEnumClass";

	UPROPERTY(BlueprintReadOnly)
	ESGTestUEnumClassCase UEnumClassValue;

	UPROPERTY(BlueprintReadOnly)
	FString TEnumAsByteKey = "TEnumAsByte";

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ESGTestUEnumCase> TEnumAsByteValue;

	UPROPERTY(BlueprintReadOnly)
	FString TSubclassOfKey = "TSubclassOf";

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<USGTestParameterCase> TSubclassOfValue;

	UPROPERTY(BlueprintReadOnly)
	FString BoolKey = "Bool";

	UPROPERTY(BlueprintReadOnly)
	bool BoolValue;

	UPROPERTY(BlueprintReadOnly)
	FString UObjectKey = "UObject";

	UPROPERTY(BlueprintReadOnly)
	UObject* UObjectValue;

	UPROPERTY(BlueprintReadOnly)
	FString TObjectPtrKey = "TObjectPtr";

	TObjectPtr<USGTestParameterCase> TObjectPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString TWeakObjectPtrKey = "TWeakObjectPtr";

	TWeakObjectPtr<USGTestParameterCase> TWeakObjectPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString TLazyObjectPtrKey = "TLazyObjectPtr";

	TLazyObjectPtr<USGTestParameterCase> TLazyObjectPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString TSoftObjectPtrKey = "TSoftObjectPtr";

	TSoftObjectPtr<USGTestParameterCase> TSoftObjectPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString TSoftClassPtrKey = "TSoftClassPtr";

	TSoftClassPtr<USGTestParameterCase> TSoftClassPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString TScriptInterfaceKey = "TScriptInterface";

	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ISGTestInterfaceCase> TScriptInterfaceValue;

	UPROPERTY(BlueprintReadOnly)
	FString FNameKey = "FName";

	UPROPERTY(BlueprintReadOnly)
	FName FNameValue;

	UPROPERTY(BlueprintReadOnly)
	FString FStringKey = "FString";

	UPROPERTY(BlueprintReadOnly)
	FString FStringValue;

	UPROPERTY(BlueprintReadOnly)
	FString FTextKey = "FText";

	UPROPERTY(BlueprintReadOnly)
	FText FTextValue;

	UPROPERTY(BlueprintReadOnly)
	FString TArrayKey = "TArray";

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> TArrayValue;

	UPROPERTY(BlueprintReadOnly)
	FString TMapKey = "TMap";

	UPROPERTY(BlueprintReadOnly)
	TMap<int32, int32> TMapValue;

	UPROPERTY(BlueprintReadOnly)
	FString TSetKey = "TSet";

	UPROPERTY(BlueprintReadOnly)
	TSet<int32> TSetValue;

	UPROPERTY(BlueprintReadOnly)
	FString UStructKey = "UStruct";

	UPROPERTY(BlueprintReadOnly)
	FSGTestUStructCase UStructValue;

	UPROPERTY(BlueprintReadOnly)
	FString UStructPtrKey = "UStructPtr";

	FSGTestUStructCase* UStructPtrValue;

	UPROPERTY(BlueprintReadOnly)
	FString CharKey = "Char";

	const char* CharValue;

	UPROPERTY(BlueprintReadOnly)
	FString AnsicharKey = "Ansichar";

	ANSICHAR* AnsicharValue;

	UPROPERTY(BlueprintReadOnly)
	FString ClassKey = "Class";

	SGTestClassCase ClassValue;

	UPROPERTY(BlueprintReadOnly)
	FString StructKey = "Struct";

	SGTestStructCase StructValue;
};
