#pragma once

#include "CoreMinimal.h"
#include "Core/Interface/ISGMessage.h"
#include "SGAny.h"

class FSGMessage
	: public ISGMessage
{
public:
	FSGMessage()
	{
	}

	template <typename ...Args>
	FSGMessage(Args&&... InParams)
	{
		Add(Forward<Args>(InParams)...);
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
	T Get(const FString& Key) const
	{
		return Params.Find(Key)->Cast<T>();
	}

	template <typename T>
	void Set(const FString& Key, T&& Value)
	{
		Params.Add(Key, FSGAny(Value));
	}

private:
	template <typename T>
	void AddImplementation(const FString& Key, T&& Value)
	{
		Params.Add(Key, FSGAny(Forward<T>(Value)));
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
