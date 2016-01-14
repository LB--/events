#ifndef LB_events_Exclusive_HeaderPlusPlus
#define LB_events_Exclusive_HeaderPlusPlus

#include "Event.hpp"

namespace LB
{
	namespace events
	{
		struct Exclusive
		: Implementor<Exclusive, Event>
		{
			using Claim_t = ReactorBase const *; //std::optional<ReactorBase const &>
			virtual ~Exclusive() = 0;

			virtual void claim(Claim_t r) noexcept
			{
				c = r;
			}
			virtual Claim_t claim() const noexcept final
			{
				return c;
			}

			virtual bool should_react(ReactorBase const &r) const noexcept override
			{
				return std::addressof(r) == c;
			}

		private:
			Claim_t c {};
		};
		inline Exclusive::~Exclusive() = default;
	}
}

#endif
