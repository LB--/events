#ifndef resplunk_event_Construct_HeaderPlusPlus
#define resplunk_event_Construct_HeaderPlusPlus

#include "resplunk/util/SpecificToEvent.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct Construct
		: Implementor<Construct<T>, util::SpecificToEvent<T>>
		{
		private:
			Construct(T &inst)
			: util::SpecificTo<T>(inst)
			{
			}
			friend T;
		};
	}
}

#endif
