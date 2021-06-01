#include "IEventDispatcher.h"
#include "IEventReciever.h"

void IEventDispatcher::DispatchEvent()
{
	for (auto er : eventRecievers)
	{
		er->OnRecievedEvent();
	}
}

void IEventDispatcher::SubscribeReciever(IEventReciever* reciever)
{
	eventRecievers.push_back(reciever);
}
