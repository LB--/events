#ifndef resplunk_event_ConstructEvent_HeaderPlusPlus
#define resplunk_event_ConstructEvent_HeaderPlusPlus

#include "resplunk/util/SpecificToEvent.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct ConstructEvent
		: EventImplementor<ConstructEvent<T>, util::SpecificToEvent<T>>
		{
		private:
			ConstructEvent(T &inst)
			: util::SpecificTo<T>(inst)
			{
			}
			friend T;
		};
	}
}

#endif
