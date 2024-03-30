// 这个播放效果不好
#include "speak_offline/sound_control.h"

void set_master_volume(long volume){
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

bool PlayWAV(std::string wav_file_name){
    // wav_file_name = "/home/imi/head_ws/src/speak/resource/speak.wav";

    // open wav file 
    std::fstream wav(wav_file_name, std::ios_base::in|std::ios::binary );
    if(wav.fail()){
        std::cout << wav_file_name << " open failed!" << std::endl;
        return false;
    }

    // parse wav file 
    WavHeader wav_header; 

    wav.read((char*)&wav_header, sizeof(wav_header));
    if(wav.fail()){
        std::cout << "Read file failed!" << std::endl;
        return -1;
    }
    if(wav_header.id != (uint32_t)0x46464952 or wav_header.fmt != (u_int32_t)0x20746d66 \
        or wav_header.audio_format != 1){
        std::cout << std::hex << wav_header.id << std::endl;
        std::cout << std::hex << wav_header.fmt << std::endl;
        std::cout << std::hex << wav_header.data << std::endl;
        std::cout << "audio format: " << wav_header.audio_format << std::endl; 
        // audio format 为1 表示
        std::cout << "File format error!" << std::endl;
        return -1;
    }

    //summary 
    std::cout << "sample_rate: " << wav_header.sample_rate <<  std::endl;
    std::cout << "bit per sample: " << wav_header.bits_per_sample << std::endl;
    std::cout << "channel nums: " << wav_header.channel_nums << std::endl;

    set_master_volume(50); // 99%
    // play audio 
    int err;
    // unsigned int i;
    snd_pcm_t *handle;
    snd_pcm_sframes_t frames;
    std::string device("default");
    unsigned char buffer[512];

    if ((err = snd_pcm_open(&handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    snd_pcm_format_t pcm_format;
    switch (wav_header.bits_per_sample)
    {

    case 8:
        pcm_format = SND_PCM_FORMAT_S8;
        break;
    case 16:
        pcm_format = SND_PCM_FORMAT_S16_LE;
        break;
    case 32:
        pcm_format = SND_PCM_FORMAT_S32_LE;
        break;
    default:
        break;
    }

    if ((err = snd_pcm_set_params(handle,
                      pcm_format,
                      SND_PCM_ACCESS_RW_INTERLEAVED,
                      wav_header.channel_nums,
                      wav_header.sample_rate,
                      1,
                      500000)) < 0) {   /* 0.5sec */
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    while (true) {
        wav.read((char *)&buffer, 512);
        if(wav.gcount() <= 0){
            std::cout << "END" << std::endl;
            break;
        }
        frames = snd_pcm_writei(handle, buffer, wav.gcount()/(wav_header.bits_per_sample * 2 / 8));
        if (frames < 0)
            frames = snd_pcm_recover(handle, frames, 0);
        if (frames < 0) {
            printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
            break;
        }
        if(wav.gcount() != 512){
            std::cout << "END" << std::endl;
            break;
        }
    }

    /* pass the remaining samples, otherwise they're dropped in close */
    err = snd_pcm_drain(handle);
    if (err < 0)
        printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
    snd_pcm_close(handle);
    return true;
}