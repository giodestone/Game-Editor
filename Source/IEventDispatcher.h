#pragma once
#include <vector>
#include "IEventReciever.h"

/// <summary>
/// Interface for a class that dispatches events.
/// </summary>
class IEventDispatcher
{
private:
	std::vector<IEventReciever*> eventRecievers;
	
public:
	IEventDispatcher() = default;
	~IEventDispatcher() = default;

	/// <summary>
	/// Dispatch the event to all recievers.
	/// </summary>
	void DispatchEvent();

	/// <summary>
	/// Subscribe a receiver to receive the messages.
	/// </summary>
	/// <param name="reciever"></param>
	void SubscribeReciever(IEventReciever* reciever);
};

