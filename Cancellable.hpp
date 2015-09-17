#ifndef LB_events_Cancellable_HeaderPlusPlus
#define LB_events_Cancellable_HeaderPlusPlus

#include "Event.hpp"

namespace LB
{
	namespace events
	{
		struct Cancellable
		: Implementor<Cancellable, Event>
		{
			virtual ~Cancellable() = 0;

			virtual void cancelled(bool c) noexcept
			{
				cancel = c;
			}
			virtual bool cancelled() const noexcept final
			{
				return cancel;
			}

			virtual bool should_react(ReactorBase const &) const noexcept override
			{
				return !cancel;
			}

		private:
			bool cancel = false;
		};
		inline Cancellable::~Cancellable() = default;
	}
}

#endif
