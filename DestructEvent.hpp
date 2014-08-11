#ifndef resplunk_event_DestructEvent_HeaderPlusPlus
#define resplunk_event_DestructEvent_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct DestructEvent final
		: EventImplementor<DestructEvent, Event>
		{
			T &instance()
			{
				return inst;
			}
			T const &instance() const
			{
				return inst;
			}

		private:
			T &inst;
			DestructEvent(Server &s, T &inst)
			: ServerSpecific(s)
			, inst(inst)
			{
			}
			friend T::~T();
		};
	}
}

#endif
