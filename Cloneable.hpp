#ifndef LB_events_Cloneable_HeaderPlusPlus
#define LB_events_Cloneable_HeaderPlusPlus

#include "Event.hpp"
#include "LB/cloning/cloning.hpp"

namespace resplunk
{
	namespace event
	{
		struct Cloneable
		:                   Implementor<Cloneable, Event>
		, LB::cloning::CloneImplementor<Cloneable>
		{
		};
	}
}

#endif
