
add_executable(test-Cancellable-basic
	"basic.cpp"
)
target_link_libraries(test-Cancellable-basic
	PUBLIC
		events
)
add_test(
	NAME    test-Cancellable-basic
	COMMAND test-Cancellable-basic
)
