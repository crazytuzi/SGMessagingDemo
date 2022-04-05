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

void USGBlueprintMessageEndpoint::Subscribe(const int32 InTopicID, const int32 InMessageID,
                                            const FSGBlueprintMessageDelegate& InDelegate)
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint->Subscribe(MESSAGE_TAG_PARAM_VALUE, TSGLambdaMessageHandler<FSGMessage>::FuncType(
			                           [this, InDelegate](ISGMessage* Message,
			                                              const TSharedRef<ISGMessageContext, ESPMode::ThreadSafe>&
			                                              Context)
			                           {
				                           if (InDelegate.IsBound())
				                           {
					                           InDelegate.Execute(Message, Context);
				                           }
			                           }));
	}
}

void USGBlueprintMessageEndpoint::Publish(const int32 InTopicID, const int32 InMessageID,
                                          const FSGBlueprintPublishParameter InParameter,
                                          const FSGBlueprintMessage InMessage)
{
	if (MessageEndpoint.IsValid())
	{
		MessageEndpoint->PublishWithMessage<FSGMessage>(
			MESSAGE_TAG_PARAM_VALUE, FSGMessageParameter::FPublishParameter(MESSAGE_PARAMETER), InMessage);
	}
}

void USGBlueprintMessageEndpoint::Send(const int32 InTopicID, const int32 InMessageID,
                                       const TArray<FSGBlueprintMessageAddress>& InRecipients,
                                       const FSGBlueprintSendParameter InParameter,
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

FSGBlueprintMessageAddress USGBlueprintMessageEndpoint::GetAddress() const
{
	if (MessageEndpoint.IsValid())
	{
		return FSGBlueprintMessageAddress(MessageEndpoint->GetAddress());
	}

	return FSGBlueprintMessageAddress();
}
