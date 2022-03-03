// Fill out your copyright notice in the Description page of Project Settings.


#include "SGMessagingTestSubsystem.h"

void USGMessagingTestSubsystem::TestPublishSubscribe()
{
	if (TestPublishSubscribeDelegate.IsBound())
	{
		TestPublishSubscribeDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestRequestReply()
{
	if (TestRequestReplyDelegate.IsBound())
	{
		TestRequestReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestForward()
{
	if (TestForwardReplyDelegate.IsBound())
	{
		TestForwardReplyDelegate.Broadcast();
	}
}
