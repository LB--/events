#undef NDEBUG

#include "resplunk/event/Cancellable.hpp"

#include <cassert>

template<typename... Args>
using EventImplementor = resplunk::event::Implementor<Args...>;
template<typename... Args>
using EventProcessor = resplunk::event::Processor<Args...>;
template<typename... Args>
using EventReactor = resplunk::event::Reactor<Args...>;
using CancellableEvent = resplunk::event::Cancellable;

struct TestEvent
: EventImplementor<TestEvent, CancellableEvent>
{
	int x;
	TestEvent(int x) noexcept
	: x(x)
	{
	}
};
RESPLUNK_EVENT(TestEvent);

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
			e.cancelled(true);
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
	assert(n == 2);
}
