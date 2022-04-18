// Fill out your copyright notice in the Description page of Project Settings.


#include "MessagingFramework/Kismet/SGMessageFunctionLibrary.h"
#include "Blueprint/Common/SGBlueprintMessageEndpointBuilder.h"
#include "MessagingFramework/Subsystems/SGMessageWorldSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#define COMPLETE_SET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	if(const auto PropertyName = CastField<PropertyType>(InProperty)) \
	{ \
		ValueType Value; \
		PropertyName->CopySingleValue(&Value,PropertyAddress); \
		Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key),Value); \
	}

#define ELSE_COMPLETE_SET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	else COMPLETE_SET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType )

#define SET_MESSAGE_FIELD( PropertyName, ValueType ) \
	COMPLETE_SET_MESSAGE_FIELD( PropertyName, F##PropertyName, ValueType )

#define ELSE_SET_MESSAGE_FIELD( PropertyName, ValueType ) \
	else SET_MESSAGE_FIELD( PropertyName, ValueType )

#define CUSTOMIZE_SET_MESSAGE_FIELD( PropertyName ) \
	if (const auto PropertyName = CastField<F##PropertyName>(InProperty)) \
	{ \
		Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), PropertyName, PropertyAddress); \
	}

#define ELSE_CUSTOMIZE_SET_MESSAGE_FIELD( PropertyName ) \
	else CUSTOMIZE_SET_MESSAGE_FIELD( PropertyName )

#define COMPLETE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	if(const auto PropertyName = CastField<PropertyType>(InProperty)) \
	{ \
		const auto Value = Message.Message->Get<ValueType>(UKismetStringLibrary::Conv_NameToString(Key)); \
		PropertyName->CopySingleValue(PropertyAddress,&Value); \
	}

#define ELSE_COMPLETE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	else COMPLETE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType )

#define GET_MESSAGE_FIELD( PropertyName, ValueType ) \
	COMPLETE_GET_MESSAGE_FIELD( PropertyName, F##PropertyName, ValueType )

#define ELSE_GET_MESSAGE_FIELD( PropertyName, ValueType ) \
	else GET_MESSAGE_FIELD( PropertyName, ValueType )

#define CUSTOMIZE_GET_MESSAGE_FIELD( PropertyName ) \
	if (const auto PropertyName = CastField<F##PropertyName>(InProperty)) \
	{ \
		Message.Message->Get(UKismetStringLibrary::Conv_NameToString(Key), PropertyName, PropertyAddress); \
	}

#define ELSE_CUSTOMIZE_GET_MESSAGE_FIELD( PropertyName ) \
	else CUSTOMIZE_GET_MESSAGE_FIELD( PropertyName )

USGBlueprintMessageBus* USGMessageFunctionLibrary::GetDefaultBus(UObject* WorldContextObject)
{
	if (const auto MessageWorldSubsystem = Cast<USGMessageWorldSubsystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(WorldContextObject, USGMessageWorldSubsystem::StaticClass())))
	{
		return MessageWorldSubsystem->GetDefaultBus();
	}

	return nullptr;
}

USGBlueprintMessageEndpoint* USGMessageFunctionLibrary::GetDefaultMessageEndpoint(UObject* WorldContextObject)
{
	if (const auto MessageWorldSubsystem = Cast<USGMessageWorldSubsystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(WorldContextObject, USGMessageWorldSubsystem::StaticClass())))
	{
		return MessageWorldSubsystem->GetDefaultMessageEndpoint();
	}

	return nullptr;
}

FSGBlueprintMessage USGMessageFunctionLibrary::BuildBlueprintMessage()
{
	return FSGBlueprintMessage(nullptr);
}

FSGBlueprintMessageAddress USGMessageFunctionLibrary::GetSender(const FSGBlueprintMessageContext& InContent)
{
	return InContent.GetSender();
}

FSGBlueprintMessage USGMessageFunctionLibrary::ExecSet(const FSGBlueprintMessage& Message, const FName& Key,
                                                       FProperty* InProperty, const void* PropertyAddress)
{
	if (Message.Message != nullptr && InProperty != nullptr)
	{
		SET_MESSAGE_FIELD(ByteProperty, uint8)
		ELSE_SET_MESSAGE_FIELD(Int8Property, int8)
		ELSE_SET_MESSAGE_FIELD(Int16Property, int16)
		ELSE_SET_MESSAGE_FIELD(IntProperty, int32)
		ELSE_SET_MESSAGE_FIELD(Int64Property, int64)
		ELSE_COMPLETE_SET_MESSAGE_FIELD(UInt16PropertyPointer, FUInt16Property, uint16)
		ELSE_SET_MESSAGE_FIELD(UInt32Property, uint32)
		ELSE_COMPLETE_SET_MESSAGE_FIELD(UInt64PropertyPointer, FUInt64Property, uint64)
		ELSE_SET_MESSAGE_FIELD(FloatProperty, float)
		ELSE_SET_MESSAGE_FIELD(DoubleProperty, double)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(EnumProperty)
		ELSE_SET_MESSAGE_FIELD(BoolProperty, bool)
		ELSE_SET_MESSAGE_FIELD(ClassProperty, UClass*)
		ELSE_SET_MESSAGE_FIELD(ObjectProperty, UObject*)
		ELSE_SET_MESSAGE_FIELD(SoftClassProperty, TSoftClassPtr<UObject>)
		ELSE_SET_MESSAGE_FIELD(SoftObjectProperty, TSoftObjectPtr<UObject>)
		ELSE_SET_MESSAGE_FIELD(InterfaceProperty, TScriptInterface<IInterface>)
		ELSE_SET_MESSAGE_FIELD(NameProperty, FName)
		ELSE_SET_MESSAGE_FIELD(StrProperty, FString)
		ELSE_SET_MESSAGE_FIELD(TextProperty, FText)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(ArrayProperty)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(MapProperty)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(SetProperty)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(StructProperty)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(MulticastInlineDelegateProperty)
		ELSE_CUSTOMIZE_SET_MESSAGE_FIELD(MulticastSparseDelegateProperty)
	}

	return Message;
}

FSGBlueprintMessage USGMessageFunctionLibrary::ExecGet(const FSGBlueprintMessage& Message, const FName& Key,
                                                       FProperty* InProperty,
                                                       void* PropertyAddress)
{
	if (Message.Message != nullptr && InProperty != nullptr)
	{
		GET_MESSAGE_FIELD(ByteProperty, uint8)
		ELSE_GET_MESSAGE_FIELD(Int8Property, int8)
		ELSE_GET_MESSAGE_FIELD(Int16Property, int16)
		ELSE_GET_MESSAGE_FIELD(IntProperty, int32)
		ELSE_GET_MESSAGE_FIELD(Int64Property, int64)
		ELSE_COMPLETE_GET_MESSAGE_FIELD(UInt16PropertyPointer, FUInt16Property, uint16)
		ELSE_GET_MESSAGE_FIELD(UInt32Property, uint32)
		ELSE_COMPLETE_GET_MESSAGE_FIELD(UInt64PropertyPointer, FUInt64Property, uint64)
		ELSE_GET_MESSAGE_FIELD(FloatProperty, float)
		ELSE_GET_MESSAGE_FIELD(DoubleProperty, double)
		ELSE_GET_MESSAGE_FIELD(EnumProperty, int64)
		ELSE_GET_MESSAGE_FIELD(BoolProperty, bool)
		ELSE_GET_MESSAGE_FIELD(ClassProperty, UClass*)
		ELSE_GET_MESSAGE_FIELD(ObjectProperty, UObject*)
		ELSE_GET_MESSAGE_FIELD(SoftClassProperty, TSoftClassPtr<UObject>)
		ELSE_GET_MESSAGE_FIELD(SoftObjectProperty, TSoftObjectPtr<UObject>)
		ELSE_GET_MESSAGE_FIELD(InterfaceProperty, TScriptInterface<IInterface>)
		ELSE_GET_MESSAGE_FIELD(NameProperty, FName)
		ELSE_GET_MESSAGE_FIELD(StrProperty, FString)
		ELSE_GET_MESSAGE_FIELD(TextProperty, FText)
		ELSE_CUSTOMIZE_GET_MESSAGE_FIELD(ArrayProperty)
		ELSE_CUSTOMIZE_GET_MESSAGE_FIELD(MapProperty)
		ELSE_CUSTOMIZE_GET_MESSAGE_FIELD(SetProperty)
		ELSE_CUSTOMIZE_GET_MESSAGE_FIELD(StructProperty)
		ELSE_GET_MESSAGE_FIELD(MulticastInlineDelegateProperty, FMulticastScriptDelegate*)
		ELSE_GET_MESSAGE_FIELD(MulticastSparseDelegateProperty, FSparseDelegate*)
	}

	return Message;
}
