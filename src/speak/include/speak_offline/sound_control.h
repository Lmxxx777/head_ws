#include <alsa/asoundlib.h>
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <fstream>

struct WavHeader
{
    uint32_t id;
    uint32_t chunk_size;
    uint32_t form_type;
    uint32_t fmt;
    uint32_t subchunk_size;
    uint16_t audio_format;
    uint16_t channel_nums;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data;
    uint32_t data_size;
};

void set_master_volume(long volume);
bool PlayWAV(std::string wav_file_name);