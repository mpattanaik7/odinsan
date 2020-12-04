cmake_minimum_required(VERSION 3.14)
project(odinsan C CXX)
set(CMAKE_CXX_STANDARD 14)


add_subdirectory(lib/FFmpeg)
add_definitions(-DGL_SILENCE_DEPRECATION)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


list(APPEND SOURCES
    src/main.cpp
    src/video_reader.cpp
    src/video_reader.hpp
    src/Queue.cpp
    src/stream_server.cpp
    src/test_client.cpp
)

add_executable(odinsan MACOSX_BUNDLE WIN32 ${SOURCES})

if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL"
    )

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/assets/MacOSXBundleInfo.plist
    )

    set_target_properties(odinsan PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/assets/MacOSXBundleInfo.plist
    )

elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")
else()
    list(APPEND EXTRA_LIBS
        "-lGL -lGLU -lX11"
    )
endif()

target_link_libraries(odinsan FFmpeg ${CONAN_LIBS} ${EXTRA_LIBS})
