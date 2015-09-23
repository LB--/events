events [![Build Status](https://webapi.biicode.com/v1/badges/LB/LB/events/master)](https://www.biicode.com/LB/events)
======
Whereas most event libraries allow changing both the event itself and the state of the program while enumerating handlers, this library specifically forbids that by separating event handling into two phases.
The first phase is the _processing_ phase, where the event iself can be changed, but the state of the program _must not change_.
The second phase is the _reacting_ phase, where the event itself cannot be changed, and the state of the program _should_ change.
Events are classes and support inheritance, including multiple inheritance.

_This library requires that your compiler support C++1z (the C++ standard after C++14)_

## Processing vs Reacting
The difference between processing and reacting is by design - there is no common "listener" class between processors and reactors.
Processing an event means _only the event should be modified_.
Trying to change the state of the application during the processing phase is _wrong_ and should never be done.
Reacting is where the event can no longer be modified, but you are _expected_ to change the state of the application.
This is because an event might never move on from the processing phase to the reacting phase, and if you already did something because of the event during the processing phase, there is no way to know that you need to undo it.
Alternatively, the caller may wish that the event be reacted to without processing it first.

This library _tries_ to enforce the distinction between processing and reacting through const-correctness, but it's not possible to prevent misuse entirely.
For processing an event, the event is non-const but the processor is const.
For reacting to an event, the event is const but the reactor is non-const.
Additionally, some events' member functions have inversed const-correctness to mirror processing vs reacting (you may be familiar with logical-const vs bitwise-const).
If you try to use `const_cast`, that's _wrong_, **even if you're going from non-const to const**.
Beware implicit casts - they can bypass the two-phase dichotomy unintentionally.
It is up to you to not violate the contract.

## Order of Execution
When events are called, the call propagates through the entire class hierarchy.
The very first thing that happens is that the root `Event` class' listeners are triggered, then it moves down through the class hierarchy.
With multiple inheritance, the base class listeners are triggered in order from left to right as specified.
Per invocation of an event, all classes in the hierarchy get their listeners called exactly once.
This logic is generated at compile time via template metaprograms, thus there is very little runtime overhead.

Example:
```txt
   0
  / \
/ / \ \
1 2 3 4
\ / \ /
 5   6
  \ /
   7
```
0 is the root `Event` class, and 7 is the fully derived type of the event instance that was called.

## Listening to events
Classes which process events derive from `LB::events::Processor<>`, and classes which react to events derive from `LB::events::Reactor<>`.
There is also `LambdaProcessor<>` and `LambdaReactor<>` for the occasional time when you just need a simple lambda to handle an event.
A single class can derive from multiple instantiations of the `Processor`/`Reactor` classes to listen to multiple types of events.

**Note** that if your processor or reactor doesn't have any state, you should only ever need a single instance of it - otherwise you will process/react to an event multiple times in the same way, which is almost always undesireable.

You should also be aware of `LB::events::ListenerPriority`, which you can pass to the constructors for `Processor`/`Reactor`.
Priorities with lower values are executed before priorities with higher values.
If multiple listeners have the same priority, they are executed in the order they began listening.
You can change your priority or stop listening by calling `ignore()` from the particular `Processor`/`Reactor` and then optionally later calling `listen()` with, optionally, the priority.
For extreme cases, you can use `ListenerPriority::FIRST` or `ListenerPriority::LAST`.

**Note:** even with `FIRST` or `LAST` priority, less-specific listeners are always called before more-specific listeners.
That is, the inheritance tree is more important than the listener priority.

Example event listener:
```cpp
using WidgetEvent = MyNamespace::Widget::Event;
struct MyListener
: private LB::events::Processor<WidgetEvent>
, private LB::events::Reactor<WidgetEvent>
{
	int blah = 0;

private:
	virtual void process(WidgetEvent &e) const override
	{
		//blah is const
		//e.instance() returns const
		//you may change the event
		//you should NOT change the application state
	}
	virtual void react(WidgetEvent const &e) override
	{
		//blah is non-const
		//e.instance() returns non-const
		//you may NOT change the event
		//you should change the application state
	}
};
```
Take note of the differing locations of the keyword `const`.
For processing events, the member function is `const`.
For reacting to events, the events themselves are `const`.
Don't vioate the two-phase dichotomy - if you want to do evil things, just use any other event library ever.

## Calling Events
You can invoke `call()` to have any event be both `process()`ed and `react()`ed to. Events are called for the least derived classes first before moving on to more specific handling for more derived classes and finally reaching the most derived class.

Example:
```cpp
SomeEvent{/**/}.call();
```

## The predefined events
For your convenience, some types of events are defined for you.
Most predefined events are abstract and require you to derive them with your own events, which you will see how do do in the next section.

### `Event`
When you `#include "LB/events/Event.hpp"` you get access to `LB::events::Event`, which is the enforced base class of all events.

### `Cancellable`
When you `#include "LB/events/Cancellable.hpp"` you get access to `LB::events::Cancellable`, which is the base class for all events that can be cancelled.
If an event is cancelled, invoking `react()` will have no effect.
During the processing phase, call `cancelled(true)` to cancel reacting to the event, or `cancelled(false)` to override a previous processor's decision.
You can also call `cancelled()` with no parameters to see if the event will be reacted to or not.

### `Exclusive`
When you `#include "LB/events/Exclusive.hpp"` you get access to `LB::events::Exclusive`, which is the base class for all events which should have exactly either 0 or 1 reactors.
During the processing phase, the last processor to `claim(reactor)` the event will ensure that the specified reactor is the only reactor which will be called for the event.

### `Cloneable`
When you `#include "LB/events/Cloneable.hpp"` you get access to `LB::events::Cloneable`, which is the base class for all events that can be cloned.
Why or when would you need to clone an event?
That's for you to decide.

**Note** that you should consult the documentation for [`Cloneable` types](https://github.com/LB--/cloning) to learn how to properly derive this event.

### `Construct<>` and `Destruct<>`
When you `#include "LB/events/RAII.hpp"` you get access to `LB::events::Construct`, which is a template base class for classes that need to fire an event when they are constructed.
`Construct` events intentionally ignore the inheritance tree for their particular class - as they are called from constructors, the more-derived class constructors have not been called yet.
Once the `Construct` event has ended, the more-derived class' constructor will call its own `Construct` event, and so on.
Be aware, there's [an ugly part](#the-ugly-part).

Also when you `#include "LB/events/RAII.hpp"` you get access to `LB::events::Destruct` too, which is a template base class for classes that need to fire an event when they are destructed.
`Destruct` events intentionally ignore the inheritance tree for their particular class - as they are called from destructors, the less-derived class destructors have not been called yet.
Once the `Destruct` event has ended, the less-derived class' destructor will call its own `Destruct` event, and so on. Be aware, there's [an ugly part](#the-ugly-part).

## Implementing your own event type
To implement your own kind of event, you need to derive from `LB::events::Implementor<>`.
The first template parameter is your new event class, and the rest are all the parent event classes.
For example, to create a widget-specific event that can be cancelled:
```cpp
//In MyEvent.hpp
namespace MyNamespace
{
	struct MyEvent
	: LB::events::Implementor
	<
		MyEvent,
		LB::events::Cancellable,
		MyNamespace::Widget::Event
	>
	{
		MyEvent(Widget &s)
		: s(s)
		{
		}

		virtual Widget const &instance() noexcept override
		{
			return s;
		}
		virtual Widget &instance() const noexcept override //logical-const
		{
			return s;
		}

	private:
		Widget &s;
	};
}

//In MyEvent.cpp
LB_EVENTS_EVENT(MyNamespace::MyEvent);
//Yes, it's ugly, but it's required and cannot be inside any namespace.
```
That's it!
Your new event type is ready to be used.
[Just be sure to reread the processing vs reacting section](#processing-vs-reacting) before adding member functions and data members.
Members which affect the state of the event itself should have normal const-correctness.
Members which affect the state of the application should have _inversed_ const-correctness.

### Exception Guarantee
By default, exceptions can be thrown when processing or reacting to an event - only the minimal basic exception guarantee is made (the application will be in a consistent state, though not necessarily a predictable state).
You can enforce the `noexcept` exception guarantee on processing and reacting by adding a static member to your event class:
```cpp
static constexpr bool NOEXCEPT = true;
```
Now your event and all its derived events will have the `noexcept` guarantee.

### The Ugly Part
The `LB::events::Implementor` class needs to store static data (`LB::events::Registrar`), but since it is a template, you have to manually define that static data for every instantiation of `Implementor`.
You saw how to do it in the `MyEvent` example above - it's the only reason you need a source file at all.
The even uglier part is that events which are templates require you to define the static data for each instantiation of them you generate.
_Why is this even needed at all?_
Because **linkage in C++ sucks!**
Macros are a necessary evil here, unfortunately.
(Maybe one day when Modules get added to the C++ standard, this problem will go away.)

## Event Recursion
Although currently untested, event recursion should be fully supported.
Just don't try to recursively react to an event while processing another event - that would violate the two-phase dichotomy.

## Resolving Multiple Inheritance Conflicts
For the most part, multiple inheritance of events should work just fine.
The only case you may have issue with is when you try to inherit two or more events which override the same virtual function.
With normal multiple inheritance this would not be an issue as you would just override that member function in your class and be done with it, however C++ does not consider the fact that an abstract class need not override the ambiguous virtual function and so an error occurs while instantiating the `LB::events::Implementor` class.

Until a future standard of C++ eventually fixes this issue, you have to help out `Implementor` by providing a class that properly inherits all the parent classes you want:
```cpp
template<typename... ParentT>
struct MyCaseSpecificResolver
: virtual ParentT...
{
	//Override the conflicting virtual functions as pure virtual
	virtual void do_stuff() noexcept override = 0;

	//These would also be considered conflicting
	virtual void process() override = 0;
	virtual void react() const override = 0;
};
```
After that, use `Implementor` like this:
```cpp
struct MyEvent
: LB::events::Implementor<MyEvent, MyCaseSpecificResolver<Parent1, Parent2, Parent3>>
{
	virtual void do_stuff() noexcept override //= 0;
	{
		//...
	}
};
```
