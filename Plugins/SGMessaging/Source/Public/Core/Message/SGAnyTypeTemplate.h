#pragma once

#include "CoreMinimal.h"

template <typename T>
struct TSGIsEnum
{
	enum { Value = TAndValue<TIsEnum<T>::Value, UE::Core::Private::IsEnumClass::TIsEnumConvertibleToInt<T>>::Value };
};

template <typename T>
struct TSGIsEnumClass
{
	enum { Value = TIsEnumClass<T>::Value };
};

template <typename T>
struct TSGIsUObject
{
	enum { Value = TPointerIsConvertibleFromTo<typename TDecay<T>::Type, UObjectBase>::Value };
};

template <typename T>
struct TSGIsUStruct
{
private:
	template <typename A>
	static uint8 Check(decltype(&A::StaticStruct));

	template <typename B>
	static uint16 Check(...);

public:
	enum { Value = sizeof(Check<T>(0)) == sizeof(uint8) };
};

template <typename T>
struct TSGIsUStructPtr
{
	enum
	{
		Value = TSGIsUStruct<T>::Value && TNot<TPointerIsConvertibleFromTo<
			typename TDecay<T>::Type, UObjectBase>>::Value
	};
};
