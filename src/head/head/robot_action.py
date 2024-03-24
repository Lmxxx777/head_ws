
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import time
from chat_msgs.msg import Servo
from xml_read import XmlData , TimeTable
import random


class ActionSubscriber(Node):
    def __init__(self, name):
        super().__init__(name) # ROS2节点父类初始化
        self.dataxml = XmlData()
        self.xml_data = self.dataxml.xml_moto()
        self.acts_data = self.dataxml.xml_act()
        self.timetable = TimeTable()

        self.sub_status = self.create_subscription(String, 'face_ctrl', self.status_callback, 10) # 订阅　机器人当前状态
        self.sub = self.create_subscription(String, 'face_action', self.action_callback, 10) # 订阅std 各种表情测试节点
        self.pub = self.create_publisher(Servo, "robot_state", 10) # 解析动作指令 发布消息
        timer_period = 8 # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)        

        self.nameE = {11 :'left_blink' , 12 :'left_smile' , 13 :'left_eye_erect' , 14 :'left_eye_level' , 44 :'left_eyebrow' ,
                24 :'right_blink' , 23 :'right_smile' , 22 :'right_eye_erect' , 21 :'right_eye_level' , 31 :'right_eyebrow' ,
                43 :'head_dian'   , 42 :'head_yao'    , 33 :'head_bai'        ,
                32 :'mouth'  }
        self.expr_flag = 0

        self.get_logger().info('demo_action node init')
        # self.get_logger().info(self.dataxml.nameall)


    def status_callback(self, msg):
        self.get_logger().info('Receiving status2')   # 输出日志信息，提示已进入回调函数        
        if '开启' in msg.data:
            self.expr_flag = 1
        else:
            self.expr_flag = 0
        print('self.expr_flag',self.expr_flag)

    def action_callback(self, msg):
        print("get msg.data:",msg.data)
        for name in self.dataxml.nameall:
            if name == msg.data :
                # self.get_logger().info('call back')
                self.Std_action(msg.data)    
                # print("name:",name)
                break

    def Std_action(self, action):
        # self.get_logger().info('Receiving action')   # 输出日志信息，提示已进入回调函数
        servos = Servo()
        # print('发送准备中\n...\n:',self.timetable.Timetable(action))
        self.timetable.Timetable(action)
        time1 = 0 
        timeold = 0
        #计时器 发送 id 匹配到
        for Send_time in self.timetable.timeline :
            time1 = Send_time - timeold
            if time1 != 0 :
                # print("时间节点:",Send_time)
                id_send = self.timetable.Timetable(action)[Send_time]#发送同一时刻的id动作和时间
                for id in id_send :
                    name = self.nameE[id]
                    pos = int(id_send[id]['pos'])
                    tim = int(id_send[id]['tim']) *20
                    setattr(servos, name, [pos, tim])
                self.pub.publish(servos)
            # print("tim:",time1)
            # print("Send_time:",Send_time)
            # print("time1*10/1000:",time1*18/1000)
            time.sleep(time1*18/1000)
            timeold = Send_time
        # print(action)
        print("send_all_finish")   

    def timer_callback(self):
        if self.expr_flag == 1 :    
            num = random.randint(0, 2)
            randact = ['初始化','微表情一','微表情二']#,'微表情三','微表情四'
            print(randact[num])
            self.Std_action(randact[num]) 
        elif self.expr_flag == 0 :
            pass

def main(args=None):
    rclpy.init(args=args)
    node = ActionSubscriber("facial_expression")

    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
