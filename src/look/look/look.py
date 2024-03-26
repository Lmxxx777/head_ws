import rclpy
from rclpy.node import Node
from std_msgs.msg import Int8
from chat_msgs.msg import Servo
import time
import cv2
import os
from look.xml_read import XmlData
from face_dlib import face_db

def face_expression():
    path = os.path.join(os.getcwd(), r'/model/shape_predictor_68_face_landmarks.dat')
    face_db = face_db(path)

    cap = cv2.VideoCapture(0) #设备号为0
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M','J','P','G'))
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 480)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    last = None
    next = None
    time = 0

    while(True):
        if cap.isOpened() == False:
            print('can not open camera')
            break
        #读取图像
        ret, img = cap.read()
        #图像读取失败则直接进入下一次循环
        if ret == False:
            continue

        #判断是否存在人脸，
        #如果不存在人脸则直接继续下一轮循环
        shape = face_db.point_face_68(img)
        if shape is None:
            continue
        else:
            last = next
            next = shape

        # 标出68个点的位置
        for i in range(68):
            #print(shape.part(i))
            cv2.circle(img, (shape.part(i).x, shape.part(i).y), 2, (0, 255, 0), -1, 1)
            #cv2.putText(img, str(i), (shape.part(i).x, shape.part(i).y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))  
        
        if last is not None:
            motion = face_db.experssion(last, next)
            if motion['blink_left_eye']:
                print(time)
                time += 1
                print('左眼闭眼了')
            if motion['blink_right_eye']:
                print(time)
                time += 1            
                print('右眼闭眼了')
            if motion['percentage_eyebrow'] > 0.3:
                print(time)
                time += 1            
                print('向上眉毛动了')
            if motion['percentage_mouth'] > 0.1 :
                print(time)
                time += 1            
                print('嘴巴向下动了')
            elif motion['percentage_mouth'] < -0.1:
                print(time)
                time += 1
                print('嘴巴向上动了')

        cv2.namedWindow("frame")
        img = cv2.resize(img, (720, 720))
        cv2.imshow('frame', img)
        
        mykey = cv2.waitKey(1)
        #按q退出循环，0xFF是为了排除一些功能键对q的ASCII码的影响
        if mykey & 0xFF == ord('q'):
            break
            
        #限制运行
        #sleep(0.05)
        #释放资源
    cap.release()
    cv2.destroyAllWindows()

class LookNode(Node):
    def __init__(self):
        super().__init__('look')      
        self.dataxml = XmlData()
        self.xml_data = self.dataxml.xml_moto()

        self.std_length_ratio = 1
        self.publish_cnt = 0

        self.sub = self.create_subscription(Int8, 'copy_action', self.copy_action_callback, 10)
        self.pub = self.create_publisher(Servo, 'robot_state', 10)
        
        self.timer_pause = True
        self.timer = self.create_timer(0.03, self.timer_callback)  # 30ms的定时器
        self.get_logger().info('LookNode initialized successfully......')

    def copy_action_callback(self, msg):
        if msg.data == 1:
            # 开启模仿，先初始化，然后打开回调函数
            self.publish_cnt = 1
            self.publish_robot_state([90, 90, 65, 90, 90], 1)
            self.timer_pause = False
        elif msg.data == 2:
            # 停止模仿，先停止，再归位
            self.timer_pause = True
            self.publish_cnt = 1
            self.publish_robot_state([90, 90, 65, 90, 90])

    def timer_callback(self):
        if not self.timer_pause:
            start_time = time.time()

            face_point = self.face_recognation_function()
            face_action = self.face_action_function(face_point)

            end_time = time.time()
            duration = end_time - start_time
            print(f"程序运行时间：{duration} 秒")
            remaining_time = 0.03 - duration
            if remaining_time > 0:
                time.sleep(remaining_time)

            self.publish_robot_state(face_action)


    # def copy_action_callback(self, msg):
    #     if msg.data == 1:
    #         self.publish_cnt = 0
    #         self.publish_robot_state([90, 90, 65, 90, 90])
    #     elif msg.data == 2:
    #         start_time = time.time()

    #         face_point = self.face_recognation_function()
    #         face_action = self.face_action_function(face_point)

    #         end_time = time.time()
    #         duration = end_time - start_time
    #         print(f"程序运行时间：{duration} 秒")
    #         remaining_time = 0.03 - duration
    #         if remaining_time > 0:
    #             time.sleep(remaining_time)

    #         self.publish_robot_state(face_action)


    def publish_robot_state(self, face_action):
        if not self.timer_pause:
            msg = Servo()
            id = [44, 31, 32, 11, 24]
            for i in range(3):
                name = id[i]   # 电机ID
                pos = int(face_action[0])   # Y的值，pos
                tim = 300 * self.publish_cnt
                setattr(msg, name, [pos, tim])
            for i in range(3, 5):
                name = id[i]   # 电机ID
                pos = int(face_action[0])   # Y的值，pos
                tim = 300 * self.publish_cnt
                setattr(msg, name, [pos, tim])
            
            self.publish_cnt = self.publish_cnt + 1
            self.pub.publish(msg)

    def face_recognation_function(self):
        percentage_eyebrow_left = -0.2
        percentage_eyebrow_right = 0.4
        percentage_mouth = -0.3
        blink_left_eye = 1
        blink_right_eye = 2
        face_point = {
            'percentage_eyebrow_left' : percentage_eyebrow_left,
            'percentage_eyebrow_right' : percentage_eyebrow_right,
            'percentage_mouth' : percentage_mouth,
            'blink_left_eye' : blink_left_eye,
            'blink_right_eye': blink_right_eye
        }
        return face_point

    def face_action_function(self, face_point):
        # percentage_eyebrow_left
        eyebrow_left_length = int(self.xml_data['44']['jdMax']) - int(self.xml_data['44']['jdMin'])
        print("eyebrow_left_length : %d" % eyebrow_left_length)
        eyebrow_left_ressult = self.xml_data['44']['jdStart'] + face_point['percentage_eyebrow_left'] * eyebrow_left_length * self.std_length_ratio
        print("eyebrow_left_length : %f" % eyebrow_left_length)

        # percentage_eyebrow_right
        eyebrow_right_length = int(self.xml_data['31']['jdMax']) - int(self.xml_data['31']['jdMin'])
        print("eyebrow_right_length : %d" % eyebrow_right_length)
        eyebrow_right_ressult = self.xml_data['31']['jdStart'] + face_point['percentage_eyebrow_right'] * eyebrow_right_length * self.std_length_ratio
        print("eyebrow_right_length : %f" % eyebrow_right_length)

        # percentage_mouth
        mouth_length = int(self.xml_data['32']['jdMax']) - int(self.xml_data['32']['jdMin'])
        print("mouth_length : %d" % mouth_length)
        mouth_ressult = self.xml_data['32']['jdStart'] + face_point['percentage_mouth'] * mouth_length * self.std_length_ratio
        print("mouth_length : %f" % mouth_length)

        blink_left_eye = 1 if face_point['blink_left_eye'] else 0
        blink_right_eye = 1 if face_point['blink_right_eye'] else 0

        face_action = [eyebrow_left_ressult, eyebrow_right_ressult, mouth_ressult, blink_left_eye, blink_right_eye]
        return face_action

def main(args=None):
    rclpy.init(args=args)
    node = LookNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()