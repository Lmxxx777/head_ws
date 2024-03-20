# 语音听写（流式版）60s时长 节点文件

import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int8
# import iat_ws_python3
from listen.iat_ws_python3 import ListenContent
from chat_msgs.msg import Content

# def ListenContent():
#     '''
#     执行听写操作
#     '''
#     listen_content = "你好，睿娜。"
#     return listen_content

class ListenNode(Node):
    
    def __init__(self, name):
        super().__init__(name)
        self.start_listen = 0
        self.head_console_sub = self.create_subscription(Int8, 'head_console', self.Command_Callback, 10)
        self.content_pub = self.create_publisher(Content, "listen", 10)
        # self.timer = self.create_timer(0.5, self.Command_Callback)
        self.get_logger().info('ListenNode initialized successfully......')

    def Command_Callback(self, msg):
        self.get_logger().info('Command: "%s"' % msg.data)
        self.start_listen = msg.data
        if self.start_listen == 1 :
            pub_msg = Content()
            content = ListenContent()
            pub_msg.content = content
            pub_msg.length = len(content)
            self.get_logger().info('listen content: "%s"' % content)
            self.content_pub.publish(pub_msg)
            self.start_listen = 0


def main(args=None):
    rclpy.init(args=args)
    node = ListenNode("listen")
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    # content = ListenContent()
    main()