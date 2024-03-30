import cv2                    
import dlib               
import os

class face_db:
    #输入特征预测器的地址 获取的图像的行和列
    def __init__(self, path):
        self.path = path
        # 使用特征提取器get_frontal_face_detector
        self.detector = dlib.get_frontal_face_detector()
        # dlib的68点模型，使用官方训练好的特征预测器
        self.predictor = dlib.shape_predictor(self.path)
    

    #输入经过特征处理器处理后的dets
    #输出最靠近人脸的下标, 如果不存在人脸返回none，如果存在多个人脸则返回最靠近摄像头人脸的下标
    def num_face_closest(self, dets) -> int:
        if len(dets) == 0:
            return None
        elif len(dets) > 1:
            #num_face 为 最大的人脸的下标
            num_face = 0
            #area_face 为当前最大人脸的面积
            area_face = 0
            for k, d in enumerate(dets):
                width = d.right() - d.left()
                height = d.bottom() - d.top()

                if area_face < width * height:
                    num_face = k
                    area_face = width * height
        else:
           num_face = 0
        
        return num_face
    
    #输入图片，返回68个点的位置
    def point_face_68(self, img):
        #如果图片为none则返回none
        if img is None:
            return None
        
        #如果不存在人脸就返回none        
        dets = self.detector(img, 0)
        num_face = self.num_face_closest(dets)
        if(num_face is None):
            return None

        shape = self.predictor(img, dets[num_face])
        return shape


    #表情的判断， 输入的是前后的两个图片的68个点位的位置，可以由point_face_68得到，last，next为前后两个时间点的脸部的点位
    #输出脸部相关部位的表情的变化幅度的字典 
    #用之前一定先要用num_face_closest来判断是否存在人脸
    def experssion(self, last, next) -> dict:
        #以27到30之间的点位作为参照距离 用前后两张图的平均值作为27到30之间的值减少误差
        distance_27to30_last = abs(last.part(27).y - last.part(30).y)
        distance_27to30_new = abs(next.part(27).y - next.part(30).y)
        distance_27to30 = (distance_27to30_last + distance_27to30_new) / 2

        #嘴巴之间的距离 用3对节点之间的距离求平均得到上下嘴唇之间的平均距离
        distance_61to67_last = abs(last.part(61).y - last.part(67).y)
        distance_62to66_last = abs(last.part(62).y - last.part(66).y)
        distance_63to65_last = abs(last.part(63).y - last.part(65).y)
        distance_61to67_next = abs(next.part(61).y - next.part(67).y)
        distance_62to66_next = abs(next.part(62).y - next.part(66).y)
        distance_63to65_next = abs(next.part(63).y - next.part(65).y)
        distance_mouth_last = (distance_61to67_last + distance_62to66_last + distance_63to65_last) / 3
        distance_mouth_next = (distance_61to67_next + distance_62to66_next + distance_63to65_next) / 3

        #眉毛之间的距离  用19和24点到27之间的距离来判断运动趋势
        distance_19to27_last = abs(last.part(19).y - last.part(27).y)
        distance_24to27_last = abs(last.part(24).y - last.part(27).y)
        distance_19to27_next = abs(next.part(19).y - next.part(27).y)
        distance_24to27_next = abs(next.part(24).y - next.part(27).y)

        #判断眼睛的闭合情况 用纵横比来判断是否眨眼， 眨眼的时候眼睛的纵横比会急剧下降
        #由于眼睛的眨眼相对于人头摄像头的帧率来说速度过快，所以同时采用前后两帧来判断是否眨眼
        distance_37to41_last = abs(last.part(37).y - last.part(41).y)
        distance_38to40_last = abs(last.part(38).y - last.part(40).y)
        distance_43to47_last = abs(last.part(43).y - last.part(47).y)
        distance_44to46_last = abs(last.part(44).y - last.part(46).y)
        distance_37to41_next = abs(next.part(37).y - next.part(41).y)
        distance_38to40_next = abs(next.part(38).y - next.part(40).y)
        distance_43to47_next = abs(next.part(43).y - next.part(47).y)
        distance_44to46_next = abs(next.part(44).y - next.part(46).y)
        distance_36to39_last = abs(last.part(36).x - last.part(39).x)
        distance_42to45_last = abs(last.part(42).x - last.part(45).x)
        distance_36to39_next = abs(next.part(36).x - next.part(39).x)
        distance_42to45_next = abs(next.part(42).x - next.part(45).x)

        #眉毛的运动趋势 眉毛之间的运动范围与distance_27to30的比例是0.55-0.7 上升就为正数，下降就为负数
        distance_eyebrow_last = (distance_19to27_last + distance_24to27_last) / distance_27to30 / 2
        distance_eyebrow_next = (distance_19to27_next + distance_24to27_next) / distance_27to30 / 2
        percentage_eyebrow = (distance_eyebrow_next - distance_eyebrow_last) / 0.15

        #嘴的运动趋势 上下嘴唇之间的距离与distance_27to30的比值的范围是0-0.41 除以0.41表示变化的百分比
        percentage_mouth = (distance_mouth_next - distance_mouth_last) / 0.41 / distance_27to30

        #眨眼的运动趋势 当一只眼睛的纵横比小于0.15时判断为眨眼
        distance_left_last = (distance_37to41_last + distance_38to40_last) / 2 / distance_36to39_last
        distance_right_last = (distance_43to47_last + distance_44to46_last) / 2 / distance_42to45_last
        distance_left_next = (distance_37to41_next + distance_38to40_next) / 2 / distance_36to39_next
        distance_right_next = (distance_43to47_next + distance_44to46_next) / 2 / distance_42to45_next

        if distance_left_last < 0.1 or distance_left_next < 0.1:
            blink_left_eye = True
        else:
            blink_left_eye = False
        if distance_right_last < 0.12 or distance_right_next < 0.12:
            blink_right_eye = True
        else:
            blink_right_eye = False


        #过滤波动 根据帧率和摄像头的参数来进行相对应的调整
        if abs(percentage_mouth) < 0.25:
            percentage_mouth = 0
        if abs(percentage_eyebrow) < 0.40:
            percentage_eyebrow = 0
        
        output = {
            'percentage_eyebrow':percentage_eyebrow,
            'percentage_mouth':percentage_mouth,
            'blink_left_eye':blink_left_eye,
            'blink_right_eye':blink_right_eye
        }

        return output
    
"""     #人脸跟随   输入图像
    #返回人脸相对于图像中心点的相对位置的字典
    def face_trace(self, img) -> dict:
        if img is None:
            return None
        #如果不存在人脸就返回none        
        dets = detector(img, 0)
        num_face = self.num_face_closest(dets)
        if(num_face is None):
            return None

        #得到x和y的中心点
        x_center = self.column / 2
        y_center = self.row / 2

        #输出为相对位置的地点，如果在中心点的右下，则left和top都为None
        #如果人脸在中心点的右下，则right 和 bottom 不为None且 为 距离中心点的百分比
        output = {
            'left':None,
            'right':None,
            'top':None,
            'bottom':None
        }

        return output """

if __name__ == '__main__':

    path = os.path.join(os.getcwd(), r'temporary\shape_predictor_68_face_landmarks.dat')
    # 使用特征提取器get_frontal_face_detector
    detector = dlib.get_frontal_face_detector()
    # dlib的68点模型，使用官方训练好的特征预测器
    predictor = dlib.shape_predictor(path)


    cap = cv2.VideoCapture(0) #设备号为0
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M','J','P','G'))
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 720)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

    while(True):
        if cap.isOpened() == False:
            print('can not open camera')
            break
        #读取图像
        ret, img = cap.read()
        #图像读取失败则直接进入下一次循环
        if ret == False:
            continue

        dets = detector(img, 0)

        #判断是否存在人脸
        #如果不存在人脸则返回继续下一轮循环直至有发现人脸
        #如果人脸数量大于1，则选取面积最大那个的人脸，即靠的最近的那个的人脸来进行表情的识别
        if len(dets) == 0:
            continue
        elif len(dets) > 1:
            #num_face 为 最大的人脸的下标
            num_face = 0
            #area_face 为当前最大人脸的面积
            area_face = 0
            for k, d in enumerate(dets):
                width = d.right() - d.left()
                height = d.bottom() - d.top()

                if area_face < width * height:
                    num_face = k
                    area_face = width * height
        else:
           num_face = 0

        # 利用预测器预测
        shape = predictor(img, dets[num_face]) 
        left_distance = (abs(shape.part(37).y - shape.part(41).y) + abs(shape.part(38).y - shape.part(40).y)) / abs(shape.part(36).x - shape.part(39).x) / 2
        right_distance = (abs(shape.part(43).y - shape.part(47).y) + abs(shape.part(44).y - shape.part(46).y)) / abs(shape.part(42).x - shape.part(45).x) / 2
        #print('左眼为',round(left_distance, 3),'右眼为',round(right_distance, 3))
        cv2.putText(img, str(round(left_distance, 3)), (shape.part(1).x, shape.part(1).y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))
        cv2.putText(img, str(round(right_distance, 3)), (shape.part(35).x, shape.part(35).y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))
        # 标出68个点的位置
        for i in range(68):
            #print(shape.part(i))
            cv2.circle(img, (shape.part(i).x, shape.part(i).y), 2, (0, 255, 0), -1, 1)
            #cv2.putText(img, str(i), (shape.part(i).x, shape.part(i).y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))  


        cv2.namedWindow("frame")
        cv2.imshow('frame', img)
    
        mykey = cv2.waitKey(1)
	    #按q退出循环，0xFF是为了排除一些功能键对q的ASCII码的影响
        if mykey & 0xFF == ord('q'):
            break
        
        #限制运行
        #sleep(0.1)
        #释放资源
    cap.release()
    cv2.destroyAllWindows()