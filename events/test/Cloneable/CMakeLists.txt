
add_executable(test-Cloneable-basic
	"basic.cpp"
)
target_link_libraries(test-Cloneable-basic
	PUBLIC
		events
)
add_test(
	NAME    test-Cloneable-basic
	COMMAND test-Cloneable-basic
)
