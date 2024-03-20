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

# set(LINUX64 ON)
# if(LINUX64)
#     set(LIB_DIR "libs/x64")
# else()
#     set(LIB_DIR "libs/x86")
# endif()
# file(GLOB 3RD_LIB "${LIB_DIR}/*.so")
# message("3RD_LIB: ${3RD_LIB}")
# set(LIB_DIR "/home/lmx/head_ws/src/speak/libs/x64")

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(chat_msgs REQUIRED)

include_directories(include)

# 在链接第三方库libxxx.so时，在编译时找不到也会通过，但是在运行时会显示找不到它，
# 用ldd your_executable发现在install/package/lib/package下的executable，对于第三方库是not_found，
# 使用locate libxxx.so，可以找到他在电脑中的位置，然后直接把他cp到/usr/lib
# 在下面的target_link_libraries中，就可以用像link系统中的库时用xxx来link到libxxx.so，
# 不用考虑是不是路径问题，最简单解禁额问题，这次的问题相对路径和绝对路径等方法都不行
# link_directories(/home/lmx/head_ws/src/speak/libs/x64/)

set(executable_name ${PROJECT_NAME})
set(library_name ${executable_name}_core)

set(dependencies
  rclcpp
  std_msgs
  chat_msgs
)

add_library(${library_name}
  src/speak_offline/speak_offline.cpp
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
  # ${3RD_LIB}
  msc
  rt
  dl
  pthread 
  stdc++
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