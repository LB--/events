#cmake_minimum_required(VERSION 3.1)

project(events)

ADD_BII_TARGETS()

#set_property(TARGET ${BII_BLOCK_TARGET} PROPERTY CXX_STANDARD 17)
target_compile_options(${BII_BLOCK_TARGET} INTERFACE "-std=c++1z")
