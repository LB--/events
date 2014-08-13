#ifndef resplunk_event_Cloneable_HeaderPlusPlus
#define resplunk_event_Cloneable_HeaderPlusPlus

#include "resplunk/event/Event.hpp"
#include "resplunk/util/Cloneable.hpp"

namespace resplunk
{
	namespace event
	{
		template<template<typename T, typename...> typename Wrapper = std::unique_ptr, typename... Args>
		struct Cloneable
		:       Implementor<Cloneable<Wrapper, Args...>, Event>
		, util::CloneImplementor<Cloneable<Wrapper, Args...>, Wrapper, Args...>
		{
		};
	}
}

#endif
