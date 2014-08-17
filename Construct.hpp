#ifndef resplunk_event_Construct_HeaderPlusPlus
#define resplunk_event_Construct_HeaderPlusPlus

#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		template<typename T>
		struct Construct
		: Implementor<Construct<T>, Event>
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
			Construct(T &inst) noexcept
			: inst(inst)
			{
			}
			friend T/*::T(Args...)*/;
		};
	}
}

#endif
