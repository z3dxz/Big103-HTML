cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(big103html VERSION 2.0)

find_package(SFML COMPONENTS network audio graphics window system REQUIRED)
find_package(CURL REQUIRED)

configure_file(res/CascadiaCode.ttf font.ttf COPYONLY)
configure_file(res/babafixed.wav babafixed.wav COPYONLY)
configure_file(res/sourceURL.txt sourceURL.txt COPYONLY)
configure_file(res/webhookURL.txt webhookURL.txt COPYONLY)

if (WIN32)

file(GLOB_RECURSE SRC_FILES src/*.cpp "res/Icon.rc")
add_executable(big103html ${SRC_FILES})
target_link_libraries(big103html dwmapi)

else()

find_package(X11 REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(big103html ${SRC_FILES})
target_link_libraries(big103html ${X11_LIBRARIES})

endif()
target_link_libraries(big103html sfml-network sfml-audio sfml-graphics sfml-window sfml-system CURL::libcurl)