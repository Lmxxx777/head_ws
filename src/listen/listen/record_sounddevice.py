# from servo_control import ServoCtrl

# servo = ServoCtrl('/dev/pts/3', 200000, 8, 'N', 1)
# servo.send(50)



# from servo_control import ServoCtrl

# class RobotCtrlNode(ServoCtrl):
#     def __init__(self, arg, *args, **kwargs):
#         super(RobotCtrlNode, self).__init__(arg, *args, **kwargs)
#         self.send(100)


# node = RobotCtrlNode('/dev/pts/3', 200000, 8, 'N', 1)


# class A(object):
#     def __init__(self, *args, **kwargs):
#         print("A")

# class B(object):
#     def __init__(self, *args, **kwargs):
#         print("B")

# class C(A):
#     def __init__(self, arg, *args, **kwargs):
#         print("C","arg=",arg)
#         super(C, self).__init__(arg, *args, **kwargs)

# class D(B):
#     def __init__(self, arg, *args, **kwargs):
#         print("D", "arg=",arg)
#         super(D, self).__init__(arg, *args, **kwargs)

# class E(C,D):
#     def __init__(self, arg, *args, **kwargs):
#         print("E", "arg=",arg)
#         super(E, self).__init__(arg, *args, **kwargs)

# print("MRO:", [x.__name__ for x in E.__mro__])
# E(10, 20, 30, 40, 50)


import matplotlib.pyplot as plt
import sounddevice as sd
# sd.default.device[0] = 11
print(sd.query_devices())
fs = 20000 # Hz
tip = 5
# duration = 1  # seconds
myrecording = sd.rec(int(tip * fs), samplerate=fs, channels=1)
# myrecording = sd.rec(int(duration * fs), samplerate=fs, channels=1)
sd.wait()
# for i in myrecording:
#     print(i)
data = myrecording[-1]
if data < 0.1 :
    mouth = 65
elif data > 0.1 & data < 0.6 :
    mouth = 65 + (data -0.1) * 500
elif data > 0.6 :
    mouth = 115
print(data)
print(mouth)
plt.plot(myrecording)	
plt.show()
# from scipy.io import wavfile

# wavfile.write('recording.wav', fs, myrecording)