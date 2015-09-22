#undef NDEBUG

#include "Cloneable.hpp"

#include <cassert>

template<typename... Args>
using EventImplementor = LB::events::Implementor<Args...>;
template<typename... Args>
using EventProcessor = LB::events::Processor<Args...>;
template<typename... Args>
using EventReactor = LB::events::Reactor<Args...>;
using CloneableEvent = LB::events::Cloneable;

struct TestEvent
: EventImplementor<TestEvent, CloneableEvent>
{
	TestEvent() noexcept = default;

protected:
	TestEvent(TestEvent const &) noexcept = default;

private:
	virtual TestEvent *clone() const noexcept override
	{
		return new TestEvent(*this);
	}
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
	}
	virtual void react(TestEvent const &e) noexcept override
	{
		++nr;
	}
} listener;

int main() noexcept
{
	TestEvent original;
	auto clone1 = CloneableEvent::Clone<>(original);
	auto clone2 = TestEvent::Clone<>(original);
	clone1->call();
	clone2->call();
	assert(np == 2);
	assert(nr == 2);
}
