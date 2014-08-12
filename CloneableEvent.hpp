#ifndef resplunk_event_CloneableEvent_HeaderPlusPlus
#define resplunk_event_CloneableEvent_HeaderPlusPlus

#include "resplunk/event/Event.hpp"
#include "resplunk/util/Cloneable.hpp"

namespace resplunk
{
	namespace event
	{
		template<template<typename T, typename...> typename Wrapper = std::unique_ptr, typename... Args>
		struct CloneableEvent
		:       EventImplementor<CloneableEvent<Wrapper, Args...>, Event>
		, util::CloneImplementor<CloneableEvent<Wrapper, Args...>, Wrapper, Args...>
		{
		};
	}
}

#endif
