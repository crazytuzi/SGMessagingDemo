// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "Settings/Public/ISettingsModule.h"
#include "Core/Interface/ISGMessageBus.h"
#include "Core/Bus/SGMessageBus.h"
#include "Core/Bridge/SGMessageBridge.h"
#include "Core/Interface/ISGMessagingModule.h"
#include "Core/Interface/ISGNetworkMessagingExtension.h"
#include "Core/Settings/SGMessagingSettings.h"


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
			WeakBuses.Remove(FName(WeakBus.Pin()->GetName()));
			OnMessageBusShutdownDelegate.Broadcast(WeakBus);
		});

		WeakBuses.Add(FName(InName), Bus);
		OnMessageBusStartupDelegate.Broadcast(Bus);
		return Bus;
	}

	virtual TArray<TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>> GetAllBuses() const override
	{
		TArray<TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>> Buses;
		for (const auto& WeakBus : WeakBuses)
		{
			if (TSharedPtr<ISGMessageBus, ESPMode::ThreadSafe> Bus = WeakBus.Value.Pin())
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
		// validate all other buses were also properly shutdown
		for (const auto& WeakBus : WeakBuses)
		{
			if (WeakBus.Value.IsValid())
			{
				ensureMsgf(!WeakBus.Value.IsValid(), TEXT("Something is holding on a message bus"));
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
	/** All buses that were created through this module including the default one. */
	TMap<FName, TWeakPtr<ISGMessageBus, ESPMode::ThreadSafe>> WeakBuses;

	/** The delegate fired when a message bus instance is started. */
	FOnMessageBusStartupOrShutdown OnMessageBusStartupDelegate;

	/** The delegate fired when a message bus instance is shutdown. */
	FOnMessageBusStartupOrShutdown OnMessageBusShutdownDelegate;
};

FName ISGNetworkMessagingExtension::ModularFeatureName("SGNetworkMessaging");

IMPLEMENT_MODULE(FSGMessagingModule, SGMessaging);
