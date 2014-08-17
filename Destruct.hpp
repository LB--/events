#ifndef resplunk_event_Destruct_HeaderPlusPlus
#define resplunk_event_Destruct_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct Destruct
		: Implementor<Destruct<T>, Event>
		{
			virtual T const &instance() noexcept final
			{
				return inst;
			}
			virtual T &instance() const noexcept final
			{
				return inst;
			}

		private:
			T &inst;
			Destruct(T &inst) noexcept
			: inst(inst)
			{
			}
			friend T/*::~T()*/;
		};
	}
}

#endif
