import ffmpeg

from ffmpy import FFmpeg

# #将mp3转化没pcm
# ff = FFmpeg(inputs={r'd:\test\1.mp3':"-i"},
#            outputs={r'd:\test\1.pcm':"-f s16be -ar 16000 -ac 1 -acodec pcm_s16be"})
# print(ff.cmd)
# ff.run()

#将pcm转化为mp3
ff1 = FFmpeg(inputs={r'/home/lmx/head_ws/VOICE/iat_pcm_16k.pcm':"-f s16be -ar 16000 -ac 1 -acodec pcm_s16be"},
           outputs={r'/home/lmx/head_ws/VOICE/iat_pcm_16k.mp3':None})
print(ff1.cmd)
ff1.run()

# from pydub import AudioSegment
# song = AudioSegment.from_wav("demo.wav")
# song.export("wav-mp3.mp3", format="mp3")

def main():
    print('Hi from my_package.')


if __name__ == '__main__':
    main()
