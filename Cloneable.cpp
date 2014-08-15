#include "resplunk/event/Cloneable.hpp"

namespace resplunk
{
	namespace event
	{
		template<>
		Cloneable<>::Registrar_t Cloneable<>::Implementor_t::registrar {};
	}
}
