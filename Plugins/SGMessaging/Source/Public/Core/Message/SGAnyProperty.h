#pragma once

#include "CoreMinimal.h"
#include "SGAny.h"

struct FSGAnyProperty
{
	FSGAnyProperty(TMap<FString, FSGAny>& InParams, const FString& InKey)
		: Params(InParams), Key(InKey)
	{
	}

	FSGAnyProperty(const TMap<FString, FSGAny>& InParams, const FString& InKey)
		: Params(const_cast<TMap<FString, FSGAny>&>(InParams)), Key(InKey)
	{
	}

	TMap<FString, FSGAny>& Params;

	const FString& Key;
};

template <typename T, typename Enable = void>
struct TSGAnyProperty : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(T& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	T operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->IsA<T>());

			return Value->Cast<T>();
		}

		return T();
	}
};

template <typename T>
struct TSGAnyProperty<uint8, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(uint8& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	uint8 operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::UInt8 || Value->GetType() == ESGAnyTypes::TEnumAsByte);

			return Value->Cast<uint8>();
		}

		return uint8();
	}
};

template <typename T>
struct TSGAnyProperty<int64, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(int64& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FEnumProperty* EnumProperty, const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(PropertyAddress)));
	}

	int64 operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::Int64 || Value->GetType() == ESGAnyTypes::EnumClass);

			return Value->Cast<int64>();
		}

		return int64();
	}
};

template <typename T>
struct TSGAnyProperty<double, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(double& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	double operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::Double || Value->GetType() == ESGAnyTypes::Float);

			return Value->GetType() == ESGAnyTypes::Double ? Value->Cast<double>() : Value->Cast<float>();
		}

		return double();
	}
};

template <typename T>
struct TSGAnyProperty<T, typename TEnableIf<TSGIsEnumClass<T>::Value>::type> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(T& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	T operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::EnumClass || Value->GetType() == ESGAnyTypes::Int64);

			return Value->GetType() == ESGAnyTypes::EnumClass ? Value->Cast<T>() : static_cast<T>(Value->Cast<int64>());
		}

		return T();
	}
};

template <typename T>
struct TSGAnyProperty<TSubclassOf<T>> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(TSubclassOf<T>& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	TSubclassOf<T> operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::TSubclassOf || Value->GetType() == ESGAnyTypes::UObject);

			return Value->GetType() == ESGAnyTypes::TSubclassOf
				       ? Value->Cast<TSubclassOf<T>>()
				       : TSubclassOf<T>(Value->Cast<T*>()->StaticClass());
		}

		return TSubclassOf<T>();
	}
};

template <typename T>
struct TSGAnyProperty<T*, typename TEnableIf<TSGIsUObject<T>::Value>::Type> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(T* Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	T* operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(
				Value->GetType() == ESGAnyTypes::UObject || Value->GetType() == ESGAnyTypes::TSubclassOf || Value->
				GetType() == ESGAnyTypes::TObjectPtr);

			return Value->Cast<T*>();
		}

		return nullptr;
	}
};

template <typename T>
struct TSGAnyProperty<TObjectPtr<T>> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(TObjectPtr<T>& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	TObjectPtr<T> operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::TObjectPtr || Value->GetType() == ESGAnyTypes::UObject);

			return Value->GetType() == ESGAnyTypes::TObjectPtr
				       ? Value->Cast<TObjectPtr<T>>()
				       : TObjectPtr<T>(Value->Cast<T*>());
		}

		return TObjectPtr<T>();
	}
};

template <typename T>
struct TSGAnyProperty<TArray<T>> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(TArray<T>& Value) const
	{
		auto Any = FSGAny(Value);

		Any.ScriptArray = reinterpret_cast<FScriptArray*>(&Value);

		Params.Add(Key, MoveTemp(Any));
	}

	TArray<T> operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::TArray || Value->GetType() == ESGAnyTypes::FScriptArray);

			if (Value->GetType() == ESGAnyTypes::TArray)
			{
				return Value->Cast<TArray<T>>();
			}

			if (Value->GetType() == ESGAnyTypes::FScriptArray)
			{
				auto ScriptArrayHelper = Value->Cast<FScriptArrayHelper>();

				TArray<T> ScriptArray;

				for (auto i = 0; i < ScriptArrayHelper.Num(); ++i)
				{
					ScriptArray.Add(*(ScriptArrayHelper.GetRawPtr() + i * sizeof(T)));
				}

				return MoveTemp(ScriptArray);
			}
		}

		return TArray<T>();
	}
};

template <typename T>
struct TSGAnyProperty<FScriptArrayHelper, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(FScriptArrayHelper& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FArrayProperty* ArrayProperty, const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(FScriptArrayHelper(ArrayProperty, PropertyAddress)));
	}

	void operator()(const void* PropertyAddress, const FArrayProperty* ArrayProperty) const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::FScriptArray|| Value->GetType() == ESGAnyTypes::TArray);

			auto SrcHelper = Value->GetType() == ESGAnyTypes::FScriptArray
				                 ? Value->Cast<FScriptArrayHelper>()
				                 : FScriptArrayHelper::CreateHelperFormInnerProperty(
					                 ArrayProperty->Inner, Value->ScriptArray);

			const auto Inner = ArrayProperty->Inner;

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
	}
};

template <typename K, typename V>
struct TSGAnyProperty<TMap<K, V>> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(TMap<K, V>& Value) const
	{
		auto Any = FSGAny(Value);

		Any.ScriptMap = reinterpret_cast<FScriptMap*>(&Value);

		Params.Add(Key, MoveTemp(Any));
	}

	TMap<K, V> operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::TMap||Value->GetType() == ESGAnyTypes::FScriptMap);

			if (Value->GetType() == ESGAnyTypes::TMap)
			{
				return Value->Cast<TMap<K, V>>();
			}

			if (Value->GetType() == ESGAnyTypes::FScriptMap)
			{
				TMap<K, V> ScriptMap;

				auto ScriptMapHelper = Value->Cast<FScriptMapHelper>();

				for (auto i = 0; i < ScriptMapHelper.GetMaxIndex(); ++i)
				{
					if (ScriptMapHelper.IsValidIndex(i))
					{
						ScriptMap.Add(*ScriptMapHelper.GetKeyPtr(i), *ScriptMapHelper.GetValuePtr(i));
					}
				}

				return MoveTemp(ScriptMap);
			}

			return Value->Cast<TMap<K, V>>();
		}

		return TMap<K, V>();
	}
};

template <typename T>
struct TSGAnyProperty<FScriptMapHelper, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(FScriptArrayHelper& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FMapProperty* MapProperty, const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(FScriptMapHelper(MapProperty, PropertyAddress)));
	}

	void operator()(const void* PropertyAddress, const FMapProperty* MapProperty) const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::FScriptMap || Value->GetType() == ESGAnyTypes::TMap);

			auto SrcHelper = Value->GetType() == ESGAnyTypes::FScriptMap
				                 ? Value->Cast<FScriptMapHelper>()
				                 : FScriptMapHelper::CreateHelperFormInnerProperties(
					                 MapProperty->KeyProp, MapProperty->ValueProp, Value->ScriptMap);

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
	}
};

template <typename T>
struct TSGAnyProperty<TSet<T>> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(TSet<T>& Value) const
	{
		auto Any = FSGAny(Value);

		Any.ScriptSet = reinterpret_cast<FScriptSet*>(&Value);

		Params.Add(Key, MoveTemp(Any));
	}

	TSet<T> operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::TSet || Value->GetType() == ESGAnyTypes::FScriptSet);

			if (Value->GetType() == ESGAnyTypes::TSet)
			{
				return Value->Cast<TSet<T>>();
			}

			if (Value->GetType() == ESGAnyTypes::FScriptSet)
			{
				TSet<T> ScriptSet;

				auto ScriptSetHelper = Value->Cast<FScriptSetHelper>();

				for (auto i = 0; i < ScriptSetHelper.GetMaxIndex(); ++i)
				{
					if (ScriptSetHelper.IsValidIndex(i))
					{
						ScriptSet.Add(*ScriptSetHelper.GetElementPtr(i));
					}
				}

				return MoveTemp(ScriptSet);
			}
		}

		return TSet<T>();
	}
};

template <typename T>
struct TSGAnyProperty<FScriptSetHelper, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(FScriptSetHelper& Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FSetProperty* SetProperty, const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(FScriptSetHelper(SetProperty, PropertyAddress)));
	}

	void operator()(const void* PropertyAddress, const FSetProperty* SetProperty) const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::FScriptSet || Value->GetType() == ESGAnyTypes::TSet);

			auto SrcHelper = Value->GetType() == ESGAnyTypes::FScriptSet
				                 ? Value->Cast<FScriptSetHelper>()
				                 : FScriptSetHelper::CreateHelperFormElementProperty(
					                 SetProperty->ElementProp, Value->ScriptSet);

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
	}
};

template <typename T>
struct TSGAnyProperty<T, typename TEnableIf<TSGIsUStruct<T>::Value>::Type> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(T& Value) const
	{
		auto Any = FSGAny(Value);

		Any.ScriptStruct = &Value;

		Params.Add(Key, MoveTemp(Any));
	}

	T operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::UStruct || Value->GetType() == ESGAnyTypes::Empty);

			return Value->GetType() == ESGAnyTypes::UStruct ? Value->Cast<T>() : MoveTemp(*new(Value->Cast<void*>())T);
		}

		return T();
	}
};

template <typename T>
struct TSGAnyProperty<void*, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(void* Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FStructProperty* StructProperty, const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(PropertyAddress));
	}

	void* operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->IsA<void*>());

			return Value->Cast<void*>();
		}

		return nullptr;
	}

	void operator()(void* PropertyAddress, const FStructProperty* StructProperty) const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::Empty || Value->GetType() == ESGAnyTypes::UStruct);

			const auto Struct = StructProperty->Struct;

			Struct->InitializeStruct(PropertyAddress);

			Struct->CopyScriptStruct(PropertyAddress,
			                         Value->GetType() == ESGAnyTypes::Empty
				                         ? Value->Cast<void*>()
				                         : Value->ScriptStruct);
		}
	}
};

template <typename T>
struct TSGAnyProperty<FMulticastScriptDelegate*, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(FMulticastScriptDelegate* Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FMulticastInlineDelegateProperty* MulticastInlineDelegateProperty,
	                const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(MulticastInlineDelegateProperty->GetPropertyValuePtr(PropertyAddress)));
	}

	FMulticastScriptDelegate* operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::FMulticastInlineDelegate);

			return Value->Cast<FMulticastScriptDelegate*>();
		}

		return nullptr;
	}
};

template <typename T>
struct TSGAnyProperty<FSparseDelegate*, T> : FSGAnyProperty
{
	using FSGAnyProperty::FSGAnyProperty;

	void operator()(FSparseDelegate* Value) const
	{
		Params.Add(Key, FSGAny(Value));
	}

	void operator()(const FMulticastSparseDelegateProperty* MulticastSparseDelegateProperty,
	                const void* PropertyAddress) const
	{
		Params.Add(Key, FSGAny(MulticastSparseDelegateProperty->GetPropertyValuePtr(PropertyAddress)));
	}

	FSparseDelegate* operator()() const
	{
		if (const auto Value = Params.Find(Key))
		{
			ensure(Value->GetType() == ESGAnyTypes::FMulticastSparseDelegate);

			return Value->Cast<FSparseDelegate*>();
		}

		return nullptr;
	}
};
