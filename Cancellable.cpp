#include "resplunk/event/Cancellable.hpp"

namespace resplunk
{
	namespace event
	{
		template<>
		Cancellable::Registrar_t Cancellable::Implementor_t::registrar {};
	}
}
