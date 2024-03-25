# facial_mouth 节点

[facial_mouth](../ulaahead/mouth_talk.py) 是应用功能节点，通过面部特征接口和语音，实现说话时的嘴形配合

## 使用准备

1. 安装　pavucontrol

```shell
sudo apt install pavucontrol
```

2. 安装　pip

```shell
sudo apt-get install python3-pip
```

3. 安装　sounddevice

```shell
pip3 install sounddevice
```

  添加组 打开bashrc

```shell
export PATH=$PATH:$HOME/.local/bin
```

4.安装　 pyaudio

```shell
　　sudo apt-get install portaudio19-dev
　　sudo apt-get install python3-all-dev
    pip3 install pyaudio
```

4. 创建.py文件输入测试代码：

```python
import matplotlib.pyplot as plt
import sounddevice as sd

print(sd.query_devices()) #查看系统扬声器和录音设备
# sd.default.device[0] = 11　#选择11号设备为当前扬声器
fs = 20000 # Hz　采样频率
duration = 5  # seconds　采样时长
myrecording = sd.rec(int(duration * fs), samplerate=fs, channels=1)
sd.wait()
plt.plot(myrecording)	#可视化接受的音频信号
plt.show()
# from scipy.io import wavfile
# wavfile.write('recording.wav', fs, myrecording)　＃生成.wav录音文件
```

3. 打开ubuntu的功能菜单点击 *PulseAudio音量控制*
4. 点击 *录音*　此时没有程序正在录音
5. 随便播放电脑声音(音乐)
6. 运行.py测试文件，查看*录音*发现有可选择设备：选择 *立体声*

## Code Information
