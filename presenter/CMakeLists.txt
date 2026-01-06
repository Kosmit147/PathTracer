cmake_minimum_required(VERSION 4.1)

add_executable(presenter)

target_sources(
    presenter

    PRIVATE
        src/main.cpp
        src/render_worker.cpp

    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
            src
        FILES
            src/assert.hpp
            src/common.hpp
            src/log.hpp
            src/render_worker.hpp
            src/timer.hpp
            src/ui.hpp
)

target_compile_features(presenter PRIVATE cxx_std_23)
target_compile_options(presenter PRIVATE "${PT_COMPILE_FLAGS}")

if(PT_WARNING_AS_ERROR)
    set_target_properties(presenter PROPERTIES COMPILE_WARNING_AS_ERROR TRUE)
endif()

if(PT_ASSERTS)
    target_compile_definitions(presenter PRIVATE PT_ASSERTS)
endif()

if(PT_OPENGL_DEBUG_CONTEXT)
    target_compile_definitions(presenter PRIVATE PT_OPENGL_DEBUG_CONTEXT)
endif()

if(PT_DEBUG_BREAKS)
    target_compile_definitions(presenter PRIVATE PT_DEBUG_BREAKS)
endif()

target_link_libraries(presenter PRIVATE glad::glad)
target_link_libraries(presenter PRIVATE glfw)
target_link_libraries(presenter PRIVATE glm::glm)
target_link_libraries(presenter PRIVATE PathTracer::imgui)
target_link_libraries(presenter PRIVATE portable-file-dialogs)
target_link_libraries(presenter PRIVATE spdlog::spdlog)
target_link_libraries(presenter PRIVATE PathTracer::stb)

target_link_libraries(presenter PRIVATE PathTracer::tracer)

add_executable(PathTracer::presenter ALIAS presenter)
