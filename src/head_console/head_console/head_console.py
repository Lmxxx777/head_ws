import rclpy
from rclpy.node import Node
from std_msgs.msg import Int8

class HeadConsole(Node):
    def __init__(self, name):
        super().__init__(name)
        self.publisher = self.create_publisher(Int8, 'head_console', 10)
        self.timer = self.create_timer(1.0, self.timer_callback)

        
        self.get_logger().info('HeadConsole initialized')

    def timer_callback(self):
        self.get_logger().info('Enter a command: ')
        terminal_input = input()
        if terminal_input:
            message = Int8()
            message.data = int(terminal_input)
            self.publisher.publish(message)

def main(args=None):
    rclpy.init(args=args)
    node = HeadConsole('head_console')
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()