#ifndef resplunk_event_Cloneable_HeaderPlusPlus
#define resplunk_event_Cloneable_HeaderPlusPlus

#include "resplunk/event/Event.hpp"
#include "resplunk/util/Cloneable.hpp"

namespace resplunk
{
	namespace event
	{
		struct Cloneable
		:            Implementor<Cloneable, Event>
		, util::CloneImplementor<Cloneable>
		{
		};
	}
}

#endif
