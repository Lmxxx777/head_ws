cmake_minimum_required(VERSION 3.5)
project(voiceinteraction)

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

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(user_interfaces REQUIRED)

# 声明三方库位置路径
link_directories(libs/x64)

include_directories(
  src
  include
  include/hid
  src/jsoncpp
  src/aiui_new_mic/inc
  # /usr/include/json-c                      
  # /usr/lib/libusb/include/libusb-1.0
  )

# 生成可执行文件
add_executable(tts_subscribe src/tts_subscribe.cpp)
target_link_libraries(
                tts_subscribe
                libmsc.so -ldl -pthread
                )
ament_target_dependencies(tts_subscribe
                rclcpp
                std_msgs
                )

add_executable(iat_publish 
                src/iat_publish.cpp 
                src/speech_recognizer.c 
                src/linuxrec.c
                )
target_link_libraries(
                iat_publish
                libmsc.so -ldl -lpthread -lm -lrt -lasound
                )
ament_target_dependencies(iat_publish
                rclcpp
                std_msgs
                )

add_executable(voice_control src/voice_control.cpp)
target_link_libraries(
                voice_control
                libmsc.so -ldl -pthread
                )
ament_target_dependencies(voice_control
                rclcpp
                geometry_msgs
                user_interfaces
                std_msgs
                )

add_executable(aiui_interaction 
                src/jsoncpp/json_reader.cpp
                src/jsoncpp/json_value.cpp
                src/jsoncpp/json_writer.cpp
                src/MyListener.cpp
                src/AudioPlayer.cpp
                src/AudioRecorder.cpp                
                src/FileUtil.cpp
                src/aiui_interaction.cpp 
                )
target_link_libraries(
                aiui_interaction
                aiui
                asound
                hid_lib
                msc
                pthread
                )
ament_target_dependencies(aiui_interaction
                rclcpp
                geometry_msgs
                std_msgs
                user_interfaces
                )

# add_executable(aiui_new_mic 
#                 src/jsoncpp/json_reader.cpp
#                 src/jsoncpp/json_value.cpp
#                 src/jsoncpp/json_writer.cpp
#                 src/AudioPlayer.cpp   
#                 src/aiui_new_mic/my_aiui.cpp
#                 src/aiui_new_mic/mic_com.c
#                 src/aiui_new_mic/aiui_new_mic.cpp 
#                 )
# target_link_libraries(
#                 aiui_new_mic
#                 aiui
#                 asound
#                 cjson
#                 )
# ament_target_dependencies(aiui_new_mic
#                 rclcpp
#                 geometry_msgs
#                 std_msgs
#                 user_interfaces
#                 )

install(TARGETS tts_subscribe DESTINATION lib/${PROJECT_NAME})
install(TARGETS iat_publish DESTINATION lib/${PROJECT_NAME})
install(TARGETS voice_control DESTINATION lib/${PROJECT_NAME})
install(TARGETS aiui_interaction DESTINATION lib/${PROJECT_NAME})
# install(TARGETS aiui_new_mic DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ./libs/x64 DESTINATION share/${PROJECT_NAME})
install(DIRECTORY ./configs DESTINATION ../../)
install(DIRECTORY ./AIUI DESTINATION ../../)

install(DIRECTORY launch
DESTINATION share/${PROJECT_NAME})

ament_package()

