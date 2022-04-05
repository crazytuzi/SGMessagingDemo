#pragma once

#include "CoreMinimal.h"
#include "Core/Message/SGMessage.h"
#include "SGBlueprintMessage.generated.h"

USTRUCT(BlueprintType)
struct FSGBlueprintMessage
{
	GENERATED_BODY()

	FSGBlueprintMessage() = default;

	FSGBlueprintMessage(ISGMessage* InMessage)
	{
		if (InMessage == nullptr)
		{
			Message = new FSGMessage();
		}
		else
		{
			Message = static_cast<FSGMessage*>(InMessage);
		}
	}

	operator FSGMessage*() const
	{
		return Message;
	}

	FSGMessage* Message;
};
