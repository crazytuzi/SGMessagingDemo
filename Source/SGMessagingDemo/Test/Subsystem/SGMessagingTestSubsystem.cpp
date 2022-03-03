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

void USGMessagingTestSubsystem::TestForwardReply()
{
	if (TestForwardReplyDelegate.IsBound())
	{
		TestForwardReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestDelayPublishSubscribe()
{
	if (TestDelayPublishSubscribeDelegate.IsBound())
	{
		TestDelayPublishSubscribeDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestDelayRequestReply()
{
	if (TestDelayRequestReplyDelegate.IsBound())
	{
		TestDelayRequestReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestDelayForwardReply()
{
	if (TestDelayForwardReplyDelegate.IsBound())
	{
		TestDelayForwardReplyDelegate.Broadcast();
	}
}
