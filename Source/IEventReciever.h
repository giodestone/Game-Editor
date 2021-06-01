#pragma once

/// <summary>
/// Interface for an event receiver.
/// </summary>
class IEventReciever
{
	friend class IEventDispatcher;

protected:
	virtual void OnRecievedEvent() = 0;
	
public:
	IEventReciever() = default;
	virtual ~IEventReciever() = default;
};

