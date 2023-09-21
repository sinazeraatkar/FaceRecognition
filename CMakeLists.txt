# cmake_minimum_required(VERSION 3.0)
# project(FaceRecognition)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# find_package(OpenCV REQUIRED)
# find_package(Threads REQUIRED)

# include_directories(${PROJECT_SOURCE_DIR}/include/) 
# include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(${MySQL_INCULDE_DIRS})

# configure_file("${CMAKE_SOURCE_DIR}/scripts/audio.sh" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/audio.sh")
# configure_file("${CMAKE_SOURCE_DIR}/haarcascade_frontalface_alt.xml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/haarcascade_frontalface_alt.xml")

# set(APP_SOURCES1 src/http_server.cpp)
# set(APP_SOURCES2 src/mqtt_server.cpp)

# add_executable(http_server ${APP_SOURCES1})
# target_link_libraries(http_server ${CMAKE_THREAD_LIBS_INIT} -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm)

# add_executable(mqtt_server ${APP_SOURCES2})
# target_link_libraries(mqtt_server ${CMAKE_THREAD_LIBS_INIT} -lpaho-mqtt3c -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm ${OpenCV_LIBS})

# INSTALL(TARGETS http_server mqtt_server DESTINATION bin)
cmake_minimum_required(VERSION 3.0)
project(FaceRecognition)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include/) 
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/include/mysql) # Add this line with the correct path

configure_file("${CMAKE_SOURCE_DIR}/scripts/audio.sh" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/audio.sh")
configure_file("${CMAKE_SOURCE_DIR}/haarcascade_frontalface_alt.xml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/haarcascade_frontalface_alt.xml")

set(APP_SOURCES1 src/http_server.cpp)
set(APP_SOURCES2 src/mqtt_server.cpp)

add_executable(http_server ${APP_SOURCES1})
target_link_libraries(http_server ${CMAKE_THREAD_LIBS_INIT} -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm)

add_executable(mqtt_server ${APP_SOURCES2})
target_link_libraries(mqtt_server ${CMAKE_THREAD_LIBS_INIT} -lpaho-mqtt3c -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm ${OpenCV_LIBS})

INSTALL(TARGETS http_server mqtt_server DESTINATION bin)

