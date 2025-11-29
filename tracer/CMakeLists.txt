cmake_minimum_required(VERSION 4.1)

add_library(tracer)

target_sources(
	tracer

	PRIVATE
	    src/core.cpp

	PUBLIC
	    FILE_SET HEADERS
		BASE_DIRS
		    include
		FILES
		    include/tracer/common.hpp
		    include/tracer/core.hpp
)

target_compile_features(tracer PUBLIC cxx_std_23)
target_compile_options(tracer PRIVATE "${PT_WARNINGS}")

if(PT_WARNING_AS_ERROR)
    set_target_properties(tracer PROPERTIES COMPILE_WARNING_AS_ERROR TRUE)
endif()

target_link_libraries(tracer PUBLIC glm::glm)

add_library(PathTracer::tracer ALIAS tracer)
