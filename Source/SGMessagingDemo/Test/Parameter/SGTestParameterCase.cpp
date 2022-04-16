// Fill out your copyright notice in the Description page of Project Settings.


#include "SGTestParameterCase.h"

void USGTestParameterCase::Initialize()
{
	Int8Value = 1;

	UInt8Value = 1;

	Int16Value = 1;

	UInt16Value = 1;

	Int32Value = 1;

	UInt32Value = 1;

	Int64Value = 1;

	UInt64Value = 1;

	FloatValue = 1.f;

	DoubleValue = 1.f;

	EnumValue = EnumOne;

	UEnumValue = UEnumOne;

	EnumClassValue = ESGTestEnumClassCase::EnumClassOne;

	UEnumClassValue = ESGTestUEnumClassCase::UEnumClassOne;

	TEnumAsByteValue = UEnumOne;

	TSubclassOfValue = this->GetClass();

	BoolValue = true;

	UObjectValue = this;

	TObjectPtrValue = this;

	TWeakObjectPtrValue = this;

	TLazyObjectPtrValue = this;

	TSoftObjectPtrValue = this;

	TSoftClassPtrValue = this->GetClass();

	TScriptInterfaceValue = this;

	FNameValue = FName("FName");

	FStringValue = FString("FString");

	FTextValue = FText::FromName("FText");

	TArrayValue = TArray<int32>{1};

	TMapValue = TMap<int32, int32>{{1, 1}};

	TSetValue = TSet<int32>{1};

	UStructValue = 1;

	UStructPtrValue = &UStructValue;

	CharValue = "Char";

	AnsicharValue = "Ansichar";

	ClassValue = 1;

	StructValue = 1;
}

bool USGTestParameterCase::IsEqualTMap(const TMap<int, int>& InMap)
{
	if (TMapValue.Num() != InMap.Num())
	{
		return false;
	}

	for (const auto& Elem : TMapValue)
	{
		if (const auto& Value = InMap.Find(Elem.Key))
		{
			if (Elem.Value != *Value)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool USGTestParameterCase::IsEqualTSet(const TSet<int>& InSet)
{
	if (TSetValue.Num() != InSet.Num())
	{
		return false;
	}

	for (const auto& Elem : TSetValue)
	{
		if (!InSet.Contains(Elem))
		{
			return false;
		}
	}

	return true;
}

bool USGTestParameterCase::IsEqualTSoftObjectPtr(const TSoftObjectPtr<USGTestParameterCase>& InTSoftObjectPtr)
{
	return TSoftObjectPtrValue == InTSoftObjectPtr;
}

bool USGTestParameterCase::IsEqualTSoftClassPtr(const TSoftClassPtr<USGTestParameterCase>& InTSoftClassPtr)
{
	return TSoftClassPtrValue == InTSoftClassPtr;
}

bool USGTestParameterCase::IsEqualTScriptInterface(const TScriptInterface<ISGTestInterfaceCase>& InTScriptInterface)
{
	return TScriptInterfaceValue == InTScriptInterface;
}
