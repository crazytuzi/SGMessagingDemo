#pragma once

class FSGMessageBuilder
{
public:
	template <typename MessageType, typename ...Args>
	static MessageType* Builder(Args&&... InParams)
	{
		auto Buffer = FMemory::Malloc(sizeof(MessageType));

		auto Message = new(Buffer) MessageType(Forward<Args>(InParams)...);

		return Message;
	}
};
