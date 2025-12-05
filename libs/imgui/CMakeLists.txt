cmake_minimum_required(VERSION 4.1)

add_library(imgui)

target_sources(
    imgui

	PRIVATE
	    imgui/imgui.cpp
	    imgui/imgui_demo.cpp
	    imgui/imgui_draw.cpp
	    imgui/imgui_tables.cpp
	    imgui/imgui_widgets.cpp
		imgui/backends/imgui_impl_glfw.cpp

	PUBLIC
	    FILE_SET HEADERS
		BASE_DIRS
		    imgui
		FILES
            imgui/imconfig.h
            imgui/imgui.h
            imgui/imgui_internal.h
            imgui/imstb_rectpack.h
            imgui/imstb_textedit.h
            imgui/imstb_truetype.h
            imgui/backends/imgui_impl_glfw.h
)

target_compile_features(imgui PUBLIC cxx_std_11)

target_link_libraries(imgui PUBLIC glfw)

add_library(PathTracer::imgui ALIAS imgui)
