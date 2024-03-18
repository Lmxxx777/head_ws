from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    #voice
    aiui_interaction = Node(
        package="voiceinteraction",
        executable="aiui_interaction"
        )
    voice_control = Node(
        package="voiceinteraction",
        executable="voice_control"
        )
    vitsrosser = Node(
        package="vitsros",
        executable="vitsrosser"
        )
    launch_description = LaunchDescription([aiui_interaction,vitsrosser]) #,random_blink,voice_control
    return launch_description