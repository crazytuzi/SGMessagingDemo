#pragma once

#include "CoreMinimal.h"
#include "Core/Message/SGMessage.h"
#include "SGBlueprintMessage.generated.h"

USTRUCT(BlueprintType)
struct FSGBlueprintMessage
{
	GENERATED_BODY()

	FSGBlueprintMessage() = default;

	FSGBlueprintMessage(const void* InMessage)
	{
		if (InMessage == nullptr)
		{
			Message = new FSGMessage();
		}
		else
		{
			Message = static_cast<FSGMessage*>(const_cast<void*>(InMessage));
		}
	}

	FSGBlueprintMessage(const FSGMessage& InMessage): Message(&const_cast<FSGMessage&>(InMessage))
	{
	}

public:
	FSGMessage* GetMessage() const
	{
		return Message;
	}

	operator FSGMessage*() const
	{
		return Message;
	}

private:
	FSGMessage* Message;
};
