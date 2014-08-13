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
			using typename util::SpecificToEvent<T>::SpecificTo_t;
		private:
			Destruct(T &inst) noexcept
			: SpecificTo_t(inst)
			{
			}
			friend T/*::~T()*/;
		};
	}
}

#endif
