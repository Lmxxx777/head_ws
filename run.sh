#!/bin/bash

# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run head_console head_console; echo 'Press ENTER to exit...' && read"
# sleep 1
gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run listen_v2 listen_v2; echo 'Press ENTER to exit...' && read"
sleep 1
gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run spark_v2 spark_v2; echo 'Press ENTER to exit...' && read"
sleep 1
# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run speak speak; echo 'Press ENTER to exit...' && read"
# sleep 1 
gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run face_detection face_detection; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run head facial_control; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run head facial_action; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run voiceinteraction voice_control; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "export ROS_DOMAIN_ID=5; source install/setup.bash; ros2 run voiceinteraction aiui_interaction; echo 'Press ENTER to exit...' && read"