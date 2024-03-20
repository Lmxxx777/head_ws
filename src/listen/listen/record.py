import pyaudio
import wave

# 配置录音参数
CHUNK = 1024  # 每个缓冲区的帧数
FORMAT = pyaudio.paInt16  # 采样位数
CHANNELS = 1  # 声道数
RATE = 44100  # 采样率
RECORD_SECONDS = 5  # 录音时长
OUTPUT_FILE = "recording.wav"  # 保存文件名

# 创建 PyAudio 对象
audio = pyaudio.PyAudio()

# 打开麦克风
stream = audio.open(format=FORMAT, channels=CHANNELS,
                    rate=RATE, input=True,
                    frames_per_buffer=CHUNK)

print("开始录音...")

frames = []  # 用于存储录音数据

# 录音
for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
    data = stream.read(CHUNK)
    frames.append(data)

print("录音结束.")

# 停止录音和关闭流
stream.stop_stream()
stream.close()
audio.terminate()

# 保存录音数据到文件
wf = wave.open(OUTPUT_FILE, 'wb')
wf.setnchannels(CHANNELS)
wf.setsampwidth(audio.get_sample_size(FORMAT))
wf.setframerate(RATE)
wf.writeframes(b''.join(frames))
wf.close()

print("录音已保存为文件:", OUTPUT_FILE)