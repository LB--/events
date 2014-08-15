#include "resplunk/event/Cloneable.hpp"

namespace resplunk
{
	namespace event
	{
		template<>
		Cloneable<>::Registrar_t Cloneable<>::Implementor_t::registrar {};
		template<>
		Cloneable<std::shared_ptr>::Registrar_t Cloneable<std::shared_ptr>::Implementor_t::registrar {};
	}
}
