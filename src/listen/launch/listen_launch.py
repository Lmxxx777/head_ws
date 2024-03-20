from launch import LaunchDescription
from launch_ros.actions import Node
 
def generate_launch_description():
   return LaunchDescription([
      Node(
         package='listen',
         executable='listen_online',
         name='listen_online'
      ),
   ])