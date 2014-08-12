#ifndef resplunk_event_ServerSpecificEvent_HeaderPlusPlus
#define resplunk_event_ServerSpecificEvent_HeaderPlusPlus

#include "resplunk/event/Event.hpp"
#include "resplunk/server/ServerSpecific.hpp"

namespace resplunk
{
	namespace event
	{
		struct ServerSpecificEvent
		: EventImplementor<ServerSpecificEvent, Event>
		, virtual ServerSpecific
		{
			virtual ~ServerSpecificEvent() = 0;
		};
		inline ServerSpecificEvent::~ServerSpecificEvent() = default;
	}
}

#endif
