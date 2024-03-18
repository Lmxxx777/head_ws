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
    launch_description = LaunchDescription([aiui_interaction, voice_control]) #,random_blink,voice_control
    return launch_description