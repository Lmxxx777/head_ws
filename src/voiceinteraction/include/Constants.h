#ifndef CONSTANTS_X_H_
#define CONSTANTS_X_H_

#define TEST_ROOT_DIR "./src/voiceinteraction/AIUI/"
//配置文件打的路径，里面是客户端设置的参数
#define CFG_FILE_PATH "./src/voiceinteraction/AIUI/cfg/aiui.cfg"
//测试音频的路径
#define TEST_AUDIO_PATH "./src/voiceinteraction/AIUI/audio/test.pcm"
#define GRAMMAR_FILE_PATH "./src/voiceinteraction/AIUI/asr/call.bnf"
#define LOG_DIR "./src/voiceinteraction/AIUI/log/"
#define MSC_DIR         "./src/voiceinteraction/AIUI/msc/"

#define TEST_RECORD_PATH "./src/voiceinteraction/AIUI/audio/record.pcm"
#define CONFIG_FILE_PATH "./src/voiceinteraction/tmp/config.txt"
#define SOURCE_FILE_PATH "./src/voiceinteraction/tmp/system.tar"
#define PCM_FILE_PATH    "./src/voiceinteraction/hid_aiui_deno.pcm"
#define ORIPCM_FILE_PATH "./src/voiceinteraction/hid_aiui_ori.pcm"
#define PCM_MSG_LEN      1024
#define ORI_PCM_MSG_LEN  16384
#include <string>
using namespace std;
static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
class Constants
{
private:
    /* data */
public:
    Constants(/* args */);
    ~Constants();
};

Constants::Constants(/* args */)
{
}

Constants::~Constants()
{
}


#endif /* CONSTANTS_X_H_ */
