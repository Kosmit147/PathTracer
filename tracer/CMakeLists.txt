cmake_minimum_required(VERSION 4.1)

add_library(tracer)

target_sources(
    tracer

    PRIVATE
        src/object.cpp
        src/random.cpp
        src/software_renderer.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/tracer/assert.hpp
            include/tracer/common.hpp
            include/tracer/geometric.hpp
            include/tracer/numeric.hpp
            include/tracer/object.hpp
            include/tracer/random.hpp
            include/tracer/ray.hpp
            include/tracer/software_renderer.hpp
            include/tracer/trigonometric.hpp
)

target_compile_features(tracer PUBLIC cxx_std_23)
target_compile_options(tracer PRIVATE "${PT_COMPILE_FLAGS}")

if(PT_WARNING_AS_ERROR)
    set_target_properties(tracer PROPERTIES COMPILE_WARNING_AS_ERROR TRUE)
endif()

if(PT_ASSERTS)
    target_compile_definitions(tracer PUBLIC PT_ASSERTS)
endif()

if(PT_DEBUG_BREAKS)
    target_compile_definitions(tracer PUBLIC PT_DEBUG_BREAKS)
endif()

target_link_libraries(tracer PUBLIC glm::glm)
target_link_libraries(tracer PUBLIC PathTracer::pcg)

add_library(PathTracer::tracer ALIAS tracer)
