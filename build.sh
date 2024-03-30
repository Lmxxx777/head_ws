#!/bin/bash

# 如果出现找不到msc库的问题
# locate /path/to/libmsc.so
# chmod +x libmsc.so
# sudo cp libmsc.so /usr/lib

colcon build --packages-select chat_msgs

colcon build

colcon build

source install/setup.bash