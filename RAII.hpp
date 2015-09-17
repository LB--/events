#ifndef LB_events_RAII_HeaderPlusPlus
#define LB_events_RAII_HeaderPlusPlus

#include "Event.hpp"

namespace LB
{
	namespace events
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
