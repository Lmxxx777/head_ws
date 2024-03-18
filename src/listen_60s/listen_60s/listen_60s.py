import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int8

# from listen_60s.iat_ws_python3 import ListenContent


def ListenContent():
    '''
    执行听写操作
    '''
    listen_content = "你好，睿娜。"
    return listen_content

class Listen60s(Node):
    
    def __init__(self, name):
        super().__init__(name)
        self.start_listen = 0
        self.head_console_sub = self.create_subscription(Int8, 'head_console', self.Command_Callback, 10) # 订阅 语音
        self.content_pub = self.create_publisher(String, "content", 2)    # 创建发布者对象（消息类型、话题名、队列长度）
        # self.timer = self.create_timer(0.5, self.Command_Callback)
        self.get_logger().info('Listen60s initialized')

    def Command_Callback(self, msg):
        print(msg)
        self.start_listen = msg.data
        if self.start_listen == 1 :
            pub_msg = String()
            content = ListenContent()
            pub_msg.data = content
            # self.publisher.publish(pub_msg)
            self.start_listen = 0
            self.get_logger().info('Receiving self.start_listen = 1') 


def main(args=None):
    rclpy.init(args=args)
    node = Listen60s("content_pub")
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    # content = ListenContent()
    main()