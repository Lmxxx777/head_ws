from launch import LaunchDescription
from launch_ros.actions import Node
 
def generate_launch_description():
    return LaunchDescription([
        Node(
            package='head_console',
            executable='head_console',
            name='head_console',
            output='screen'
        ),
    ])