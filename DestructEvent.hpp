#ifndef resplunk_event_DestructEvent_HeaderPlusPlus
#define resplunk_event_DestructEvent_HeaderPlusPlus

#include "resplunk/util/SpecificToEvent.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct DestructEvent final
		: EventImplementor<DestructEvent<T>, util::SpecificToEvent<T>>
		{
		private:
			DestructEvent(T &inst)
			: util::SpecificTo<T>(inst)
			{
			}
			friend T/*::~T()*/;
		};
	}
}

#endif
