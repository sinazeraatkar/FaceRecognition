# cmake_minimum_required(VERSION 3.0)
# project(video-streaming-app)

# # Find GStreamer packages
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

# # Include directories
# include_directories(${GSTREAMER_INCLUDE_DIRS})

# # Source files
# set(SRC_FILES src/video_stream.c)

# # Executable
# add_executable(video_stream ${SRC_FILES})

# # Linking
# target_link_libraries(video_stream
#     ${GSTREAMER_LIBRARIES})
# cmake_minimum_required(VERSION 3.0)
# project(video-streaming-app)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# include_directories(${GSTREAMER_INCLUDE_DIRS})

# set(SRC_FILES src/video_stream.c)

# add_executable(video_stream ${SRC_FILES})

# target_link_libraries(video_stream ${GSTREAMER_LIBRARIES})
# cmake_minimum_required(VERSION 3.0)
# project(video-streaming-app)

# # Find GStreamer packages
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

# # Include directories
# include_directories(${GSTREAMER_INCLUDE_DIRS})

# # Source files
# set(SRC_FILES src/video_stream.c)

# # Executable
# add_executable(video_stream ${SRC_FILES})

# # Linking
# target_link_libraries(video_stream ${GSTREAMER_LIBRARIES})
cmake_minimum_required(VERSION 3.0)
project(video_stream)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

set(SOURCES src/video_stream.c)

add_executable(video_stream ${SOURCES})

target_include_directories(video_stream PRIVATE ${GST_INCLUDE_DIRS})
target_compile_options(video_stream PRIVATE ${GST_CFLAGS_OTHER})
target_link_libraries(video_stream ${GST_LIBRARIES})
