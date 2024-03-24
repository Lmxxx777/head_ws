from numpy import rate
import rclpy
from rclpy.node import Node
from std_msgs.msg import String , Int32
from xml_read import XmlData , TimeTable
from chat_msgs.msg import Servo
from chat_msgs.msg import State
import sounddevice as sd
import threading
import sys
import time


class TalkSubscriber(Node):
    def __init__(self, name):
        super().__init__(name)                                  # ROS2节点父类初始化
        self.i = 0
        self.dataxml = XmlData()
        self.xml_data = self.dataxml.xml_moto()
        self.acts_data = self.dataxml.xml_act()
        self.timetable = TimeTable()

        self.talk_sub = self.create_subscription(String, 'test', self.talk_callback, 10) # 订阅 语音
        self.pub = self.create_publisher(Servo, "robot_state", 10) # 解析动作指令 发布消息
        self.pub_speak = self.create_publisher(State,"speak_state",10) #扬声器说话指令
        self.fs = 16000 # Hz
        self.time = 0.05 # seconds　　
        timer_period = 0.02  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        
        self.nameE = {11 :'left_blink' , 12 :'left_smile' , 13 :'left_eye_erect' , 14 :'left_eye_level' , 44 :'left_eyebrow' ,
                24 :'right_blink' , 23 :'right_smile' , 22 :'right_eye_erect' , 21 :'right_eye_level' , 31 :'right_eyebrow' ,
                43 :'head_dian'   , 42 :'head_yao'    , 33 :'head_bai'        ,
                32 :'mouth'  }
        self.talk_flag = 1
        self.data_old = 0
        self.min_in = 0
        self.max_in = 0
        self.pos_old = 0
        self.speak_i = 0
        self.send_flag = Int32()
        # self.myrecording = sd.rec(int(self.time * self.fs), samplerate=self.fs, channels=1)
        self.myrecording = [0]
        self.get_logger().info('talk_mouth node init')
        # self.thread1 = threading.Thread(name='t1', target = self.sound_thread)
        # self.thread1.start()

        self.stream = sd.InputStream(channels=1, samplerate=16000, callback=self.streamCallback)
        self.stream.start()

    def talk_callback(self, msg):
        self.get_logger().info('Receiving status1')   # 输出日志信息，提示已进入回调函数
        if msg.data == 'speak_flag' :
            self.talk_flag = 1
            self.get_logger().info('Receiving self.talk_flag = 1') 
        # else :
        #     self.talk_flag = 0

    def timer_callback(self):
        # self.i = self.i+1
        # print("i:%d\n"%self.i)
        servos = Servo()
        state = State()
        tim = 0#ms
        if self.talk_flag == 1 :
            # myrecording = sd.rec(int(self.time * self.fs), samplerate=self.fs, channels=1)
            # sd.wait()c
            max_prsent = 0.95 
            min_prsent = 0.05
            data = abs(self.myrecording[-1]) 
            # print(data)
            if self.data_old < data : 
                self.max_in = data * max_prsent
                self.min_in = data * min_prsent
                self.data_old = data
                print('self.data_max',self.data_old)
                # sd.wait()
            # print('self.max_in',self.max_in)
            if data == 0 :
                pos = 66
                self.speak_i = self.speak_i + 1
            elif data < self.min_in :
                pos = 70
            elif data > self.min_in and data < self.max_in :
                pos = 70 + int(data*(50/(self.max_in-self.min_in))*1.5)
            elif data > self.max_in and data < self.max_in*1.1:
                pos = 120
            else :
                pos = 65
                print("no get")
            
            if pos == self.pos_old :
                pass
            else :
                servos.mouth.extend([int(pos), int(tim)])
                # setattr(servos, 'mouth', [pos, tim])
                time.sleep(0.1)
                self.pub.publish(servos)
                self.pos_old = pos
            # del myrecording
            #扬声器结束&开始
            if self.speak_i >= 25 :
                self.speak_i = 0
                if self.send_flag.data != 0:
                    state.speak_flag = 0
                    self.send_flag.data = 0
                    self.pub_speak.publish(state)
            if data != 0 and self.send_flag.data != 1:
                self.speak_i = 0
                self.send_flag.data = 1
                state.speak_flag = 1
                self.pub_speak.publish(state)
                
            # print('self.speak_i',self.speak_i)
            # print("self.send_flag",self.send_flag.data)

            
    #线程（暂时不用）
    def sound_thread(self):
        while True:
            self.myrecording = sd.rec(int(self.time * self.fs), samplerate=self.fs, channels=1)
            sd.wait()
            sd.stop()
    #录音设备回调函数
    def streamCallback(self, indata, frames, time, status):
        if status:
            print(status, file=sys.stderr)
        # Fancy indexing with mapping creates a (necessary!) copy:
        self.myrecording = indata[-1]


# if self.talk_flag == 1 :
#             self.get_logger().info('Receiving self.talk_flag = 1') 
#             myrecording = sd.rec(int(self.time * self.fs), samplerate=self.fs, channels=1)
#             # sd.wait()c
#             data = abs(myrecording[-1]) 
#             if data < 0.05 :
#                 pos = 65
#             elif data > 0.05 and data < 0.55 :
#                 pos = 70 + (data -0.1) * 600
#             elif data > 0.55 and data < 0.7:
#                 pos = 120
#             else :
#                 pos = 65
#                 print("no get")
def main(args=None):
    rclpy.init(args=args)
    node = TalkSubscriber("facial_mouth")
    # rate = node.create_rate(1000)
    rclpy.spin(node)
    # while(rclpy.ok):
    #     rclpy.spin_once(node)
        #rate.sleep()
    node.destroy_node()
    rclpy.shutdown()
