// Fill out your copyright notice in the Description page of Project Settings.


#include "SGMessagingTestSubsystem.h"

void USGMessagingTestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Case = NewObject<USGTestParameterCase>(this);

	Case->Initialize();
}

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

void USGMessagingTestSubsystem::TestBlueprintPublishSubscribe()
{
	if (TestBlueprintPublishSubscribeDelegate.IsBound())
	{
		TestBlueprintPublishSubscribeDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBlueprintRequestReply()
{
	if (TestBlueprintRequestReplyDelegate.IsBound())
	{
		TestBlueprintRequestReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBlueprintForwardReply()
{
	if (TestBlueprintForwardReplyDelegate.IsBound())
	{
		TestBlueprintForwardReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBlueprintDelayPublishSubscribe()
{
	if (TestBlueprintDelayPublishSubscribeDelegate.IsBound())
	{
		TestBlueprintDelayPublishSubscribeDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBlueprintDelayRequestReply()
{
	if (TestBlueprintDelayRequestReplyDelegate.IsBound())
	{
		TestBlueprintDelayRequestReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBlueprintDelayForwardReply()
{
	if (TestBlueprintDelayForwardReplyDelegate.IsBound())
	{
		TestBlueprintDelayForwardReplyDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestCpp2CppParameter()
{
	if (TestCpp2CppParameterDelegate.IsBound())
	{
		TestCpp2CppParameterDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestCpp2BPParameter()
{
	if (TestCpp2BPParameterDelegate.IsBound())
	{
		TestCpp2BPParameterDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBP2CppParameter()
{
	if (TestBP2CppParameterDelegate.IsBound())
	{
		TestBP2CppParameterDelegate.Broadcast();
	}
}

void USGMessagingTestSubsystem::TestBP2BPParameter()
{
	if (TestBP2BPParameterDelegate.IsBound())
	{
		TestBP2BPParameterDelegate.Broadcast();
	}
}
