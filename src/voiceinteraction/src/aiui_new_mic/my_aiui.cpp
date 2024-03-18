#ifdef _WIN32
#    include <windows.h>
#    define AIUI_SLEEP Sleep
#else
#    include <unistd.h>
#    define AIUI_SLEEP(x) usleep(x * 1000)
#endif

#undef AIUI_LIB_COMPILING

#include "aiui/AIUI_V2.h"
#include "json/json.h"
#include <fstream>
#include <iostream>
#include <thread>
#include "aiui/AIUIType.h"
#include "AudioPlayer.h"
#include "RingBuffer.h"
#include "mic_com.h"
#include "my_aiui.h"
#include "ros2node.h"
#include "rclcpp/rclcpp.hpp"

using namespace std;
using namespace VA;
using namespace aiui_v2;

IAIUIAgent* agent = nullptr;
void writeAudio();
void stopAudio();
void gTTS(string text);
bool no_tts = false;

#define MAX_BUFFER 2097152
AudioPlayer* audioPlayer; // 与音频播放相关
static RingBuffer buffer_source(MAX_BUFFER);
int read_flag = 0;
int speak_flag = 0;
AIUINewMic *p_node;

void read_and_play1() //void *
{
	if (buffer_source.get_length() > 0 && speak_flag == 1)
	{
		read_flag = 1; //pthread_mutex_unlock(&mut);
		char data_to_speech[200000];
		int res1 = buffer_source.RingBuff_Tx(data_to_speech, buffer_source.get_length());
		read_flag = 0;
		if (res1 != -1)
		{
			audioPlayer->Write((unsigned char *)data_to_speech, res1);
		}
	}
}


extern int angle_int;
extern int if_awake;
int once_awake;
int wake_switch = 1;

void AIUINewMic::subTtsText(std_msgs::msg::String::SharedPtr msg)
{
    if(msg->data != " ")
    {
        stopAudio();
        gTTS(msg->data);
    }
        
    // else if(msg->data == "开启录音")
    //     writeAudio();
}

void *com_wakeup(void* args)
{
    int fd=1, read_num = 0;
    unsigned char buffer[1];
    memset(buffer, 0, 1);
    char* uartname="/dev/wheeltec_mic";
    if((fd=open_port(uartname))<0)
    {
         printf("open %s is failed\n",uartname);
         return 0;
    }
    else{
            set_opt(fd, 115200, 8, 'N', 1);
            printf("set_opt fd=%d\n",fd);
            int state_old;
            int state_flag = 0;
            int state_ulaa_comming = 0;
            std::string send_data = "";
            int gtts_flag = 0;
            while(1){
                    // usleep(1000000);
                    memset(buffer, 0, 1);
                    read_num = read(fd, buffer, 1);
                    // printf("read_num=%d\n",read_num);
                    if(read_num>0){
						// printf("%x\n", buffer[0]);
						deal_with(buffer[0]);
                    }
                    //else{
                    //    printf("read error\n");
                    //}
                    int state = audioPlayer->pcmState();
                    if(if_awake){
						printf("angle -------------------------- : [%d]\n",angle_int);
                        sleep(0.8);
						if_awake = 0;
                        wake_switch = 0;
                        p_node->pubMicAngleMsg(angle_int);
                        writeAudio();
                        gTTS("嗯,我在");
                        state_flag = 1;
					} 
                    if ((state == state_old) && (state_flag == 0)) {
                        // sleep(1);
                        writeAudio();
                        state_flag = 1;
                        p_node->ulaa_state.speak_flag = 0;
                        p_node->speak_state_pub->publish(p_node->ulaa_state);
                        RCLCPP_INFO(p_node->get_logger(),"录音状态: 开始录音  state: %d",state);
                    }
                     else if ((state != state_old) && (state_flag == 1)){
                        stopAudio();
                        state_flag = 0;
                        p_node->ulaa_state.speak_flag = 1;
                        p_node->speak_state_pub->publish(p_node->ulaa_state);
                        RCLCPP_INFO(p_node->get_logger(),"录音状态: 停止录音  state: %d",state);
                        //cout << "playing: " << state << endl;
                    }
                    state_old = state;
                    //介绍内容修改部分
                    if (p_node->school_msg == "到达展厅" && gtts_flag == 0) {
                        // gTTS("首先");
                        //p_node->school_msg = "";
                        cout << "到达展厅" << endl;
                        p_node->string_msg_body.data = "动作1";
                        p_node->body_pub->publish(p_node->string_msg_body);
                        //gTTS("首先，我先跟大家介绍一下中原动力。");
                        //gTTS("首先，我先跟大家介绍一下中原动力。中原动力智能机器人有限公司由豫籍顶尖科学家、德国国家科学与工程院院士张建伟及其博士团队主导组建，于2019年12月成立，2020年5月15日正式运营，总部位于中原科技城创新孵化基地C栋2层。中原动力是一家拥有160多名员工，集研发、设计、生产和销售于一体的现代化科技企业，目前旗下业务面向智能制造和公共服务两大领域。也就是说，制造我的人类爸爸，就在其中。中原动力始终洞察时代发展趋势，探索机器人技术的创新与应用，赋能全球客户实现数智化发展，覆盖智慧物业、智能制造、智慧医疗、智慧城市、智慧建筑等多个领域。中原动力总部位于郑州，在德国汉堡和上海分别设有研发中心。2021年，中原动力在郑州建设交付中心，以打造河南人工智能产业集群为目标，整合机器人产业链资源，构建智能机器人产业基地和生态体系。作为中原科技城首批签约入驻企业，公司始终秉承“始于中原，驱动世界”的初心，瞄准国际前沿，致力于建立一家河南人自己创办的生于河南、长于河南、以技术创新为原动力的新型科技企业。依托张建伟院士团队的有力支撑以及德国汉堡大学多模态技术研究所和上海理工大学机器智能研究院的技术、人才优势，公司在视觉智能分析、自然语言处理、多模态人机共融、智能精密装配、定位导航、关节模组等领域研发方面具有领先优势，涵盖机器人结构、电气、运动控制和智能计算等多个领域。中原动力创新性地提出类脑智能（OD Brain）+自有核心组件（OD Module）架构，依托机器视觉、多模态融合和数字孪生等先进技术，实现旗下自研智能产品（OD Agent）的自我学习与进化。类人大脑通过5G网络赋能各种类型的机器人，使其具有规划运动、自主导航、智能识别、自主避障等能力。");
                        gtts_flag = 1;
                    }
                    if (p_node->school_msg == "到达双足机器人" && gtts_flag == 0) {
                        // gTTS("首先");
                        // p_node->school_msg = "";
                        cout << "到达双足机器人" << endl;
                        p_node->string_msg_body.data = "动作2";
                        p_node->body_pub->publish(p_node->string_msg_body);
                        gTTS("我的工程师爸爸们在降低硬件复杂度的同时，提高了平衡、安全、负载能力，让我的前辈在消耗0.8度电的情况下，耗时5 0多小时，行走了134公里，远超美国机器人，创造了吉尼斯世界纪录，是当今最具普及前景的高性能双足机器人。中原动力双足机器人克服了传统人形机器人腿弯、步小、大平脚的难题，做到了与人“神似”行走步态。");
                        gtts_flag = 1;
                    }
                    if (p_node->school_msg == "0") {
                        gtts_flag = 0;
                    }

                    
            }
        fd=close(fd);
    }
    return 0;
}


class MyListener : public IAIUIListener
{
public:
    void onEvent(const IAIUIEvent& event) override
    {
        try {
            __onEvent(event);
        } catch (std::exception& e) {
            printf("%s", e.what());
        }
    }

private:
    void __onEvent(const IAIUIEvent& event)
    {
        switch (event.getEventType()) {
            //SDK 状态回调
            case AIUIConstant::EVENT_STATE: {
                switch (event.getArg1()) {
                    case AIUIConstant::STATE_IDLE: {
                        cout << "EVENT_STATE:"
                             << "IDLE" << endl;
                    } break;

                    case AIUIConstant::STATE_READY: {
                        cout << "EVENT_STATE:"
                             << "READY" << endl;
                    } break;

                    case AIUIConstant::STATE_WORKING: {
                        cout << "EVENT_STATE:"
                             << "WORKING" << endl;
                    } break;
                }
            } break;

                //唤醒事件回调
            case AIUIConstant::EVENT_WAKEUP: {
                cout << "EVENT_WAKEUP:" << event.getInfo() << endl;
            } break;

                //休眠事件回调
            case AIUIConstant::EVENT_SLEEP: {
                cout << "EVENT_SLEEP:arg1=" << event.getArg1() << endl;
            } break;

                //VAD事件回调，如找到前后端点
            case AIUIConstant::EVENT_VAD: {
                switch (event.getArg1()) {
                    case AIUIConstant::VAD_BOS: {
                        cout << "EVENT_VAD: BOS" << endl;
                    } break;

                    case AIUIConstant::VAD_EOS: {
                        cout << "EVENT_VAD: EOS" << endl;
                    } break;

                    case AIUIConstant::VAD_VOL: {
                    } break;
                }
            } break;

                //最重要的结果事件回调
            case AIUIConstant::EVENT_RESULT: {
                Json::Value bizParamJson;
                Json::Reader reader;

                if (!reader.parse(event.getInfo(), bizParamJson, false)) {
                    cout << "parse error!" << endl << event.getInfo() << endl;
                    break;
                }
                Json::Value& data = (bizParamJson["data"])[0];
                Json::Value& params = data["params"];
                Json::Value& content = (data["content"])[0];

                string sub = params["sub"].asString();

                if (sub == "nlp" || sub == "iat" || sub == "tts" || sub == "asr" ||
                    sub == "esr_pgs" || sub == "esr_fsa" || sub == "esr_iat") {
                    Json::Value empty;
                    Json::Value contentId = content.get("cnt_id", empty);

                    if (contentId.empty()) {
                        cout << "Content Id is empty" << endl;
                        break;
                    }

                    string cnt_id = contentId.asString();
                    int dataLen = 0;
                    const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

                    if (sub == "tts") {
                        Json::Value&& isUrl = content.get("url", empty);

                        if (isUrl.asString() == "1") {
                            std::cout << string(buffer, dataLen) << std::endl;
                        } else {
                            // cout << sub << ": " << event.getInfo() << endl;
                            string cnt_id_tts = content["cnt_id"].asString();
                            int dataLen = 0;
                            const char *data = event.getData()->getBinary(cnt_id_tts.c_str(), &dataLen);
                            int dts = content["dts"].asInt();
                            string errorinfo = content["error"].asString();



                            if (2 == dts && errorinfo == "AIUI DATA NULL")
                            {
                                //满足这两个条件的是空结果,不处理,直接丢弃
                            }
                            else if (3 == dts)
                            {
                                cout << "tts result receive dts = 3 result" << endl;
                                // mTtsFileHelper->createWriteFile("tts", ".pcm", false);
                                // mTtsFileHelper->write((const char *)data, dataLen, 0, dataLen);
                                // mTtsFileHelper->closeFile();
                            }
                            else
                            {
                                if (0 == dts)
                                {
                                    sleep(0.0000001);
                                    audioPlayer->Clear_Write();
                                    // mTtsFileHelper->createWriteFile("tts", ".pcm", false);
                                }
                                if (read_flag == 0)
                                {

                                    //each_time_read = dataLen;
#if if_print_proc_log
                                    cout << "[**import location] buffer's write location:" << buffer_source.get_tail() << endl;
#endif
                                    int res = buffer_source.RingBuff_Rx((char *)data, dataLen);
#if if_print_proc_log
                                    cout << "[**import log**] write buffer's curret length:" << buffer_source.get_length() << endl;
#endif
                                    // cout << "[**import log**] res:" << res << endl;
                                    read_and_play1();
                                    // mTtsFileHelper->write((const char *)data, dataLen, 0, dataLen);

                                    if (2 == dts)
                                    {
                                        // cout << sub << " 2 : " << event.getInfo() << endl;
                                    }
                                }
                            }

                            ofstream out("tts.pcm", std::ios::out | std::ios::app);
                            out.write(buffer, dataLen);
                            // int res = buffer_source.RingBuff_Rx((char *)buffer, dataLen);
                            // // read_and_play1();
                            // cout << "exit tts" << endl;
						}
                    } else if (sub == "nlp") {
                        string resultStr;

                        if (NULL != buffer)
                        {
                            resultStr = string(buffer, dataLen);
                            //cout<<resultStr<<endl;
                            Json::Reader resultReader;
                            Json::Value resultJson;
                            resultReader.parse(resultStr, resultJson);
                            Json::Value result_category = resultJson["intent"]["category"];
                            std::string res_str = result_category.toStyledString();
                            std::string skill_id = "\"OS1763379980.sleep1\"\n";

                            if (res_str == skill_id)
                            {
                                //cout << "///////////////////////////////////trun to sleep model" << endl;
                                // gSleep();
                            }

                            if (resultStr.size() > 20)
                            {
                                Json::Value result_question = resultJson["intent"]["text"];
                                std::string res_question_str = result_question.toStyledString();

                                Json::Value result_answer = resultJson["intent"]["answer"]["text"];
                                std::string res_answer_str = result_answer.toStyledString();
                                cout << ">>>>>问题是:" << res_question_str << endl;
                                cout << ">>>>>答案是:" << res_answer_str << endl;

                                p_node->pubQuestionMsg(res_question_str);
                                p_node->pubAnswerMsg(res_answer_str);
                                // if(res_answer_str != "null"){
                                //     stopAudio();
                                // }
                                
                                // //gTTS(res_answer_str);
                                // if (wait_for_awake_word)
                                // {
                                //     set_awake_word_once(res_question_str);
                                //     wait_for_awake_word = false;
                                //     no_tts = true;
                                //     gTTS("设置成功,你可以叫我" + res_question_str);
                                // }
                                // std::string result_hope("那给我起个4到6个字的新名字吧");
                                // if (res_answer_str.find(result_hope) != string::npos)
                                // {

                                //     wait_for_awake_word = true;
                                // }
                                // else
                                // {
                                //     no_tts = false;
                                // }

                                //gTTS(res_answer_str);
                            }
                        }
                        else
                        {
                            cout << "buffer is NULL" << endl;
                        }
                    } else {
                        if (buffer) {
                            string resultStr = string(buffer, dataLen);
                            // cout << sub << ": " << event.getInfo() << endl << resultStr << endl;
                        }
                    }
                }
            } break;

                //上传资源数据的返回结果
            case AIUIConstant::EVENT_CMD_RETURN: {
                if (AIUIConstant::CMD_BUILD_GRAMMAR == event.getArg1()) {
                    if (event.getArg2() == 0) {
                        cout << "build grammar success." << endl;
                    } else {
                        cout << "build grammar error, errcode = " << event.getArg2() << endl;
                        cout << "error reasion is " << event.getInfo() << endl;
                    }
                } else if (AIUIConstant::CMD_UPDATE_LOCAL_LEXICON == event.getArg1()) {
                    if (event.getArg2() == 0) {
                        cout << "update lexicon success" << endl;
                    } else {
                        cout << "update lexicon error, errcode = " << event.getArg2() << endl;
                        cout << "error reasion is " << event.getInfo() << endl;
                    }
                }
            } break;

            case AIUIConstant::EVENT_ERROR: {
                cout << "EVENT_ERROR:" << event.getArg1() << endl;
                cout << " ERROR info is " << event.getInfo() << endl;
            } break;
        }
    }
};

MyListener listener;

#define TEST_ROOT_DIR   "./AIUI/"
#define CFG_FILE_PATH   "./AIUI/cfg/aiui.cfg"
#define TEST_AUDIO_PATH "./AIUI/audio/test.pcm"
#define LOG_DIR         "./AIUI/log/"
#define MSC_DIR         "./AIUI/msc/"

string readFileAsString(const string& path)
{
    std::ifstream t(path, std::ios_base::in | std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

streamsize Read(istream& stream, char* buffer, streamsize count)
{
    // This consistently fails on gcc (linux) 4.8.1 with failbit set on read
    // failure. This apparently never fails on VS2010 and VS2013 (Windows 7)
    streamsize reads = stream.rdbuf()->sgetn(buffer, count);

    // This rarely sets failbit on VS2010 and VS2013 (Windows 7) on read
    // failure of the previous sgetn()
    stream.rdstate();

    // On gcc (linux) 4.8.1 and VS2010/VS2013 (Windows 7) this consistently
    // sets eofbit when stream is EOF for the conseguences  of sgetn(). It
    // should also throw if exceptions are set, or return on the contrary,
    // and previous rdstate() restored a failbit on Windows. On Windows most
    // of the times it sets eofbit even on real read failure
    stream.peek();

    return reads;
}

void MyAIUI::createAgent()
{
    if (agent) {
        return;
    }

    Json::Value paramJson;

    std::cout << CFG_FILE_PATH << std::endl;

    string fileParam = readFileAsString(CFG_FILE_PATH);

    Json::Reader reader;
    if (reader.parse(fileParam, paramJson, false)) {
        agent = IAIUIAgent::createAgent(paramJson.toStyledString().c_str(), &listener);
    }

    if (!agent) {
        std::cout << "create agent fail" << std::endl;
    } else {
        std::cout << "create agent sucess" << std::endl;
    }
}

void MyAIUI::wakeup()
{
    if (NULL != agent) {
        IAIUIMessage* wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_WAKEUP);
        agent->sendMessage(wakeupMsg);
        wakeupMsg->destroy();
    }
}

void MyAIUI::reset_wakeup()
{
    if (NULL != agent) {
        IAIUIMessage* wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_RESET_WAKEUP);
        agent->sendMessage(wakeupMsg);
        wakeupMsg->destroy();
    }
}

//停止AIUI服务，此接口是与stop()对应，调用stop()之后必须调用此接口才能继续与SDK交互，如果你没有调用过stop(),就不需要调用该接口
void MyAIUI::agentStart()
{
    if (NULL != agent) {
        IAIUIMessage* startMsg = IAIUIMessage::create(AIUIConstant::CMD_START);
        agent->sendMessage(startMsg);
        startMsg->destroy();
    }
}

//停止AIUI服务
void MyAIUI::agentStop()
{
    if (NULL != agent) {
        IAIUIMessage* stopMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP);
        agent->sendMessage(stopMsg);
        stopMsg->destroy();
    }
}

// 写入测试音频
void writeAudio()
{
    
    if (NULL != agent) {
        IAIUIMessage* stopMsg =
            IAIUIMessage::create(AIUIConstant::CMD_START_RECORD, 0, 0, "data_type=audio");
        agent->sendMessage(stopMsg);
        stopMsg->destroy();
    }
}

void MyAIUI::writeAudioFromLocal()
{
    if (NULL != agent) {
        ifstream testData(TEST_AUDIO_PATH, std::ios::in | std::ios::binary);

        if (testData.is_open()) {
            char buff[1280];
            while (testData.good() && !testData.eof()) {
                memset(buff, '\0', 1280);
                testData.read(buff, 1280);

                // textData内存会在Message在内部处理完后自动release掉
                AIUIBuffer textData = aiui_create_buffer_from_data(buff, 1280);

                IAIUIMessage* writeMsg = IAIUIMessage::create(
                    AIUIConstant::CMD_WRITE,
                    0,
                    0,
                    "data_type=audio",
                    textData);

                agent->sendMessage(writeMsg);
                writeMsg->destroy();

                // 必须暂停一会儿模拟人停顿，太快的话后端报错
                AIUI_SLEEP(40);
            }

            testData.close();

            IAIUIMessage* msg = IAIUIMessage::create(
                AIUIConstant::CMD_STOP_WRITE,
                0,
                0,
                "data_type=audio");
            agent->sendMessage(msg);
            msg->destroy();
        } else {
            printf("file[%s] open error", TEST_AUDIO_PATH);
        }
    }
}

void stopAudio()
{
    if (NULL != agent) {
        IAIUIMessage* stMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP_RECORD);
        agent->sendMessage(stMsg);
        stMsg->destroy();
    }
}

//文本语义测试接口
void MyAIUI::writeText()
{
    if (NULL != agent) {
        string text = "研究院";

        // textData内存会在Message在内部处理完后自动release掉
        AIUIBuffer textData = aiui_create_buffer_from_data(text.c_str(), text.length());

        IAIUIMessage* writeMsg = IAIUIMessage::create(
            AIUIConstant::CMD_WRITE, 0, 0, "data_type=text", textData);

        agent->sendMessage(writeMsg);
        writeMsg->destroy();
    }
}

void testTTs()
{
    if (NULL != agent) {
        string text = "敕勒川，阴山下。天似穹庐，笼盖四野。天苍苍，野茫茫。风吹草低见牛羊。";

        AIUIBuffer textData = aiui_create_buffer_from_data(text.c_str(), text.length());

        IAIUIMessage* ttsMsg = IAIUIMessage::create(
            AIUIConstant::CMD_TTS, 1, 0, "text_encoding=utf-8,vcn=xiaoyan", textData);

        agent->sendMessage(ttsMsg);
        ttsMsg->destroy();
    }
}

void MyAIUI::testTTs2()
{
    if (NULL != agent) {
        string text = "敕勒川，阴山下。天似穹庐，笼盖四野。天苍苍，野茫茫。风吹草低见牛羊。";
        // textData内存会在Message在内部处理完后自动release掉
        AIUIBuffer textData = aiui_create_buffer_from_data(text.c_str(), text.length());

        IAIUIMessage* ttsMsg = IAIUIMessage::create(
            AIUIConstant::CMD_TTS, 1, 0, "text_encoding=utf-8,vcn=xiaoyan,tts_res_type=url", textData);

        agent->sendMessage(ttsMsg);
        ttsMsg->destroy();
    }
}

void destroy()
{
    if (NULL != agent) {
        agent->destroy();
        agent = NULL;
    }
}

// void buildGrammar()
// {
//     if (NULL != agent) {
//         string grammer = readFileAsString("AIUI/asr/call.bnf");

//         auto msg =
//             IAIUIMessage::create(AIUIConstant::CMD_BUILD_GRAMMAR, 0, 0, grammer.c_str(), NULL);
//         agent->sendMessage(msg);
//         msg->destroy();
//     }
// }

void help()
{
    static string s =
        "demo示例为输入命令，调用对应的函数，使用AIUI "
        "SDK完成文本理解，语义理解，文本合成等功能，如：\r\n"
        "c命令，创建AIUI代理，与AIUI SDK交互都是通过代理发送消息的方式进行, "
        "所以第一步必须是输入该命令；\r\n"
        "s命令，初始化内部组件 第二步；\r\n"
        "w命令，发送外部唤醒命令唤醒AIUI，AIUI只有在唤醒过后才可以交互；第三步\r\n"
        "rw命令，发送外部休眠命令 AIUI进入休眠状态\r\n"
        "wrt命令，字符串文本写入sdk，sdk会返回云端识别的语义结果；\r\n"
        "wra命令， 音频文件写入sdk，sdk会返回云端识别的语义结果；\r\n"
        "tts命令，单合成示例，返回合成的音频，demo将音频保存为本地的pcm文件；\r\n"
        "stts命令，单合成示例，返回合成的音频的url保存为本地的txt文件；\r\n"
        "bg命令，构建语法\r\n"
        "help命令，显示本demo提供的示例的介绍；\r\n"
        "输入c命令后，正常情况返回结果为：\r\n"
        "EVENT_STATE:READY\r\n"
        "输入w命令后，正常情况返回结果为: \r\n"
        "EVENT_WAKEUP\r\n"
        "EVENT_STATE:WORKING\r\n"
        "听写，语义，合成等结果在onEvent函数，该函数是结果回调，请仔细研究。\r\n"
        "使用流程 c -> s -> w -> wra\r\n"
        "input cmd:";

    cout << s << endl;
}

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
 
static void GenerateMACAddress(char* mac) {
    // reference: https://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program/35242525
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return;
    };
 
    struct ifconf ifc;
    char buf[1024];
    int success = 0;
 
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        return;
    }
 
    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    struct ifreq ifr;
 
    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        } else { 
            return;
         }
    }
 
    unsigned char mac_address[6];
    if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
    
    sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
}

#define AIUI_KEY_SERIAL_NUM       "sn"
#define AIUI_KEY_AUTH_PUBLIC_KEY  "auth_public_key"
#define AIUI_KEY_AUTH_PRIVATE_KEY "auth_private_key"


void gTTS(string text)
{
    stopAudio();
    std::string vitstext = text;
    p_node->speakVitsMsg(vitstext);
	if (NULL != agent && speak_flag == 1)
	{
		Buffer *textData = Buffer::alloc(text.length());
		text.copy((char *)textData->data(), text.length());
		string paramStr = "vcn=x4_lingxiaoxuan_chat"; //x2_yezi x2_pengfei x2_qige x2_yifei
		paramStr += ",speed=40";
		paramStr += ",pitch=50";
		paramStr += ",volume=80";
		// paramStr += ",aue=speex-wb;7";

		IAIUIMessage *ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS, AIUIConstant::START, 0, paramStr.c_str(), textData);
		
		agent->sendMessage(ttsMsg);
		ttsMsg->destroy();
	}
}


// int main()
// {
// #ifdef WIN32
//     system("chcp 65001 >nul");
// #endif

//     std::cout << "Version: " << getVersion() << std::endl;

//     pthread_t pid1;
//     if (pthread_create(&pid1, NULL, com_wakeup, NULL)) {
//         return -1;
//     }

//     // help();

//     AIUISetting::setAIUIDir(TEST_ROOT_DIR);
//     AIUISetting::setMscDir(MSC_DIR);

//     AIUISetting::setNetLogLevel(aiui_debug);
//     AIUISetting::setLogLevel(aiui_debug);

//     char mac[64] = { 0 };
//     GenerateMACAddress(mac); // 设备唯一ID, 可自己重新实现

//     printf("sn is: %s\n", mac);

//     //为每一个设备设置对应唯一的SN（最好使用设备硬件信息(mac地址，设备序列号等）生成），以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数
//     AIUISetting::setSystemInfo(AIUI_KEY_SERIAL_NUM, mac);

//     audioPlayer = new AudioPlayer();
//     createAgent();
//     start();
//     wakeup();

//     string cmd;
//     while (true) {
//         cin >> cmd;

//         if (cmd == "c") {
//             cout << "createAgent" << endl;
//             createAgent();
//         } else if (cmd == "w") {
//             cout << "wakeup" << endl;
//             wakeup();
//         } else if (cmd == "rw") {
//             cout << "wakeup" << endl;
//             reset_wakeup();
//         } else if (cmd == "s") {
//             cout << "start" << endl;
//             start();
//         } else if (cmd == "st") {
//             cout << "stop" << endl;
//             stop();
//         } else if (cmd == "d") {
//             cout << "destroy" << endl;
//             destroy();
//         } else if (cmd == "e") {
//             cout << "exit" << endl;
//             break;
//         } else if (cmd == "wo") {
//             cout << "start record" << endl;
//             writeAudio();
//         } else if (cmd == "so") {
//             cout << "stop record" << endl;
//             stopAudio();
//         } else if (cmd == "wrt") {
//             cout << "writeText" << endl;
//             writeText();
//         } else if (cmd == "wra") {
//             cout << "writeAudio" << endl;
//             writeAudioFromLocal();
//         } else if (cmd == "stts") {
//             testTTs2();
//         } else if (cmd == "tts") {
//             testTTs();
//         } else if (cmd == "bg") {
//             buildGrammar();
//         } else if (cmd == "q") {
//             destroy();
//             break;
//         } else if (cmd == "h") {
//             help();
//         } else {
//             cout << "invalid cmd, input again." << endl;
//         }
//     }
// }

//接收用户输入命令，调用不同的测试接口
void MyAIUI::readCmd()
{
    printf("优拉启动成功!你现在可以跟我对话了!");
    // gTTS("优拉启动成功!你现在可以跟我对话了!");
	string cmd;

    sleep(7);
    // writeAudio();
    rclcpp::Node::SharedPtr node(p_node);
	while (true) {
		// cin >> cmd;
		// if (cmd == "q") {
		// 	break;
		// } else if (cmd == "w") {
        //     writeAudio();
		// } else if (cmd == "wo") {
        //     cout << "start record" << endl;
        //     writeAudio();
        // } else if (cmd == "so") {
        //     cout << "stop record" << endl;
        //     stopAudio();
        // } else if (cmd == "wrt") {
        //     cout << "writeText" << endl;
        //     writeText();
        // } else if (cmd == "wra") {
        //     cout << "writeAudio" << endl;
        //     writeAudioFromLocal();
        // } else if (cmd == "stts") {
        //     testTTs2();
        // } else if (cmd == "tts") {
        //     testTTs();
        // } else {
        //     cout << "invalid cmd, input again." << endl;
        // }
        rclcpp::spin_some(node);
	}
}


MyAIUI::MyAIUI()
{
    p_node = new AIUINewMic("aiui_new_mic");

    std::cout << "Version: " << getVersion() << std::endl;


    AIUISetting::setAIUIDir(TEST_ROOT_DIR);
    AIUISetting::setMscDir(MSC_DIR);
    AIUISetting::setNetLogLevel(aiui_debug);
    AIUISetting::setLogLevel(aiui_debug);
    char mac[64] = { 0 };
    GenerateMACAddress(mac); // 设备唯一ID, 可自己重新实现
    printf("sn is: %s\n", mac);

    AIUISetting::setSystemInfo(AIUI_KEY_SERIAL_NUM, mac);
    audioPlayer = new AudioPlayer();

    pthread_t pid1;
    if (pthread_create(&pid1, NULL, com_wakeup, NULL)) {
        std::cout << "pthread 'com_wakeup' creat failed!" << std::endl;
    }
}

MyAIUI::~MyAIUI()
{
    destroy();
    if (NULL != p_node) {
        delete p_node;
        p_node = NULL;
    }
    if (NULL != audioPlayer) {
        delete audioPlayer;
        audioPlayer = NULL;
    }
}

