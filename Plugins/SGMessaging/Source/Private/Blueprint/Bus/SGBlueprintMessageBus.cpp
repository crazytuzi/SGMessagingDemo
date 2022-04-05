#include "Blueprint/Bus/SGBlueprintMessageBus.h"
#include "Core/Interface/ISGMessagingModule.h"

USGBlueprintMessageBus::USGBlueprintMessageBus()
{
	if (const auto World = UObject::GetWorld())
	{
		MessageBus = ISGMessagingModule::Get().CreateBus(World->GetName());
	}
}

USGBlueprintMessageBus::~USGBlueprintMessageBus()
{
	if (!MessageBus.IsValid())
	{
		return;
	}

	const TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> DefaultBusPtr = MessageBus;

	MessageBus->Shutdown();

	MessageBus.Reset();

	// wait for the bus to shut down
	int32 SleepCount = 0;

	while (DefaultBusPtr.IsValid())
	{
		if (SleepCount > 10)
		{
			check(!"Something is holding on the default message bus");
			break;
		}

		++SleepCount;

		FPlatformProcess::Sleep(0.1f);
	}
}

TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> USGBlueprintMessageBus::GetMessageBus()
{
	return MessageBus;
}
