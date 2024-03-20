cmake_minimum_required(VERSION 3.5)
project(speak)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(DIR_INC ../include)
set(DIR_BIN ../bin)
set(DIR_LIB ../libs)

set(LINUX64 ON)
if(LINUX64)
    set(LDFLAGS "-L${DIR_LIB}/x64")
else()
    set(LDFLAGS "-L${DIR_LIB}/x86")
endif()
set(LDFLAGS "${LDFLAGS} -lmsc -lrt -ldl -lpthread -lstdc++")

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(chat_msgs REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

# add_library(${PROJECT_NAME}
#   ${PROJECT_SOURCE_DIR}/src/voice_offline/xtts_offline_sample.c
#   ${PROJECT_SOURCE_DIR}/src/voice_online/tts_online_sample.c
# )

# voice_offline
add_executable(voice_offline src/voice_offline/xtts_offline_sample.cpp)
ament_target_dependencies(voice_offline 
  rclcpp 
  rclcpp_components
  std_msgs
  chat_msgs
)
install(TARGETS voice_offline DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
