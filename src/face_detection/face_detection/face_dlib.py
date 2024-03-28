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
        #点63和67之间的最大距离大概是 28-31点之间距离的0.65
        #正常状态下 20 和 25 到 28 的 y 的距离的平均值 大概是 28-31 点之间距离的 0.91倍
        #最大状态下 20 和 25 到 28 的 y 的距离的平均值 大概是 28-31 点之间距离的 1.01倍

        #28到31之间为参照距离 用前后两张图来做平均值减少误差
        distance_28to31 = (abs(last.part(28).y - last.part(31).y) + abs(next.part(28).y - next.part(31).y)) / 2
        #25和20到28节点之间的y轴的平均距离 用来判断眉毛
        distance_25_20to28_last = (abs(last.part(20).y - last.part(28).y) + abs(last.part(25).y - last.part(28).y)) / 2
        distance_25_20to28_next = (abs(next.part(20).y - next.part(28).y) + abs(next.part(25).y - next.part(28).y)) / 2
        #63和67之间的距离，用来判断嘴的张开程度
        distance_63to67_last = abs(last.part(63).y - last.part(67).y)
        distance_63to67_next = abs(next.part(63).y - next.part(67).y)
        #38和42， 39和41， 44和48， 45和47 37和40， 43和46 之间的距离用来判断眨眼
        distance_38to42_last = abs(last.part(38).y - last.part(42).y)
        distance_39to41_last = abs(last.part(39).y - last.part(41).y)
        distance_44to48_last = abs(last.part(44).y - last.part(48).y)
        distance_45to47_last = abs(last.part(45).y - last.part(47).y)
        distance_38to42_next = abs(next.part(38).y - next.part(42).y)
        distance_39to41_next = abs(next.part(39).y - next.part(41).y)
        distance_44to48_next = abs(next.part(44).y - next.part(48).y)
        distance_45to47_next = abs(next.part(45).y - next.part(47).y)
        distance_37to40_last = abs(last.part(37).x - last.part(40).x)
        distance_43to46_last = abs(last.part(43).x - last.part(46).x)
        distance_37to40_next = abs(next.part(37).x - next.part(40).x)
        distance_43to46_next = abs(next.part(43).x - next.part(46).x)
        #左右眼的纵横比
        distance_left_eye_last = (distance_38to42_last + distance_39to41_last) / 2 / distance_37to40_last
        distance_right_eye_last = (distance_44to48_last + distance_45to47_last) / 2 / distance_43to46_last
        distance_left_eye_next = (distance_38to42_next + distance_39to41_next) / 2 / distance_37to40_next
        distance_right_eye_next = (distance_44to48_next + distance_45to47_next) / 2 / distance_43to46_next
        #眉毛的上挑的变化比例 眉毛下降就为负数，上升就为正数 眉毛的变化比例为 0.86 - 1.01
        percentage_eyebrow = (distance_25_20to28_next - distance_25_20to28_last) / distance_28to31 / 0.18
        #嘴巴的变化比例为0-0.65 除以0.65表示变化了最大值的百分之多少
        percentage_mouth = (distance_63to67_next - distance_63to67_last) / distance_28to31 / 0.65
        #左右眼是否眨眼 用纵横比来计算
        """ if distance_left_eye_last < 1 or distance_left_eye_next < 1:
            blink_left_eye = True
        else:
            blink_left_eye = False
        if distance_right_eye_last < 1 or distance_right_eye_next < 1:
            blink_right_eye = True
        else:
            blink_right_eye = False """
        if distance_left_eye_last == 0 or distance_left_eye_next == 0:
            blink_left_eye = True
        else:
            blink_left_eye = False

        if distance_right_eye_last == 0 or distance_right_eye_next == 0:
            blink_right_eye = True
        else:
            blink_right_eye = False

        if percentage_eyebrow > 1:
            percentage_eyebrow = 0.95
        elif percentage_eyebrow < -1:
            percentage_eyebrow = -0.95

        if percentage_mouth > 1:
            percentage_mouth = 0.95
        elif percentage_mouth < -1:
            percentage_mouth = -0.95
        #过滤
        if abs(percentage_eyebrow) < 0.4:
            percentage_eyebrow = 0
        if abs(percentage_mouth) < 0.2:
            percentage_mouth = 0
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
        """ distance_20_28 = abs(shape.part(20).y - shape.part(28).y)
        distance_25_28 = abs(shape.part(25).y - shape.part(28).y)
        distance_total = abs(distance_20_28 + distance_25_28) / 2
        distance_28_31 = abs(shape.part(28).y - shape.part(31).y)
        print('嘴的比例{}'.format(distance_total/distance_28_31)) """
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