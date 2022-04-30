// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Bridge/SGMessageBridge.h"
#include "Core/Interface/ISGMessagingModule.h"
#include "Core/Interface/ISGMessageBus.h"
#include "Core/Interface/ISGMessageSubscription.h"
#include "Core/Interface/ISGMessageTransport.h"


/* FSGMessageBridge structors
 *****************************************************************************/

FSGMessageBridge::FSGMessageBridge(
	const FSGMessageAddress InAddress,
	const TSharedRef<ISGMessageBus, ESPMode::ThreadSafe>& InBus,
	const TSharedRef<ISGMessageTransport, ESPMode::ThreadSafe>& InTransport
)
	: Address(InAddress)
	  , Bus(InBus)
	  , Enabled(false)
	  , Id(FGuid::NewGuid())
	  , Transport(InTransport)
{
	Bus->OnShutdown().AddRaw(this, &FSGMessageBridge::HandleMessageBusShutdown);
}


FSGMessageBridge::~FSGMessageBridge()
{
	Disable();

	if (Bus.IsValid())
	{
		Bus->OnShutdown().RemoveAll(this);
		Bus->Unregister(Address);

		TArray<FSGMessageAddress> RemovedAddresses;
		AddressBook.RemoveAll(RemovedAddresses);

		for (const auto& RemovedAddress : RemovedAddresses)
		{
			Bus->Unregister(RemovedAddress);
		}
	}
}


/* ISGMessageBridge interface
 *****************************************************************************/

void FSGMessageBridge::Disable()
{
	if (!Enabled)
	{
		return;
	}

	// disable subscription & transport
	if (MessageSubscription.IsValid())
	{
		MessageSubscription->Disable();
	}

	if (Transport.IsValid())
	{
		Transport->StopTransport();
	}

	Enabled = false;
}


void FSGMessageBridge::Enable()
{
	if (Enabled || !Bus.IsValid() || !Transport.IsValid())
	{
		return;
	}

	// enable subscription & transport
	if (!Transport->StartTransport(*this))
	{
		return;
	}

	Bus->Register(Address, AsShared());

	if (MessageSubscription.IsValid())
	{
		MessageSubscription->Enable();
	}
	else
	{
		MessageSubscription = Bus->Subscribe(AsShared(), NAME_All,
		                                     FSGMessageScopeRange::AtLeast(ESGMessageScope::Network));
	}

	Enabled = true;
}


bool FSGMessageBridge::IsEnabled() const
{
	return Enabled;
}


/* ISGMessageReceiver interface
 *****************************************************************************/

FName FSGMessageBridge::GetDebugName() const
{
	return *FString::Printf(TEXT("FSGMessageBridge (%s)"), *Transport->GetDebugName().ToString());
}


const FGuid& FSGMessageBridge::GetRecipientId() const
{
	return Id;
}


ENamedThreads::Type FSGMessageBridge::GetRecipientThread() const
{
	return ENamedThreads::AnyThread;
}


bool FSGMessageBridge::IsLocal() const
{
	return false;
}


void FSGMessageBridge::ReceiveMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context)
{
	UE_LOG(LogSGMessaging, Verbose, TEXT("Received %s message from %s"), *Context->GetMessageTag().ToString(),
	       *Context->GetSender().ToString());

	if (!Enabled)
	{
		return;
	}

	// get remote nodes
	TArray<FGuid> RemoteNodes;

	if (Context->GetRecipients().Num() > 0)
	{
		RemoteNodes = AddressBook.GetNodesFor(Context->GetRecipients());

		if (RemoteNodes.Num() == 0)
		{
			return;
		}
	}

	// forward message to remote nodes
	Transport->TransportMessage(Context, RemoteNodes);
}


/* ISGMessageSender interface
 *****************************************************************************/

FSGMessageAddress FSGMessageBridge::GetSenderAddress()
{
	return Address;
}


void FSGMessageBridge::NotifyMessageError(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
                                          const FString& Error)
{
	// deprecated
}


/* ISGMessageTransportHandler interface
 *****************************************************************************/

void FSGMessageBridge::DiscoverTransportNode(const FGuid& NodeId)
{
	// do nothing (address book is updated in ReceiveTransportMessage)
}


void FSGMessageBridge::ForgetTransportNode(const FGuid& NodeId)
{
	TArray<FSGMessageAddress> RemovedAddresses;

	// update address book
	AddressBook.RemoveNode(NodeId, RemovedAddresses);

	// unregister endpoints
	if (Bus.IsValid())
	{
		for (const auto& RemovedAddress : RemovedAddresses)
		{
			Bus->Unregister(RemovedAddress);
		}
	}
}


void FSGMessageBridge::ReceiveTransportMessage(const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>& Context,
                                               const FGuid& NodeId)
{
	if (UE_GET_LOG_VERBOSITY(LogSGMessaging) >= ELogVerbosity::Verbose)
	{
		const FString RecipientStr = FString::JoinBy(Context->GetRecipients(), TEXT("+"), &FSGMessageAddress::ToString);
		UE_LOG(LogSGMessaging, Verbose, TEXT("FSGMessageBridge::ReceiveTransportMessage: Received %s from %s for %s"),
		       *Context->GetMessageTag().ToString(),
		       *Context->GetSender().ToString(),
		       *RecipientStr);
	}

	if (!Enabled || !Bus.IsValid())
	{
		return;
	}

	// discard expired messages
	if (Context->GetExpiration() < FDateTime::UtcNow())
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("FSGMessageBridge::ReceiveTransportMessage: Message expired. Discarding"));
		return;
	}

	// register newly discovered endpoints
	if (!AddressBook.Contains(Context->GetSender()))
	{
		UE_LOG(LogSGMessaging, Verbose, TEXT("FSGMessageBridge::ReceiveTransportMessage: Registering new sender %s"),
		       *Context->GetMessageTag().ToString());

		AddressBook.Add(Context->GetSender(), NodeId);
		Bus->Register(Context->GetSender(), AsShared());
	}

	// forward message to local bus
	Bus->Forward(Context, Context->GetRecipients(), FTimespan::Zero(), AsShared());
}


/* FSGMessageBridge callbacks
 *****************************************************************************/

void FSGMessageBridge::HandleMessageBusShutdown()
{
	Disable();
	Bus.Reset();
}
