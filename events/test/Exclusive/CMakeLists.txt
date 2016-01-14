
add_executable(test-Exclusive-basic
	"basic.cpp"
)
target_link_libraries(test-Exclusive-basic
	PUBLIC
		events
)
add_test(
	NAME    test-Exclusive-basic
	COMMAND test-Exclusive-basic
)
