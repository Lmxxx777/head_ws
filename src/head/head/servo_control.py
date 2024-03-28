from serial import Serial
import time

# TODO: 从xml文件直接读取配置
class Servo:
    def __init__(self, id, jdStart, jdMax, jdMin, fScale, fOffSet, pos):
        self.id = id
        self.jdStart = jdStart
        self.jdMax = jdMax
        self.jdMin = jdMin
        self.fScale = fScale
        self.fOffSet = fOffSet
        self.pos = pos

left_blink = Servo(11, 90, 145, 60, 111.1, 0, 90)      # 左眨眼 
left_smile = Servo(12, 90, 130, 85, 111.1, 0, 90)      # 左微笑
left_eye_erect = Servo(13, 90, 120, 60, 83.3, 30, 90)  # 左眼竖
left_eye_level = Servo(14, 90, 120, 60, 83.3, 30, 90)  # 左眼平
left_eyebrow = Servo(44, 90, 140, 40, 111.1, 0, 90)    # 左眉毛

right_blink = Servo(24, 90, 120, 35, 111.1, 0, 90)     # 右眨眼
right_smile = Servo(23, 90, 95, 50, 111.1, 0, 90)      # 右微笑
right_eye_erect = Servo(22, 90, 120, 60, 83.3, 30, 90) # 右眼竖//
right_eye_level = Servo(21, 90, 120, 60, 83.3, 30, 90) # 右眼平//
right_eyebrow = Servo(31, 90, 140, 40, 111.1, 0, 90)   # 右眉毛

head_dian = Servo(43, 90, 160, 20, 111.1, 0, 90) # 点头
head_yao = Servo(42, 90, 160, 20, 111.1, 0, 90)  # 摇头 85
head_bai = Servo(33, 90, 160, 20, 111.1, 0, 90)  # 摆头85

mouth = Servo(32, 65, 135, 60, 111.1, 0, 65) # 嘴巴


servos = [left_blink, left_smile, left_eye_erect, left_eye_level, left_eyebrow,
          right_blink, right_smile, right_eye_erect, right_eye_level, right_eyebrow,
          head_dian, head_yao, head_bai,
          mouth
]

class ServoCtrl(Serial):
    def __init__(self, arg, *args, **kwargs):
        super().__init__(arg, *args, **kwargs)
        if self.is_open:
            print('Open Success')
        else:
            print('Open Error')

    # 对下位机电机的控制
    def send(self, msgs):
        head0 = 0xff
        head1 = 0Xff
        cmd_id = 0xfe
        len = 0x00
        cmd = 0x83
        par1 = 0x2a
        par2 = 0x04

        frameData = [head0, head1, cmd_id, len, cmd, par1, par2]

        servo_num = 0
        for node, servo in zip(msgs, servos):
            if node and node[0] != servo.pos: # 目标位置改变
                if node[0] != 0: # msg 没有值
                    # 限幅
                    if node[0] > servo.jdMax:
                        node[0] = servo.jdMax
                    if node[0] < servo.jdMin:
                        node[0] = servo.jdMin
                    servo.pos = node[0]
                    node[0] = int((node[0] + servo.fOffSet) * servo.fScale)
                    pos_l = node[0] & 0xFF
                    pos_h = (node[0] >> 8) & 0xFF

                    time_l = node[1] & 0xFF
                    time_h = (node[1] >> 8) & 0xFF
                    # time_l = (node[1] * 200) & 0xFF
                    # time_h = ((node[1] * 200) >> 8) & 0xFF

                    frameData.extend([servo.id, pos_h, pos_l, time_h, time_l])
                    servo_num += 1
        if servo_num == 0:
            return
        len = 5*servo_num + 4
        frameData[3] = len

        ser_sum = 0
        for x in frameData[2:]:
            ser_sum += x
        ser_sum = ser_sum & 0xff
        ser_sum = ~ser_sum & 0xff

        serFrame = frameData
        serFrame.append(ser_sum)
        
        if self.is_open:
            self.write(serFrame)
            print('send to servo ok!!!!!!!!!!\n')
            # print('send to servo ok',serFrame)
    

if __name__ == '__main__':
    servo_ctrl = ServoCtrl('/dev/ttyUSB0', 115200) #921600

    left_blink      = [ 91, 10]# 左眨眼 
    left_smile      = [ 91, 10]# 左微笑
    left_eye_erect  = [ 91, 10]# 左眼竖
    left_eye_level  = [ 91, 10]# 左眼平
    left_eyebrow    = [ 91, 10]# 左眉毛

    right_blink     = [ 91, 10]# 右眨眼
    right_smile     = [ 91, 10]# 右微笑
    right_eye_erect = [ 91, 10]# 右眼竖//
    right_eye_level = [ 91, 10]# 右眼平//
    right_eyebrow   = [ 91, 10]# 右眉毛

    head_dian       = [ 91, 10]# 点头
    head_yao        = [ 91, 10]# 摇头 85
    head_bai        = [ 91, 10]# 摆头85

    mouth           = [ 66, 10]# 嘴巴

    msgs = [left_blink, left_smile, left_eye_erect, left_eye_level, left_eyebrow, right_blink, right_smile, right_eye_erect, right_eye_level, right_eyebrow, head_dian, head_yao, head_bai, mouth] 
    print("初始化：\n", msgs)
    servo_ctrl.send(msgs)
    time.sleep(200 * 18 / 1000)

    # for i in range(10):
    #     for j in range(14):
    #         msgs[j][0] = msgs[j][0] + 5
    #         msgs[j][1] = msgs[j][1] + 20
    #         print(msgs[j])
    #     print("第几轮：\n", i)
    #     print(msgs)
    #     servo_ctrl.send(msgs)
    #     time.sleep(200 * 18 / 1000)


    # print("重置：\n", msgs)
    # msgs = [left_blink, left_smile, left_eye_erect, left_eye_level, left_eyebrow, right_blink, right_smile, right_eye_erect, right_eye_level, right_eyebrow, head_dian, head_yao, head_bai, mouth] 
    # servo_ctrl.send(msgs)
    # time.sleep(200 * 18 / 1000)

    # for i in range(10):
    #     for j in range(14):
    #         msgs[j][0] = msgs[j][0] + 5
    #         msgs[j][1] = msgs[j][1] + 20
    #         print(msgs[j])

        
    #     time.sleep(20 * 18 / 1000)
    #     print("第几轮：\n", i)
    #     print(msgs)
    #     servo_ctrl.send(msgs)