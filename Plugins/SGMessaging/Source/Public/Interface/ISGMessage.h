#pragma once

class ISGMessage
{
public:
	virtual FName GetFName() const = 0;

public:
	virtual ~ISGMessage()
	{
	}
};
