#ifndef resplunk_event_ConstructEvent_HeaderPlusPlus
#define resplunk_event_ConstructEvent_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct ConstructEvent
		: EventImplementor<ConstructEvent<T>, Event>
		{
			virtual T &instance() final
			{
				return inst;
			}
			virtual T const &instance() const final
			{
				return inst;
			}

		private:
			T &inst;
			ConstructEvent(T &inst)
			: inst(inst)
			{
			}
			friend T;
		};
	}
}

#endif
