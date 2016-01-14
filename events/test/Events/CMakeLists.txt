
add_executable(test-Events-order
	"order.cpp"
)
target_link_libraries(test-Events-order
	PUBLIC
		events
)
add_test(
	NAME    test-Events-order
	COMMAND test-Events-order
)
