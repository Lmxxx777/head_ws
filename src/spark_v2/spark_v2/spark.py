from spark_v2.sparkAPI import SparkChat
import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int8
from chat_msgs.msg import Content


class SparkNode(Node):
    
    def __init__(self, name):
        super().__init__(name)
        self.start_listen = 0
        self.listen_sub = self.create_subscription(Content, 'listen', self.Listen_Callback, 10)
        self.spark_pub = self.create_publisher(Content, "spark", 10)
        self.publisher_gTTS = self.create_publisher(String, 'gTTS', 10)

        self.get_logger().info('Spark initialized successfully......')

    def Listen_Callback(self, msg):
        print("Question: ", msg.content)
        answer = SparkChat(msg.content)

        # pub_msg = Content()
        # pub_msg.content = answer
        # pub_msg.length = len(answer)  
        # print("Answer: ", answer)
        # self.spark_pub.publish(pub_msg)

        message = String()
        # message.data = answer.replace('\n', '').replace('\r', '').replace(' ', '')
        # message.data = '深圳市明天（2024-03-31）的天气将是多云，气温较热，相对湿度较高，风力微弱，空气质量为优。具体的天气情况如下：明天的最高气温预计为30℃，最低气温为25℃。整日将出现多云的天气状况，由于天气较热，市民在外出时建议穿着轻薄的衣物。空气的相对湿度达到87%，体感可能会有些湿润。风力情况是北风微风，对日常活动的影响不大。由于空气质量属于优等级，户外活动无需过多考虑空气污染问题。此外，根据穿衣指数，建议选择精干简洁的衣物，室内则可以适当添加空调衫以保持舒适。钓鱼指数显示不适宜垂钓，建议选择其他娱乐活动。同时，运动指数提示明天较不适宜户外运动，可以考虑进行室内运动。日出和日落时间分别为06:17和18:39。'
        message.data = "深圳市"
        self.publisher_gTTS.publish(message)
        print("answer publish......")
    

def main(args=None):
    rclpy.init(args=args)
    node = SparkNode("spark")
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    # content = ListenContent()
    main()