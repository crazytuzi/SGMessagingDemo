#pragma once

#include "CoreMinimal.h"

struct FSGAny
{
	FSGAny() : TypeHash(0)
	{
	}

	FSGAny(const FSGAny& That) : Pointer(That.Clone()), TypeHash(That.TypeHash)
	{
	}

	FSGAny(FSGAny&& That) noexcept : Pointer(MoveTemp(That.Pointer)), TypeHash(That.TypeHash)
	{
	}

	template <typename T, class = TEnableIf<!TIsSame<TDecay<T>, FSGAny>::Value, T>>
	explicit FSGAny(T&& Value) : Pointer(new TDerived<typename TDecay<T>::Type>(Forward<T>(Value)))
	                             , TypeHash(GetTypeHash(Value))
	{
	}

	bool IsValid() const
	{
		return Pointer.IsValid();
	}

	template <class T>
	bool IsA() const
	{
		static T Value;

		return TypeHash == GetTypeHash(Value);
	}

	template <class T>
	T& Cast() const
	{
		ensure(!IsA<T>());

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

		TypeHash = Other.TypeHash;

		return *this;
	}

private:
	struct FBase;

	typedef TUniquePtr<FBase> FBasePtr;

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
		explicit TDerived(U&& InValue) : Value(Forward<U>(InValue))
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

private:
	FBasePtr Pointer;

	uint32 TypeHash;
};
