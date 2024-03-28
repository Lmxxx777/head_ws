import rclpy
from rclpy.node import Node
from std_msgs.msg import Int8
from chat_msgs.msg import Face
import time
import cv2
import os
from face_detection.xml_read import XmlData
from face_detection.face_dlib import face_db


class LookNode(Node):
    def __init__(self):
        super().__init__('face_detection')   
        self.face_db = face_db('/home/imi/head_ws/src/face_detection/face_detection/shape_predictor_68_face_landmarks.dat')

        self.cap = cv2.VideoCapture(0) #设备号为0
        self.cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M','J','P','G'))
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 480)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

        self.last = None
        self.next = None
        self.time = 0

        self.dataxml = XmlData()
        self.xml_data = self.dataxml.xml_moto()

        self.face_motor = [91, 91, 91, 91, 91,
                           # left_blink, left_smile, left_eye_erect, left_eye_level, left_eyebrow
                           91, 91, 91, 91, 91,
                           #right_blink, right_smile, right_eye_erect, right_eye_level, right_eyebrow
                           91, 91, 91,
                           # head_dian, head_yao, head_bai
                           66]
                           # mouth         
        self.std_length_ratio = 1
        self.period = 5
        self.publish_cnt = 1

        self.sub = self.create_subscription(Int8, 'copy_action', self.copy_action_callback, 10)
        self.pub = self.create_publisher(Face, 'face_state', 10)
        self.get_logger().info('LookNode initialized successfully......')

    

    def copy_action_callback(self, msg):
        if msg.data == 3:
            print("开始模仿。。。。。。")
            # 开启模仿，先初始化，然后打开回调函数
            self.publish_cnt = 1
            self.initial_reset()

            # 识别关键点，作出反应，发送消息
            self.face_recognation_function()
            # 测试
            # self.test()

            # 如果在运行途中关闭显示窗口，需要退出后复位
            self.initial_reset()
        elif msg.data == 4:
            # 停止模仿，先停止，再归位
            print("停止模仿。。。。。。")
            self.last = None
            self.next = None
            self.time = 0
            self.cap.release()
            cv2.destroyAllWindows()
            self.initial_reset()


    # 输入的电机位置，配上固定帧率的时间，看样子是以ms为单位的
    def publish_robot_state(self, face_action):

        msg = Face()

        msg.left_blink       = int(face_action[0])# 左眨眼
        msg.left_smile       = int(face_action[1])# 左微笑
        msg.left_eye_erect   = int(face_action[2])# 左眼竖
        msg.left_eye_level   = int(face_action[3])# 左眼平

        msg.right_blink      = int(face_action[5])# 右眨眼
        msg.right_smile      = int(face_action[6])# 右微笑
        msg.right_eye_erect  = int(face_action[7])# 右眼竖
        msg.right_eye_level  = int(face_action[8])# 右眼平

        msg.head_dian        = int(face_action[10])# 点头
        msg.head_yao         = int(face_action[11])# 摇头
        msg.head_bai         = int(face_action[12])# 摆头

        # 需要修改的量
        msg.left_eyebrow     = int(face_action[4])
        msg.right_eyebrow    = int(face_action[9])
        msg.mouth            = int(face_action[13])

        msg.period           = self.period
        msg.time_cnt         = self.publish_cnt
        
        self.publish_cnt = self.publish_cnt + 1

        print("msg : ", msg)
        self.pub.publish(msg)

    def initial_reset(self):
        self.face_motor = [91, 91, 91, 91, 91,
                # left_blink, left_smile, left_eye_erect, left_eye_level, left_eyebrow
                91, 91, 91, 91, 91,
                #right_blink, right_smile, right_eye_erect, right_eye_level, right_eyebrow
                91, 91, 91,
                # head_dian, head_yao, head_bai
                66]
                # mouth 
        self.publish_robot_state(self.face_motor)# print("初始化：\n", msgs)
        time.sleep(self.period / 1000)


    def face_recognation_function(self):
        while(True):

            if self.publish_cnt > 65530:
                break

            start_time = time.time()

            if self.cap.isOpened() == False:
                print('can not open camera')
                break
            #读取图像
            ret, img = self.cap.read()
            #图像读取失败则直接进入下一次循环
            if ret == False:
                continue

            #判断是否存在人脸，
            #如果不存在人脸则直接继续下一轮循环
            shape = self.face_db.point_face_68(img)
            if shape is None:
                continue
            else:
                self.last = self.next
                self.next = shape

            # 标出68个点的位置
            for i in range(68):
                #print(shape.part(i))
                cv2.circle(img, (shape.part(i).x, shape.part(i).y), 2, (0, 255, 0), -1, 1)
                #cv2.putText(img, str(i), (shape.part(i).x, shape.part(i).y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))  
            
            if self.last is not None:
                motion = self.face_db.experssion(self.last, self.next)
                if motion['blink_left_eye']:
                    print(self.time)
                    self.time += 1
                    print('左眼闭眼了')
                if motion['blink_right_eye']:
                    print(self.time)
                    self.time += 1            
                    print('右眼闭眼了')
                if motion['percentage_eyebrow'] > 0.4:
                    print(self.time)
                    self.time += 1            
                    print('向上眉毛动了',motion['percentage_eyebrow'])
                elif motion['percentage_eyebrow'] < -0.4:
                    print(self.time)
                    self.time += 1            
                    print('向下眉毛动了',motion['percentage_eyebrow'])
                if motion['percentage_mouth'] > 0.2 :
                    print(self.time)
                    self.time += 1            
                    print('嘴巴向下动了')
                elif motion['percentage_mouth'] < -0.2:
                    print(self.time)
                    self.time += 1
                    print('嘴巴向上动了')

                percentage_eyebrow_left = motion['percentage_eyebrow']
                percentage_eyebrow_right = motion['percentage_eyebrow']
                percentage_mouth = motion['percentage_mouth']
                blink_left_eye = motion['blink_left_eye']
                blink_right_eye = motion['blink_right_eye']
                face_point = {
                    'percentage_eyebrow_left' : percentage_eyebrow_left,
                    'percentage_eyebrow_right' : percentage_eyebrow_right,
                    'percentage_mouth' : percentage_mouth,
                    'blink_left_eye' : blink_left_eye,
                    'blink_right_eye': blink_right_eye
                }
                # if percentage_eyebrow_left == 0:
                #     print("face_point\n", face_point)

                face_action = self.face_action_function(face_point)
                # print("face_action\n", face_action)

                end_time = time.time()
                duration = end_time - start_time
                remaining_time = self.period/1000 - duration
                if remaining_time > 0:
                    # print(f"程序休眠：{remaining_time} ms")
                    time.sleep(remaining_time)
                else:
                    x=1
                    # print(f"程序比period多：{-remaining_time} ms")

                self.publish_robot_state(face_action)

            cv2.namedWindow("frame")
            img = cv2.resize(img, (720, 720))
            cv2.imshow('frame', img)
            
            mykey = cv2.waitKey(1)
            #按q退出循环，0xFF是为了排除一些功能键对q的ASCII码的影响
            if mykey & 0xFF == ord('q'):
                self.cap.release()
                cv2.destroyAllWindows()
                break

    # 根据识别出来的变化幅度确定电机的位置
    def face_action_function(self, face_point):
        # percentage_eyebrow_left
        eyebrow_left_length = int(self.xml_data[44]['jdMax']) - int(self.xml_data[44]['jdMin'])
        eyebrow_left_result = self.xml_data[44]['jdStart'] + face_point['percentage_eyebrow_left'] * eyebrow_left_length * self.std_length_ratio
        if eyebrow_left_result >= int(self.xml_data[44]['jdMax']):
            eyebrow_left_result = int(self.xml_data[44]['jdMax']) -2
        if eyebrow_left_result <= int(self.xml_data[44]['jdMin']):
            eyebrow_left_result = int(self.xml_data[44]['jdMin']) +2
        # print("eyebrow_left_result : %f" % eyebrow_left_result)

        # percentage_eyebrow_right
        eyebrow_right_length = int(self.xml_data[31]['jdMax']) - int(self.xml_data[31]['jdMin'])
        eyebrow_right_result = self.xml_data[31]['jdStart'] + face_point['percentage_eyebrow_right'] * eyebrow_right_length * self.std_length_ratio
        if eyebrow_right_result >= int(self.xml_data[31]['jdMax']):
            eyebrow_right_result = int(self.xml_data[31]['jdMax']) -2
        if eyebrow_right_result <= int(self.xml_data[31]['jdMin']):
            eyebrow_right_result = int(self.xml_data[31]['jdMin']) +2      
        # print("eyebrow_right_result : %f" % eyebrow_right_result)

        # percentage_mouth
        mouth_length = int(self.xml_data[32]['jdMax']) - int(self.xml_data[32]['jdMin'])
        mouth_result = self.xml_data[32]['jdStart'] + face_point['percentage_mouth'] * mouth_length * 2.5
        if mouth_result >= int(self.xml_data[32]['jdMax']):
            mouth_result = int(self.xml_data[32]['jdMax']) -2
        if mouth_result <= int(self.xml_data[32]['jdMin']):
            mouth_result = int(self.xml_data[32]['jdMin']) +2
        # if not (mouth_result > 64 and mouth_result < 67):
        #     print("mouth_result : %f" % mouth_result)

        # blink_left_eye = 130 if face_point['blink_left_eye'] else 50
        # blink_right_eye = 50 if face_point['blink_right_eye'] else 130
        blink_left_eye = 90
        blink_right_eye = 90

        face_action = self.face_motor
        face_action[4] = int(eyebrow_left_result)
        face_action[9] = int(eyebrow_right_result)
        face_action[-1] = int(mouth_result)

        if face_point['percentage_mouth'] != 0 or face_point['percentage_eyebrow_left'] != 0:
            print("************************************\n")
            print("eyebrow_left_result  : %d " % face_action[4])
            print(face_point['percentage_eyebrow_left'])

            print("eyebrow_right_result : %d " % face_action[9])
            print(face_point['percentage_eyebrow_right'])
            
            print("mouth_result         : %d " % face_action[-1])
            print(face_point['percentage_mouth'])
            print("************************************\n")

        return face_action

    def test(self):
        print("测试开始......\n")
        msgs = self.face_motor

        for i in range(10):
            for j in range(14):
                msgs[j] = int(msgs[j]) + 3
 
            print("第几轮：\n", i)
            print(msgs)
            self.publish_robot_state(msgs)
            time.sleep(self.period / 1000)

        print("重置：\n", msgs)
        self.publish_robot_state(self.face_motor)
        time.sleep(self.period / 1000)

def main(args=None):
    rclpy.init(args=args)
    node = LookNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()