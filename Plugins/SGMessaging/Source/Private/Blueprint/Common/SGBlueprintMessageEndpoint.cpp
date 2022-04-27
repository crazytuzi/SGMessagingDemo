#include "Blueprint/Common/SGBlueprintMessageEndpoint.h"

USGBlueprintMessageEndpoint::USGBlueprintMessageEndpoint()
{
}

USGBlueprintMessageEndpoint::~USGBlueprintMessageEndpoint()
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint.Reset();
	}
}

void USGBlueprintMessageEndpoint::Subscribe(MESSAGE_TAG_PARAM_SIGNATURE,
                                            const FSGBlueprintMessageDelegate& InDelegate)
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint->Subscribe<FSGBlueprintMessage, FSGBlueprintMessageContext>(
			MESSAGE_TAG_PARAM_VALUE, InDelegate.GetUObject(), InDelegate.GetFunctionName());
	}
}

void USGBlueprintMessageEndpoint::Publish(MESSAGE_TAG_PARAM_SIGNATURE,
                                          const FSGBlueprintPublishParameter MESSAGE_PARAMETER,
                                          const FSGBlueprintMessage InMessage)
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint->PublishWithMessage<FSGMessage>(
			MESSAGE_TAG_PARAM_VALUE, FSGMessageParameter::FPublishParameter(MESSAGE_PARAMETER), InMessage);
	}
}

void USGBlueprintMessageEndpoint::Send(MESSAGE_TAG_PARAM_SIGNATURE,
                                       const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                       const FSGBlueprintSendParameter MESSAGE_PARAMETER,
                                       const FSGBlueprintMessage InMessage)
{
	if (MessageEndpoint.IsValid())
	{
		TArray<FSGMessageAddress> Recipients;

		Recipients.Reset(InRecipients.Num());

		for (auto Recipient : InRecipients)
		{
			Recipients.Add(Recipient);
		}

		MessageEndpoint->SendWithMessage<FSGMessage>(
			MESSAGE_TAG_PARAM_VALUE, Recipients, FSGMessageParameter::FSendParameter(MESSAGE_PARAMETER), InMessage);
	}
}

void USGBlueprintMessageEndpoint::Forward(const FSGBlueprintMessageContext& InContext,
                                          const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                          const FTimespan InDelay)
{
	if (MessageEndpoint.IsValid())
	{
		TArray<FSGMessageAddress> Recipients;

		Recipients.Reset(InRecipients.Num());

		for (auto Recipient : InRecipients)
		{
			Recipients.Add(Recipient);
		}

		MessageEndpoint->Forward(InContext, Recipients, InDelay);
	}
}

void USGBlueprintMessageEndpoint::Unsubscribe(
	MESSAGE_TAG_PARAM_SIGNATURE, const FSGBlueprintMessageDelegate& InDelegate)
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint->Unsubscribe<FSGBlueprintMessage, FSGBlueprintMessageContext>(
			MESSAGE_TAG_PARAM_VALUE, InDelegate.GetUObject(), InDelegate.GetFunctionName());
	}
}

FSGBlueprintMessageAddress USGBlueprintMessageEndpoint::GetAddress() const
{
	if (MessageEndpoint.IsValid())
	{
		return FSGBlueprintMessageAddress(MessageEndpoint->GetAddress());
	}

	return FSGBlueprintMessageAddress();
}
