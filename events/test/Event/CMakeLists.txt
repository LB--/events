
add_executable(test-Event-basic
	"basic.cpp"
)
target_link_libraries(test-Event-basic
	PUBLIC
		events
)
add_test(
	NAME    test-Event-basic
	COMMAND test-Event-basic
)
