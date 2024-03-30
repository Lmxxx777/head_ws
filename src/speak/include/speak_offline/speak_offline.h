#include <iostream>
#include <unistd.h>
#include <errno.h>
#include "../../include/qtts.h"
#include "../../include/msp_cmn.h"
#include "../../include/msp_errors.h"
#include <string.h>

using namespace std;

typedef int SR_DWORD;
typedef short int SR_WORD;

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
const wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	16000,
	144,
	16,
	{'d', 'a', 't', 'a'},
	0
};

class VoiceOffline{
private:
	int         ret                  = MSP_SUCCESS;
	const char* login_params         = "appid = 8b722e6c, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
	/*
	* rdn:           合成音频数字发音方式
	* volume:        合成音频的音量
	* pitch:         合成音频的音调
	* speed:         合成音频对应的语速
	* voice_name:    合成发音人
	* sample_rate:   合成音频采样率
	* text_encoding: 合成文本编码格式
	*
	*/
	const char* session_begin_params = "engine_type = purextts,voice_name=xiaofeng, text_encoding = UTF8, tts_res_path = fo|/home/imi/head_ws/src/speak/bin/msc/res/xtts/xiaofeng.jet;fo|/home/imi/head_ws/src/speak/bin/msc/res/xtts/common.jet, sample_rate = 16000, speed = 30, volume = 50, pitch = 45, rdn = 2";

    int text_to_speech(const char* src_text, const char* des_path, const char* params);
public:
    VoiceOffline();
    int VoiceOfflineGenerator(const char* src_text, const char* des_path);
    ~VoiceOffline();
};