#include "resplunk/event/Event.hpp"

namespace resplunk
{
	namespace event
	{
		template<>
		Event::Registrar_t Event::Implementor_t::registrar {};
	}
}
