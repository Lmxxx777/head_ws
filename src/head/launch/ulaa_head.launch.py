from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    facial_features = Node(
        package="ulaahead",
        executable="facial_features"
        )
    facial_expression = Node(
        package="ulaahead",
        executable="facial_expression"
        )
    facial_mouth = Node(
        package="ulaahead",
        executable="facial_mouth"
        )
    random_blink = Node(
        package="ulaacontrol",
        executable="random_blink"
        )

    launch_description = LaunchDescription([facial_features,facial_expression,facial_mouth]) #,random_blink ,facial_mouth
    return launch_description
