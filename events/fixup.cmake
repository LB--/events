
message(STATUS "Fixup file: @_export@")
file(READ "@_export@" _config)
file(WRITE "@_export@"
	"find_package(LB/cloning REQUIRED)\n"
	"find_package(LB/tuples REQUIRED)\n\n"
	"${_config}"
)
