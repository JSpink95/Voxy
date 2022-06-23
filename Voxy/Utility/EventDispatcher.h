#pragma once
#include "CoreTypes.h"

#define DECLARE_EVENT_LISTENER_ARG0(Name)\
struct Name##Listener\
{\
	void operator()()\
	{\
		On##Name##();\
	}\
	virtual void On##Name##() {}\
}

#define DECLARE_EVENT_LISTENER_ARG1(Name, Arg0)\
struct EventListener_##Name\
{\
	virtual void On##Name##(Arg0 arg0) {}\
};

#define DECLARE_EVENT_LISTENER_ARG2(Name, Arg0, Arg1)\
struct EventListener_##Name\
{\
	virtual void On##Name##(Arg0 arg0, Arg1 arg1) {}\
};

#define DECLARE_EVENT_DISPATCHER(Name)\
struct EventDispatcher_##Name\
{\
	void SubscribeListener_##Name(EventListener_##Name##* listener)\
	{\
		listeners.push_back(listener);\
	}\
	void UnsubscribeListener_##Name(EventListener_##Name##* listener)\
	{\
		listeners.erase(std::find(listeners.begin(), listeners.end(), listener), listeners.end());\
	}\
	template<typename...TEventArgs>\
	void DispatchEvent_##Name(TEventArgs&&...args)\
	{\
		for (auto listener: listeners)\
			listener->On##Name##(std::forward<TEventArgs>(args)...);\
	}\
private:\
	List<EventListener_##Name##*> listeners = {};\
};

#define DECLARE_AUTO_EVENT_LISTENER(Name)\
struct AutoEventListener_##Name : public EventListener_##Name\
{\
	using Name ## Dispatcher = EventDispatcher_##Name;\
	AutoEventListener_##Name(Reference<Name ## Dispatcher> dispatcher)\
		: dispatcher(dispatcher)\
	{\
		dispatcher->SubscribeListener_##Name(this);\
	}\
	virtual ~AutoEventListener_##Name()\
	{\
		dispatcher->UnsubscribeListener_##Name(this);\
	}\
	Reference<Name ## Dispatcher> dispatcher;\
};

#define DECLARE_EVENT_SYSTEM_ARG0(Name)\
DECLARE_EVENT_LISTENER_ARG0(Name)\
DECLARE_EVENT_DISPATCHER(Name)\
DECLARE_AUTO_EVENT_LISTENER(Name)

#define DECLARE_EVENT_SYSTEM_ARG1(Name, Arg0)\
DECLARE_EVENT_LISTENER_ARG1(Name, Arg0)\
DECLARE_EVENT_DISPATCHER(Name)\
DECLARE_AUTO_EVENT_LISTENER(Name)

#define DECLARE_EVENT_SYSTEM_ARG2(Name, Arg0, Arg1)\
DECLARE_EVENT_LISTENER_ARG2(Name, Arg0, Arg1)\
DECLARE_EVENT_DISPATCHER(Name)\
DECLARE_AUTO_EVENT_LISTENER(Name)
