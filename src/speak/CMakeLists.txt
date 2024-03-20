# 参照cyberdog_ros2的CMakeLists.txt修改
cmake_minimum_required(VERSION 3.5)
project(speak)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(LINUX64 ON)
if(LINUX64)
    set(LIB_DIR "libs/x64")
else()
    set(LIB_DIR "libs/x86")
endif()
file(GLOB 3RD_LIB "${LIB_DIR}/*.so")
# message("3RD_LIB: ${3RD_LIB}")

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(chat_msgs REQUIRED)

include_directories(include)

set(executable_name ${PROJECT_NAME})
set(library_name ${executable_name}_core)

set(dependencies
  rclcpp
  std_msgs
  chat_msgs
)

add_library(${library_name}
  src/speak_offline.cpp
)

ament_target_dependencies(${library_name}
  ${dependencies}
)

add_executable(${executable_name} 
  src/speak.cpp
)

ament_target_dependencies(${executable_name} 
  ${dependencies}
)

target_link_libraries(${executable_name}
  ${library_name}
  ${3RD_LIB}
)

install(TARGETS ${library_name}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS ${executable_name}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch 
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY params/
  DESTINATION share/${PROJECT_NAME}/params/
)

install(DIRECTORY include/
  DESTINATION include/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${library_name})
ament_export_dependencies(${dependencies})

ament_package()
