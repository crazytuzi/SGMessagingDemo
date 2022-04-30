#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessage.h"
#include "SGAnyProperty.h"

class FSGMessage final
	: public ISGMessage
{
public:
	FSGMessage()
	{
	}

	template <typename ...Args>
	explicit FSGMessage(Args&&... InParams)
	{
		Add(Forward<Args>(InParams)...);
	}

	virtual ~FSGMessage() override
	{
		Params.Empty();
	}

public:
	virtual FName GetFName() const override
	{
		return "FSGMessage";
	}

public:
	template <typename T>
	T Get(const FString& Key) const
	{
		return TSGAnyProperty<T>(Params, Key)();
	}

	void Get(const FString& Key, const FArrayProperty* ArrayProperty, const void* PropertyAddress) const
	{
		TSGAnyProperty<FScriptArrayHelper>(Params, Key)(PropertyAddress, ArrayProperty);
	}

	void Get(const FString& Key, const FMapProperty* MapProperty, const void* PropertyAddress) const
	{
		TSGAnyProperty<FScriptMapHelper>(Params, Key)(PropertyAddress, MapProperty);
	}

	void Get(const FString& Key, const FSetProperty* SetProperty, const void* PropertyAddress) const
	{
		TSGAnyProperty<FScriptSetHelper>(Params, Key)(PropertyAddress, SetProperty);
	}

	void Get(const FString& Key, const FStructProperty* StructProperty, void* PropertyAddress) const
	{
		TSGAnyProperty<void*>(Params, Key)(PropertyAddress, StructProperty);
	}

	template <typename T>
	void Set(const FString& Key, T&& Value)
	{
		TSGAnyProperty<typename TRemoveReference<decltype(Value)>::Type>(Params, Key)(Value);
	}

	void Set(const FString& Key, const FEnumProperty* EnumProperty, const void* PropertyAddress)
	{
		TSGAnyProperty<int64>(Params, Key)(EnumProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FArrayProperty* ArrayProperty, const void* PropertyAddress)
	{
		TSGAnyProperty<FScriptArrayHelper>(Params, Key)(ArrayProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FMapProperty* MapProperty, const void* PropertyAddress)
	{
		TSGAnyProperty<FScriptMapHelper>(Params, Key)(MapProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FSetProperty* SetProperty, const void* PropertyAddress)
	{
		TSGAnyProperty<FScriptSetHelper>(Params, Key)(SetProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FStructProperty* StructProperty, const void* PropertyAddress)
	{
		TSGAnyProperty<void*>(Params, Key)(StructProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FMulticastInlineDelegateProperty* MulticastInlineDelegateProperty,
	         const void* PropertyAddress)
	{
		TSGAnyProperty<FMulticastScriptDelegate*>(Params, Key)(MulticastInlineDelegateProperty, PropertyAddress);
	}

	void Set(const FString& Key, const FMulticastSparseDelegateProperty* MulticastSparseDelegateProperty,
	         const void* PropertyAddress)
	{
		TSGAnyProperty<FSparseDelegate*>(Params, Key)(MulticastSparseDelegateProperty, PropertyAddress);
	}

private:
	template <typename T>
	void AddImplementation(const FString& Key, T&& Value)
	{
		Set(Key, Forward<T>(Value));
	}

	template <typename NameType, typename ParamType>
	void Add(const NameType& Key, ParamType&& Value)
	{
		AddImplementation(Key, Forward<ParamType>(Value));
	}

	template <typename NameType, typename ParamType, typename... Args>
	void Add(const NameType& Key, ParamType&& Value, Args&&... InParams)
	{
		AddImplementation(Key, Forward<ParamType>(Value));

		Add(InParams...);
	}

private:
	TMap<FString, FSGAny> Params;
};
