cmake_minimum_required(VERSION 2.6)

include_directories(
	include
)

add_subdirectory(src)

install(FILES svmop_feature_parameter.sample DESTINATION share/samples)

