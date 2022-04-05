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

#define SET_MESSAGE_FIELD( PropertyName, ValueType ) \
	COMPLETE_SET_MESSAGE_FIELD( PropertyName, F##PropertyName, ValueType )

#define ELSE_SET_MESSAGE_FIELD( PropertyName, ValueType ) \
	else SET_MESSAGE_FIELD( PropertyName, ValueType )

#define COMPLETE_ELSE_SET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	else COMPLETE_SET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType )

#define COMPLETE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	if(const auto PropertyName = CastField<PropertyType>(InProperty)) \
	{ \
		const auto Value = Message.Message->Get<ValueType>(UKismetStringLibrary::Conv_NameToString(Key)); \
		PropertyName->CopySingleValue(PropertyAddress,&Value); \
	}

#define GET_MESSAGE_FIELD( PropertyName, ValueType ) \
	COMPLETE_GET_MESSAGE_FIELD( PropertyName, F##PropertyName, ValueType )

#define ELSE_GET_MESSAGE_FIELD( PropertyName, ValueType ) \
	else GET_MESSAGE_FIELD( PropertyName, ValueType )

#define COMPLETE_ELSE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType ) \
	else COMPLETE_GET_MESSAGE_FIELD( PropertyName, PropertyType, ValueType )

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
		COMPLETE_ELSE_SET_MESSAGE_FIELD(UInt16PropertyPointer, FUInt16Property, uint16)
		ELSE_SET_MESSAGE_FIELD(UInt32Property, uint32)
		COMPLETE_ELSE_SET_MESSAGE_FIELD(UInt64PropertyPointer, FUInt64Property, uint64)
		ELSE_SET_MESSAGE_FIELD(FloatProperty, float)
		ELSE_SET_MESSAGE_FIELD(DoubleProperty, double)
		else if (const auto EnumProperty = CastField<FEnumProperty>(InProperty))
		{
			auto Value = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		ELSE_SET_MESSAGE_FIELD(BoolProperty, bool)
		ELSE_SET_MESSAGE_FIELD(ObjectProperty, UObject*)
		ELSE_SET_MESSAGE_FIELD(WeakObjectProperty, TWeakObjectPtr<UObject>)
		ELSE_SET_MESSAGE_FIELD(LazyObjectProperty, TLazyObjectPtr<UObject>)
		ELSE_SET_MESSAGE_FIELD(SoftObjectProperty, TSoftObjectPtr<UObject>)
		ELSE_SET_MESSAGE_FIELD(InterfaceProperty, UInterface*)
		ELSE_SET_MESSAGE_FIELD(NameProperty, FName)
		ELSE_SET_MESSAGE_FIELD(StrProperty, FString)
		ELSE_SET_MESSAGE_FIELD(TextProperty, FText)
		else if (const auto ArrayProperty = CastField<FArrayProperty>(InProperty))
		{
			auto Value = FScriptArrayHelper(ArrayProperty, PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		else if (const auto MapProperty = CastField<FMapProperty>(InProperty))
		{
			auto Value = FScriptMapHelper(MapProperty, PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		else if (const auto SetProperty = CastField<FSetProperty>(InProperty))
		{
			auto Value = FScriptSetHelper(SetProperty, PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		else if (const auto StructProperty = CastField<FStructProperty>(InProperty))
		{
			auto Struct = StructProperty->Struct;

			auto Size = Struct->GetStructureSize() ? Struct->GetStructureSize() : 1;

			auto Value = FMemory::Malloc(Size);

			Struct->InitializeStruct(Value);

			Struct->CopyScriptStruct(Value, PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		else if (const auto MulticastInlineDelegateProperty = CastField<FMulticastInlineDelegateProperty>(InProperty))
		{
			auto Value = MulticastInlineDelegateProperty->GetPropertyValuePtr(PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
		else if (const auto MulticastSparseDelegateProperty = CastField<FMulticastSparseDelegateProperty>(InProperty))
		{
			auto Value = MulticastSparseDelegateProperty->GetPropertyValuePtr(PropertyAddress);

			Message.Message->Set(UKismetStringLibrary::Conv_NameToString(Key), Value);
		}
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
		COMPLETE_ELSE_GET_MESSAGE_FIELD(UInt16PropertyPointer, FUInt16Property, uint16)
		ELSE_GET_MESSAGE_FIELD(UInt32Property, uint32)
		COMPLETE_ELSE_GET_MESSAGE_FIELD(UInt64PropertyPointer, FUInt64Property, uint64)
		ELSE_GET_MESSAGE_FIELD(FloatProperty, float)
		ELSE_GET_MESSAGE_FIELD(DoubleProperty, double)
		ELSE_GET_MESSAGE_FIELD(EnumProperty, int64)
		ELSE_GET_MESSAGE_FIELD(BoolProperty, bool)
		ELSE_GET_MESSAGE_FIELD(ObjectProperty, UObject*)
		ELSE_GET_MESSAGE_FIELD(WeakObjectProperty, TWeakObjectPtr<UObject>)
		ELSE_GET_MESSAGE_FIELD(LazyObjectProperty, TLazyObjectPtr<UObject>)
		ELSE_GET_MESSAGE_FIELD(SoftObjectProperty, TSoftObjectPtr<UObject>)
		ELSE_GET_MESSAGE_FIELD(InterfaceProperty, UInterface*)
		ELSE_GET_MESSAGE_FIELD(NameProperty, FName)
		ELSE_GET_MESSAGE_FIELD(StrProperty, FString)
		ELSE_GET_MESSAGE_FIELD(TextProperty, FText)
		else if (const auto ArrayProperty = CastField<FArrayProperty>(InProperty))
		{
			auto SrcHelper = Message.Message->Get<FScriptArrayHelper>(UKismetStringLibrary::Conv_NameToString(Key));

			auto Inner = ArrayProperty->Inner;

			auto DestHelper = FScriptArrayHelper::CreateHelperFormInnerProperty(Inner, PropertyAddress);

			DestHelper.Resize(SrcHelper.Num());

			auto Dest = DestHelper.GetRawPtr();

			auto Src = SrcHelper.GetRawPtr();

			for (auto i = 0; i < SrcHelper.Num(); ++i)
			{
				Inner->CopySingleValue(Dest, Src);

				Dest += Inner->ElementSize;

				Src += Inner->ElementSize;
			}
		}
		else if (const auto MapProperty = CastField<FMapProperty>(InProperty))
		{
			auto SrcHelper = Message.Message->Get<FScriptMapHelper>(UKismetStringLibrary::Conv_NameToString(Key));

			auto DestHelper = FScriptMapHelper::CreateHelperFormInnerProperties(
				MapProperty->KeyProp, MapProperty->ValueProp, PropertyAddress);

			for (auto i = 0; i < SrcHelper.GetMaxIndex(); ++i)
			{
				if (SrcHelper.IsValidIndex(i))
				{
					DestHelper.AddPair(SrcHelper.GetKeyPtr(i), SrcHelper.GetValuePtr(i));
				}
			}
		}
		else if (const auto SetProperty = CastField<FSetProperty>(InProperty))
		{
			auto SrcHelper = Message.Message->Get<FScriptSetHelper>(UKismetStringLibrary::Conv_NameToString(Key));

			auto DestHelper = FScriptSetHelper::CreateHelperFormElementProperty(
				SetProperty->ElementProp, PropertyAddress);

			DestHelper.EmptyElements(SrcHelper.Num());

			for (auto i = 0; i < SrcHelper.GetMaxIndex(); ++i)
			{
				if (SrcHelper.IsValidIndex(i))
				{
					DestHelper.AddElement(SrcHelper.GetElementPtr(i));
				}
			}
		}
		else if (const auto StructProperty = CastField<FStructProperty>(InProperty))
		{
			auto Value = Message.Message->Get<void*>(UKismetStringLibrary::Conv_NameToString(Key));

			auto Struct = StructProperty->Struct;

			Struct->CopyScriptStruct(PropertyAddress, Value);
		}
		ELSE_GET_MESSAGE_FIELD(MulticastInlineDelegateProperty, FMulticastScriptDelegate*)
		ELSE_GET_MESSAGE_FIELD(MulticastSparseDelegateProperty, FSparseDelegate*)
	}

	return Message;
}
