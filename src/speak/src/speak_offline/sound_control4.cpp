#include <alsa/asoundlib.h>
#include <sndfile.h>

int main() {
    // 打开.wav文件
    const char* wavFile = "/home/imi/head_ws/src/speak/resource/speak.wav/home/imi/head_ws/src/speak/resource/speak.wav";
    SF_INFO sfInfo;
    SNDFILE* sndFile = sf_open(wavFile, SFM_READ, &sfInfo);
    if (!sndFile) {
        // 错误处理
        return -1;
    }

    // 打开PCM设备
    snd_pcm_t* pcmHandle;
    int err = snd_pcm_open(&pcmHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        // 错误处理
        sf_close(sndFile);
        return -1;
    }

    // 配置PCM参数
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcmHandle, params);
    snd_pcm_hw_params_set_access(pcmHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcmHandle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcmHandle, params, sfInfo.channels);
    unsigned int sampleRate = sfInfo.samplerate;
    snd_pcm_hw_params_set_rate_near(pcmHandle, params, &sampleRate, 0);
    snd_pcm_hw_params(pcmHandle, params);

    // 准备音频缓冲区
    const int bufferSize = 1024;
    short buffer[bufferSize * sfInfo.channels];

    // 播放音频
    sf_count_t framesRead;
    while ((framesRead = sf_readf_short(sndFile, buffer, bufferSize)) > 0) {
        snd_pcm_sframes_t framesWritten = snd_pcm_writei(pcmHandle, buffer, framesRead);
        if (framesWritten < 0) {
            // 错误处理
            break;
        }
    }

    // 关闭PCM设备
    snd_pcm_close(pcmHandle);
    printf("close!!\n");
    // 关闭.wav文件
    sf_close(sndFile);

    return 0;
}