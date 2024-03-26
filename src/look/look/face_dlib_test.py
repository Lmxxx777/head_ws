import os
import cv2
from face_dlib import face_db
from time import sleep

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
