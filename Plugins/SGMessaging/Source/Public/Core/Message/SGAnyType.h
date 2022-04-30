#pragma once

#include "SGAnyTypeTemplate.h"

/**
 * Enumerates the built-in types that can be stored in instances of ESGAnyTypes.
 */
enum class ESGAnyTypes : int32
{
	Empty,
	Int8,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Float,
	Double,
	Enum,
	EnumClass,
	TEnumAsByte,
	TSubclassOf,
	Bool,
	UObject,
	TObjectPtr,
	TWeakObject,
	TLazyObject,
	TSoftObject,
	TSoftClass,
	TScriptInterface,
	FName,
	FString,
	FText,
	TArray,
	FScriptArray,
	TMap,
	FScriptMap,
	TSet,
	FScriptSet,
	UStruct,
	UStructPtr,
	FMulticastInlineDelegate,
	FMulticastSparseDelegate,
	Char,
	Ansichar,
	Class
};


/**
 * Implements an extensible union of multiple types.
 *
 * Variant types can be used to store a range of different built-in types, as well as user
 * defined types. The values are internally serialized into a byte array, which means that
 * only FArchive serializable types are supported at this time.
 */
class FSGAnyType
{
public:
	/** Default constructor. */
	FSGAnyType()
		: Type(ESGAnyTypes::Empty)
	{
	}

	FSGAnyType(const ESGAnyTypes InType)
		: Type(InType)
	{
	}

	FSGAnyType(const FSGAnyType&) = default;

	FSGAnyType& operator=(const FSGAnyType&) = default;

	~FSGAnyType() = default;

	FSGAnyType(FSGAnyType&& Other) noexcept
		: Type(Other.Type)
	{
		Other.Type = ESGAnyTypes::Empty;
	}

	FSGAnyType& operator=(FSGAnyType&& Other) noexcept
	{
		if (&Other != this)
		{
			Type = Other.Type;

			Other.Type = ESGAnyTypes::Empty;
		}

		return *this;
	}

public:
	/**
	 * Comparison operator for equality.
	 *
	 * @param Other The variant to compare with.
	 * @return true if the values are equal, false otherwise.
	 */
	bool operator==(const FSGAnyType& Other) const
	{
		return Type == Other.Type;
	}

	/**
	 * Comparison operator for inequality.
	 *
	 * @param Other The variant to compare with.
	 * @return true if the values are not equal, false otherwise.
	 */
	bool operator!=(const FSGAnyType& Other) const
	{
		return Type != Other.Type;
	}

private:
	/** Holds the type of the variant. */
	ESGAnyTypes Type;
};


/**
 * Stub for variant type traits.
 *
 * Actual type traits need to be declared through template specialization for custom
 * data types that are to be used in FSGAnyType. Traits for the most commonly used built-in
 * types are declared below.
 *
 * Complex types, such as structures and classes can be serialized into a byte array
 * and then assigned to a variant. Note that you will be responsible for ensuring
 * correct byte ordering when serializing those types.
 *
 * @param T The type to be used in FSGAnyType.
 */
template <typename T, typename Enable = void>
struct TSGAnyTraits
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Empty; }
};


/* Default FSGAnyType traits for built-in types
 *****************************************************************************/

template <typename T>
struct TSGAnyTraits<int8, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Int8; }
};

template <typename T>
struct TSGAnyTraits<uint8, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UInt8; }
};

template <typename T>
struct TSGAnyTraits<int16, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Int16; }
};

template <typename T>
struct TSGAnyTraits<uint16, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UInt16; }
};

template <typename T>
struct TSGAnyTraits<int32, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Int32; }
};

template <typename T>
struct TSGAnyTraits<uint32, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UInt32; }
};

template <typename T>
struct TSGAnyTraits<int64, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Int64; }
};

template <typename T>
struct TSGAnyTraits<uint64, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UInt64; }
};

template <typename T>
struct TSGAnyTraits<float, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Float; }
};

template <typename T>
struct TSGAnyTraits<double, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Double; }
};

template <typename T>
struct TSGAnyTraits<T, typename TEnableIf<TSGIsEnum<T>::Value>::type>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Enum; }
};

template <typename T>
struct TSGAnyTraits<T, typename TEnableIf<TSGIsEnumClass<T>::Value>::type>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::EnumClass; }
};

template <typename T>
struct TSGAnyTraits<TEnumAsByte<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TEnumAsByte; }
};

template <typename T>
struct TSGAnyTraits<TSubclassOf<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TSubclassOf; }
};

template <typename T>
struct TSGAnyTraits<bool, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Bool; }
};

template <typename T>
struct TSGAnyTraits<T*, typename TEnableIf<TSGIsUObject<T>::Value>::Type>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UObject; }
};

template <typename T>
struct TSGAnyTraits<TObjectPtr<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TObjectPtr; }
};

template <typename T>
struct TSGAnyTraits<TWeakObjectPtr<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TWeakObject; }
};

template <typename T>
struct TSGAnyTraits<TLazyObjectPtr<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TLazyObject; }
};

template <typename T>
struct TSGAnyTraits<TSoftObjectPtr<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TSoftObject; }
};

template <typename T>
struct TSGAnyTraits<TSoftClassPtr<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TSoftClass; }
};

template <typename T>
struct TSGAnyTraits<TScriptInterface<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TScriptInterface; }
};

template <typename T>
struct TSGAnyTraits<FName, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FName; }
};

template <typename T>
struct TSGAnyTraits<FString, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FString; }
};

template <typename T>
struct TSGAnyTraits<FText, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FText; }
};

template <typename T>
struct TSGAnyTraits<TArray<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TArray; }
};

template <typename T>
struct TSGAnyTraits<FScriptArrayHelper, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FScriptArray; }
};

template <typename K, typename V>
struct TSGAnyTraits<TMap<K, V>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TMap; }
};

template <typename T>
struct TSGAnyTraits<FScriptMapHelper, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FScriptMap; }
};

template <typename T>
struct TSGAnyTraits<TSet<T>>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::TSet; }
};

template <typename T>
struct TSGAnyTraits<FScriptSetHelper, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FScriptSet; }
};

template <typename T>
struct TSGAnyTraits<T, typename TEnableIf<TSGIsUStruct<T>::Value>::Type>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UStruct; }
};

template <typename T>
struct TSGAnyTraits<T*, typename TEnableIf<TSGIsUStructPtr<T>::Value>::Type>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::UStructPtr; }
};

template <typename T>
struct TSGAnyTraits<FMulticastScriptDelegate*, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FMulticastInlineDelegate; }
};

template <typename T>
struct TSGAnyTraits<FSparseDelegate*, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::FMulticastSparseDelegate; }
};

template <typename T>
struct TSGAnyTraits<const char*, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Char; }
};

template <typename T>
struct TSGAnyTraits<ANSICHAR*, T>
{
	static CONSTEXPR ESGAnyTypes GetType() { return ESGAnyTypes::Ansichar; }
};
