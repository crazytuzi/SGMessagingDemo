#pragma once

#include "CoreMinimal.h"
#include "SGAnyType.h"

struct FSGAny
{
	FSGAny()
		: ScriptArray(nullptr)
	{
	}

	FSGAny(const FSGAny& That)
		: ScriptArray(That.ScriptArray),
		  Pointer(That.Clone()),
		  AnyType(That.AnyType)
	{
	}

	FSGAny(FSGAny&& That) noexcept
		: ScriptArray(MoveTemp(That.ScriptArray)),
		  Pointer(MoveTemp(That.Pointer)),
		  AnyType(MoveTemp(That.AnyType))
	{
	}

	template <typename T, class = TEnableIf<TNot<TIsSame<TDecay<T>, FSGAny>>::Value, T>>
	explicit FSGAny(T&& Value)
		: ScriptArray(nullptr),
		  Pointer(new TDerived<typename TDecay<T>::Type>(Forward<T>(Value))),
		  AnyType(TSGAnyTraits<typename TRemoveReference<decltype(Value)>::Type>::GetType())
	{
	}

	bool IsValid() const
	{
		return Pointer.IsValid();
	}

	FSGAnyType GetType() const
	{
		return AnyType;
	}

	template <class T>
	bool IsA() const
	{
		return AnyType == TSGAnyTraits<T>::GetType();
	}

	template <class T>
	T& Cast() const
	{
		auto Derived = static_cast<TDerived<T>*>(Pointer.Get());

		return Derived->Value;
	}

	FSGAny& operator=(const FSGAny& Other)
	{
		if (Pointer == Other.Pointer)
		{
			return *this;
		}

		Pointer = Other.Clone();

		AnyType = Other.AnyType;

		return *this;
	}

private:
	struct FBase;

	using FBasePtr = TUniquePtr<FBase>;

	struct FBase
	{
		virtual ~FBase()
		{
		}

		virtual FBasePtr Clone() const = 0;
	};

	template <typename T>
	struct TDerived final : FBase
	{
		template <typename U>
		explicit TDerived(U&& InValue)
			: Value(Forward<U>(InValue))
		{
		}

		virtual FBasePtr Clone() const override
		{
			return MakeUnique<TDerived<T>>(Value);
		}

		T Value;
	};

private:
	FBasePtr Clone() const
	{
		return Pointer != nullptr ? Pointer->Clone() : nullptr;
	}

public:
	union
	{
		FScriptArray* ScriptArray;

		FScriptMap* ScriptMap;

		FScriptSet* ScriptSet;

		void* ScriptStruct;
	};

private:
	FBasePtr Pointer;

	FSGAnyType AnyType;
};
