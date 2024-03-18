# 语音交互节点使用说明
## 目录结构及文件

```
include：所需头文件路径
libs：
|-- x86
	|-- libmsc.so  // 32位动态库
|-- x64
	|-- libmsc.so  // 64位动态库
src:
|-- iat_publish.cpp   // 语音识别节点
|-- tts_subscribe.cpp // 文本转语音节点
|-- voice_control.cpp // 控制输出节点
.gitignore：git 忽略文件
CMakeLists.txt
package.xml
```
### 简易信息流描述
语音识别节点->控制输出节点->文本转语音节点
1. 语音识别节点，识别用户交互指令，发布指令文本信息；
2. 控制输出节点，订阅语音识别节点指令，发布cmd_vel以及执行结果文本信息；
3. 文本转语音节点，订阅控制输出节点的执行结果文本信息并合成语音输出；
4. 机器人控制节点，订阅控制输出节点的cmd_vel标准信息，执行前进、后退、左转、右转、停止等控制指令
## 编译测试方法
### 删除历史编译文件
rm -fr build/ install/ log/ msc/ tts_sample.wav
### 选择编译voiceinteraction包
colcon build --packages-select voiceinteraction

### 设置环境生效
cd /home/padroid/ros2_ws/
source install/setup.bash

### tts_subscribe节点测试消息
```
ros2 topic pub /tts_text std_msgs/String "data: '你好，我是机器人优拉'"
ros2 topic pub /tts_text std_msgs/String "data: '你好，我是机器人ulaa'"
ros2 topic pub /tts_text std_msgs/String "data: '前进'"
ros2 topic pub /tts_text std_msgs/String "data: '后退'"
ros2 topic pub /tts_text std_msgs/String "data: '左转'"
ros2 topic pub /tts_text std_msgs/String "data: '右转'"
ros2 topic pub /tts_text std_msgs/String "data: '停止'"
```

### 运行节点命令
```
ros2 run voiceinteraction iat_publish
ros2 run voiceinteraction tts_subscribe
ros2 run voiceinteraction voice_control
. install/setup.bash 
ros2 run voiceinteraction aiui_interaction
```