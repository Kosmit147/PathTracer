cmake_minimum_required(VERSION 4.1)

add_library(pcg INTERFACE)

target_sources(
    pcg

	INTERFACE
	    FILE_SET HEADERS
		BASE_DIRS
		    pcg-cpp/include
		FILES
            pcg-cpp/include/pcg_extras.hpp
            pcg-cpp/include/pcg_random.hpp
            pcg-cpp/include/pcg_uint128.hpp
)

target_compile_features(pcg INTERFACE cxx_std_11)

add_library(PathTracer::pcg ALIAS pcg)
