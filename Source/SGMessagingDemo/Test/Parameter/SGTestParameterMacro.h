#pragma once

#include "CoreMinimal.h"

#define ADD_CASE( TypePrefix ) Case->TypePrefix##Key, Case->TypePrefix##Value

#define LOG_VALIDATE_CASE( Case, Result ) UE_LOG(LogTemp, Log, TEXT("Validate %s %s"), Case, Result);

#define LOG_CONDITION_VALIDATE_CASE( TypePrefix, Condition ) LOG_VALIDATE_CASE( *Case->TypePrefix##Key, *UKismetStringLibrary::Conv_BoolToString(Condition) )

template <typename T>
struct TSGArgumentType;

template <typename T, typename U>
struct TSGArgumentType<T(U)>
{
	using ArgumentType = U;
};

#define TYPE_STRING(String) #String

#define LOG_TYPE_VALIDATE_CASE( TypePrefix, Type ) LOG_CONDITION_VALIDATE_CASE( TypePrefix, Case->TypePrefix##Value == Message.Get<TSGArgumentType<void(Type)>::ArgumentType>(Case->TypePrefix##Key) )
