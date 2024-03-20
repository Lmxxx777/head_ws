from launch import LaunchDescription
from launch_ros.actions import Node
 
def generate_launch_description():
    return LaunchDescription([
        Node(
            package='spark',
            executable='spark',
            name='spark',
            output='screen'
        ),
    ])