import os
import pathlib
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
import time

def generate_launch_description():
    PACKAGE_NAME = 'ulaa'
    ulaahead_dir = get_package_share_directory('ulaahead')
    ulaahead_launch_dir = os.path.join(ulaahead_dir, 'launch')

    ulaabody_dir = get_package_share_directory('voiceinteraction')
    ulaabody_launch_dir = os.path.join(ulaabody_dir, 'launch')

    ros2_bridge_py_dir = get_package_share_directory('ros2_bridge_py')
    ros2_bridge_py_launch_dir = os.path.join(ros2_bridge_py_dir, 'launch')

    # camera_dir = get_package_share_directory('Camera')
    # camera_launch_dir = os.path.join(camera_dir, 'launch')

    return  LaunchDescription([
    
	IncludeLaunchDescription(
            PythonLaunchDescriptionSource([ros2_bridge_py_launch_dir, '/ulaa_wb.launch.py'])
        ),

        IncludeLaunchDescription(
            # PythonLaunchDescriptionSource([ulaabody_launch_dir, '/ulaa_voice.launch.py'])
            PythonLaunchDescriptionSource([ulaabody_launch_dir, '/ulaa_voice_vits.launch.py'])
        ),
        
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([ulaahead_launch_dir, '/ulaa_head.launch.py'])
        ),

        # IncludeLaunchDescription(
        #      PythonLaunchDescriptionSource([camera_launch_dir, '/camera.launch.py'])
        #  ),





    ])
