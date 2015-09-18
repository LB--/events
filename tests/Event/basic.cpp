#undef NDEBUG

#include "Event.hpp"

#include <cassert>

template<typename... Args>
using EventImplementor = LB::events::Implementor<Args...>;
template<typename... Args>
using EventProcessor = LB::events::Processor<Args...>;
template<typename... Args>
using EventReactor = LB::events::Reactor<Args...>;
using Event = LB::events::Event;

struct TestEvent
: EventImplementor<TestEvent, Event>
{
	int x;
	TestEvent(int x) noexcept
	: x(x)
	{
	}
};
LB_EVENTS_EVENT(TestEvent);

int n = 0;

struct TestListener
: private EventProcessor<TestEvent>
, private EventReactor<TestEvent>
{
private:
	virtual void process(TestEvent &e) const noexcept override
	{
		if(e.x > 10)
		{
			e.x = 10;
		}
		else if(e.x < 0)
		{
			e.x = 0;
		}
	}
	virtual void react(TestEvent const &e) noexcept override
	{
		++n;
		assert(e.x >= 0 && e.x <= 10);
	}
} listener;

int main() noexcept
{
	TestEvent{7}.call();
	TestEvent{-7}.call();
	TestEvent{17}.call();
	assert(n == 3);
}
