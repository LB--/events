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
			using typename util::SpecificToEvent<T>::SpecificTo_t;
		private:
			Construct(T &inst)
			: SpecificTo_t(inst)
			{
			}
			friend T/*::T(Args...)*/;
		};
	}
}

#endif
