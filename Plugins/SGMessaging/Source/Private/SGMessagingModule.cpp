// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"
#include "Misc/CoreMisc.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "Settings/Public/ISettingsModule.h"
#include "Bus/SGMessageDispatchTask.h"
#include "Interface/ISGMessageBus.h"
#include "Bus/SGMessageBus.h"
#include "Bridge/SGMessageBridge.h"
#include "Interface/ISGMessagingModule.h"
#include "Interface/ISGNetworkMessagingExtension.h"
#include "Settings/SGMessagingSettings.h"


#ifndef PLATFORM_SUPPORTS_SGMESSAGEBUS
	#define PLATFORM_SUPPORTS_SGMESSAGEBUS 1
#endif

DEFINE_LOG_CATEGORY(LogSGMessaging);

#define LOCTEXT_NAMESPACE "FSGMessagingModule"

/**
 * Implements the SGMessaging module.
 */
class FSGMessagingModule
	: public ISGMessagingModule
{

public:

	//~ ISGMessagingModule interface

	virtual FOnMessageBusStartupOrShutdown& OnMessageBusStartup() override
	{
		return OnMessageBusStartupDelegate;
	}

	virtual FOnMessageBusStartupOrShutdown& OnMessageBusShutdown() override
	{
		return OnMessageBusShutdownDelegate;
	}

	virtual TSharedPtr<ISGMessageBridge, ESPMode::ThreadSafe> CreateBridge(const FSGMessageAddress& Address, const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& Bus, const TSharedRef<ISGMessageTransport, ESPMode::ThreadSafe>& Transport) override
	{
		return MakeShared<FSGMessageBridge, ESPMode::ThreadSafe>(Address, Bus, Transport);
	}

	virtual TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> CreateBus(const TSharedPtr<ISGAuthorizeMessageRecipients>& RecipientAuthorizer) override
	{
		return CreateBus(FGuid::NewGuid().ToString(), RecipientAuthorizer);
	}

	virtual TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> CreateBus(FString InName, const TSharedPtr<ISGAuthorizeMessageRecipients>& RecipientAuthorizer) override
	{
		TSharedRef<ISGMessageBus, ESPMode::ThreadSafe> Bus = MakeShared<FSGMessageBus, ESPMode::ThreadSafe>(MoveTemp(InName), RecipientAuthorizer);

		Bus->OnShutdown().AddLambda([this, WeakBus = TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe>(Bus)]()
		{
			WeakBuses.RemoveSwap(WeakBus);
			OnMessageBusShutdownDelegate.Broadcast(WeakBus);
		});

		WeakBuses.Add(Bus);
		OnMessageBusStartupDelegate.Broadcast(Bus);
		return Bus;
	}

	virtual TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> GetDefaultBus(UObject* WorldContextObject = nullptr) const override
	{
#if WITH_EDITOR
		if(WorldContextObject != nullptr)
		{
			if(const auto World = WorldContextObject->GetWorld())
			{
				if(const auto Bus = DefaultBusMap.Find(World->GetFName()))
				{
					return *Bus;
				}
			}
		}

		return nullptr;
#else
		return DefaultBus;
#endif
	}
	
	virtual TArray<TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>> GetAllBuses() const override
	{
		TArray<TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>> Buses;
		for (const TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe>& WeakBus : WeakBuses)
		{
			if (TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = WeakBus.Pin())
			{
				Buses.Add(Bus.ToSharedRef());
			}
		}
		return Buses;
	}

public:

	//~ IModuleInterface interface

	virtual void StartupModule() override
	{
#if PLATFORM_SUPPORTS_SGMESSAGEBUS
		FCoreDelegates::OnPreExit.AddRaw(this, &FSGMessagingModule::HandleCorePreExit);
#if !WITH_EDITOR
		DefaultBus = CreateBus(TEXT("DefaultBus"), nullptr);
#endif
#endif	//PLATFORM_SUPPORTS_MESSAGEBUS

		if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project",
			                                 "Plugins",
			                                 "SGMessaging",
			                                 LOCTEXT("SGMessagingSettingsName", "SGMessaging"),
			                                 LOCTEXT("SGMessagingSettingsDescription",
			                                         "Configure the SGMessaging plugin"),
			                                 GetMutableDefault<USGMessagingSettings>()
			);
		}

#if WITH_EDITOR
		FWorldDelegates::OnPreWorldInitialization.AddLambda(
			[this](const UWorld* World, const UWorld::InitializationValues IVS)
			{
				DefaultBusMap.Add(World->GetFName(), CreateBus(World->GetFName().ToString(), nullptr));
			});

		FWorldDelegates::OnWorldCleanup.AddLambda(
			[this](const UWorld* World, bool bSessionEnded, bool bCleanupResources)
			{
				DefaultBusMap.Remove(World->GetFName());
			});
#endif
	}

	virtual void ShutdownModule() override
	{
		ShutdownDefaultBus();

#if PLATFORM_SUPPORTS_SGMESSAGEBUS
		FCoreDelegates::OnPreExit.RemoveAll(this);
#endif	//PLATFORM_SUPPORTS_MESSAGEBUS

		if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "SGMessaging");
		}
	}

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

protected:

	void ShutdownDefaultBus()
	{
#if WITH_EDITOR
		for (const auto& DefaultBusPair : DefaultBusMap)
		{
			auto DefaultBus = DefaultBusPair.Value;

			TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> DefaultBusPtr = DefaultBus;

			DefaultBus->Shutdown();

			DefaultBusPtr.Reset();

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
#else
		if (!DefaultBus.IsValid())
		{
			return;
		}

		TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> DefaultBusPtr = DefaultBus;

		DefaultBus->Shutdown();
		DefaultBus.Reset();

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
#endif

		// validate all other buses were also properly shutdown
		for (TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe> WeakBus : WeakBuses)
		{
			if (WeakBus.IsValid())
			{
				check(!"Something is holding on a message bus");
				break;
			}
		}
	}

private:

	/** Callback for Core shutdown. */
	void HandleCorePreExit()
	{
		ShutdownDefaultBus();
	}

private:
	/** Holds the message bus. */
#if WITH_EDITOR
	TMap<FName, TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe>> DefaultBusMap;
#else
	TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> DefaultBus;
#endif

	/** All buses that were created through this module including the default one. */
	TArray<TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe>> WeakBuses;

	/** The delegate fired when a message bus instance is started. */
	FOnMessageBusStartupOrShutdown OnMessageBusStartupDelegate;

	/** The delegate fired when a message bus instance is shutdown. */
	FOnMessageBusStartupOrShutdown OnMessageBusShutdownDelegate;
};

FName ISGNetworkMessagingExtension::ModularFeatureName("SGNetworkMessaging");

IMPLEMENT_MODULE(FSGMessagingModule, SGMessaging);
