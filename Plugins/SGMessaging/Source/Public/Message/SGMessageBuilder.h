#pragma once

class FSGMessageBuilder
{
public:
	template <typename MessageType, typename ...Args>
	static MessageType* Builder(Args&&... Params)
	{
		auto Message = new MessageType();

		Add(Message, Params...);

		return Message;
	}

private:
	template <typename MessageType>
	static void Add(MessageType* Message)
	{
	}

	template <typename MessageType, typename NameType, typename ParamType>
	static void Add(MessageType* Message, const NameType& Key, ParamType&& Value)
	{
		Message->Add(FString(Key), Forward<ParamType>(Value));
	}

	template <typename MessageType, typename NameType, typename ParamType, typename... Args>
	static void Add(MessageType* Message, const NameType& Key, ParamType&& Value, Args&&... Params)
	{
		Message->Add(FString(Key), Forward<ParamType>(Value));

		Add(Message, Params...);
	}
};
