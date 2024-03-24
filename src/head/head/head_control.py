import time
import rclpy
from rclpy.node import Node

from std_msgs.msg import String
from servo_control import ServoCtrl 
from xml_read import XmlData , TimeTable


class RobotCtrlNode(Node ,ServoCtrl):
    
    def __init__(self, name, arg, *args, **kwargs):
        super().__init__(name)
        super(ServoCtrl, self).__init__(arg, *args, **kwargs)
        self.dataxml = XmlData()
        self.timetable = TimeTable()


        self.get_logger().info('start')
        self.sub = self.create_subscription(String, "face_action", self.action_callback, 10) # 解析动作指令 发布消息
        self.get_logger().info('action send over')
        print(self.dataxml.nameall)
    
    def action_callback(self, msg):
        print("get msg.data:",msg.data)
        find = 0
        for name in self.dataxml.nameall:
            if name == msg.data :
                self.get_logger().info('call back')
                self.Std_action(msg.data)    
                print("name:",name)
                break
            
        

    def Std_action(self,action):
        print('发送准备中\n...\n:',self.timetable.Timetable(action))
        time1 = 0 
        #计时器 发送 id 匹配到
        for Send_time in self.timetable.timeline :
            time1 = Send_time - time1
            print("时间节点:",Send_time)
            id_send = self.timetable.Timetable(action)[Send_time]
            self.send(id_send,self.xml_data)
            print("延时:",time1*14/1000)
            time.sleep(time1*14/1000)
        print("send_all_finish")

        
def main(args=None):
    rclpy.init(args=args)
    # moto_add = "/home/yangqi/face/eerobot_ws/src/robot_ctrl/robot_control/robot_control/motoSet.xml"
    # act_add = "/home/yangqi/face/eerobot_ws/src/robot_ctrl/robot_control/moto/"

    node = RobotCtrlNode("head_control" , '/dev/ttyUSB0', 921600, 8, 'N', 1)
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


