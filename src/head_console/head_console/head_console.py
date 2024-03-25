import rclpy
import string
from rclpy.node import Node
from std_msgs.msg import Int8, String

class HeadConsole(Node):
    def __init__(self, name):
        super().__init__(name)
        self.publisher_head_console = self.create_publisher(Int8, 'head_console', 10)
        self.publisher_face_action = self.create_publisher(String, 'face_action', 10)
        self.timer = self.create_timer(1.0, self.timer_callback)
        self.get_logger().info('HeadConsole initialized')

    def timer_callback(self):
        self.get_logger().info('Enter a command: ')
        terminal_input = int(input())
        if terminal_input == 1:
            message = Int8()
            message.data = int(terminal_input)
            self.publisher_head_console.publish(message)
        elif terminal_input == 2:
            self.DIY_action()
        else:
            print("输入1 or 2")

    def DIY_action(self):
        print("***************************************************************************************************\n")
        print("**   0.初始化\n")
        print("**   1.单次眨眼  2.单纯摇头（否定）  3.单次点头（肯定）  4.皱眉摇头（强烈否定）  5.皱眉瞪眼（生气）\n")
        print("**   6.挑眉大眼（惊奇）  7.挑眉大眼张嘴（惊吓）  8. 摇头皱眉（困惑） 9. 东张希望 10. 思考反应（短时）\n")
        print("**   11.挤眉弄眼（调戏） 12.莞尔一笑     13.综合动作     14.收到     15.说话短\n")
        print("**   16.微表情一     17. 微表情二    18. 微表情三    19. 微表情四    20.随机微表情开始\n")
        print("**   21.单个表情演示 22. 开启表情跟随    23.随机眨眼 24.关闭表情跟随\n")
        print("***************************************************************************************************\n")
        
        choice = int(input("请输入对应的序号："))

        message = String()
        
        if choice == 0:
            message.data = "初始化"
        elif choice == 1:
            message.data = "单次眨眼"
        elif choice == 2:
            message.data = "单纯摇头（否定）"
        elif choice == 3:
            message.data = "单次点头（肯定）"
        elif choice == 4:
            message.data = "皱眉摇头（强烈否定）"
        elif choice == 5:
            message.data = "皱眉瞪眼（生气）"
        elif choice == 6:
            message.data = "挑眉大眼（惊奇）"
        elif choice == 7:
            message.data = "挑眉大眼张嘴（惊吓）"
        elif choice == 8:
            message.data = "摇头皱眉（困惑）"
        elif choice == 9:
            message.data = "东张希望"
        elif choice == 10:
            message.data = "思考反应（短时）"
        elif choice == 11:
            message.data = "挤眉弄眼（调戏）"
        elif choice == 12:
            message.data = "莞尔一笑"
        elif choice == 13:
            message.data = "综合动作"
        elif choice == 14:
            message.data = "收到"
        elif choice == 15:
            message.data = "说话短"
        elif choice == 16:
            message.data = "微表情一"
        elif choice == 17:
            message.data = "微表情二"
        elif choice == 18:
            message.data = "微表情三"
        elif choice == 19:
            message.data = "微表情四"
        elif choice == 20:
            message.data = "随机微表情开始"
        elif choice == 21:
            message.data = "单个表情演示"
        elif choice == 22:
            message.data = "开启表情跟随"
        elif choice == 23:
            message.data = "随机眨眼"
        elif choice == 24:
            message.data = "关闭表情跟随"
        else:
            return

        self.publisher_face_action.publish(message)
        self.get_logger().info(f'Published face_action message: {message.data}')  # 打印消息
        

def main(args=None):
    rclpy.init(args=args)
    node = HeadConsole('head_console')
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()