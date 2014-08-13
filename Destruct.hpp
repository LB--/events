#ifndef resplunk_event_Destruct_HeaderPlusPlus
#define resplunk_event_Destruct_HeaderPlusPlus

#include "resplunk/util/SpecificToEvent.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct Destruct final
		: Implementor<Destruct<T>, util::SpecificToEvent<T>>
		{
		private:
			Destruct(T &inst)
			: util::SpecificTo<T>(inst)
			{
			}
			friend T/*::~T()*/;
		};
	}
}

#endif
