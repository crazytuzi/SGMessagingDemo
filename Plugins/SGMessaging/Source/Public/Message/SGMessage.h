#pragma once

#include "CoreMinimal.h"
#include "Interface/ISGMessage.h"
#include "SGAny.h"

class FSGMessage
	: public ISGMessage
{
public:
	FSGMessage()
	{
	}

	virtual ~FSGMessage() override
	{
	}

public:
	virtual FName GetFName() const override
	{
		return "FSGMessage";
	}

public:
	template <typename T>
	void Add(const FString& Key, T&& Value)
	{
		Params.Add(Key, FSGAny(Forward<T>(Value)));
	}

	template <typename T>
	T Get(const FString& Key) const
	{
		if (const auto& Value = Params.Find(Key))
		{
			return Value->Cast<T>();
		}

		return T();
	}

private:
	TMap<FString, FSGAny> Params;
};
