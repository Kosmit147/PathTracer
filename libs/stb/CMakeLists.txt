cmake_minimum_required(VERSION 4.1)

add_library(stb)

target_sources(
    stb

	PRIVATE
	    src/stb_image.c
		src/stb_image_write.c

	PUBLIC
	    FILE_SET HEADERS
		BASE_DIRS
		    stb
		FILES
            stb/stb_image.h
            stb/stb_image_write.h
)

target_compile_features(stb PUBLIC c_std_11)

add_library(PathTracer::stb ALIAS stb)
