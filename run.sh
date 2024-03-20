#!/bin/bash

# gnome-terminal -- bash -c "source install/setup.bash; ros2 launch head_console head_console_launch.py; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "source install/setup.bash; ros2 launch listen listen_launch.py; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "source install/setup.bash; ros2 launch spark spark_launch.py; echo 'Press ENTER to exit...' && read"
# sleep 1
# gnome-terminal -- bash -c "source install/setup.bash; ros2 launch speak speak_launch.py; echo 'Press ENTER to exit...' && read"

gnome-terminal -- bash -c "source install/setup.bash; ros2 run head_console head_console; echo 'Press ENTER to exit...' && read"
sleep 1
gnome-terminal -- bash -c "source install/setup.bash; ros2 run listen listen; echo 'Press ENTER to exit...' && read"
sleep 1
gnome-terminal -- bash -c "source install/setup.bash; ros2 run spark spark; echo 'Press ENTER to exit...' && read"
sleep 1
gnome-terminal -- bash -c "source install/setup.bash; ros2 run speak speak; echo 'Press ENTER to exit...' && read"