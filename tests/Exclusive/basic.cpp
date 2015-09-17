#undef NDEBUG

#include "Exclusive.hpp"

#include <cassert>

template<typename... Args>
using EventImplementor = LB::event::Implementor<Args...>;
template<typename... Args>
using EventProcessor = LB::event::Processor<Args...>;
template<typename... Args>
using EventReactor = LB::event::Reactor<Args...>;
using ExclusiveEvent = LB::event::Exclusive;

struct TestEvent
: EventImplementor<TestEvent, ExclusiveEvent>
{
};
LB_EVENTS_EVENT(TestEvent);

int np = 0;
int nr = 0;

struct TestListener
: private EventProcessor<TestEvent>
, private EventReactor<TestEvent>
{
private:
	virtual void process(TestEvent &e) const noexcept override
	{
		++np;
		e.claim(this);
	}
	virtual void react(TestEvent const &e) noexcept override
	{
		++nr;
	}
} listener0, listener1;

int main() noexcept
{
	TestEvent{}.call();
	assert(np == 2);
	assert(nr == 1);
}
