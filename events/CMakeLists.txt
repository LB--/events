cmake_minimum_required(VERSION 3.4)

project(LB::events
	VERSION 0.0.0
	LANGUAGES CXX
)

include(${CMAKE_INSTALL_PREFIX}/cmake/LB/cloning/cloning.cmake)
include(${CMAKE_INSTALL_PREFIX}/cmake/LB/tuples/tuples.cmake)

add_library(events
	"Cancellable.cpp"
	"Cloneable.cpp"
	"Event.cpp"
	"Exclusive.cpp"
)
#set_property(TARGET events PROPERTY CXX_STANDARD 17)
target_compile_options(events
	PUBLIC
		"-std=c++1z"
)
target_include_directories(events
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)
target_link_libraries(events
	PUBLIC
		LB::cloning
		LB::tuples
)

install(
	TARGETS
		events
	EXPORT events
	ARCHIVE DESTINATION lib
)
install(
	EXPORT events
	DESTINATION cmake/LB/events
	NAMESPACE LB::
)
install(
	FILES
		"Cancellable.hpp"
		"Cloneable.hpp"
		"Event.hpp"
		"Events.hpp"
		"Exclusive.hpp"
		"RAII.hpp"
	DESTINATION include/LB/events
)

enable_testing()
add_subdirectory("test/")
