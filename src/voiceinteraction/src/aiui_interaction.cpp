#include "aiui_interaction.hpp"
#include "MyListener.hpp"
#include <Constants.h>
#include "aiui/AIUIType.h"
#include <RingBuffer.h>

#include "msp_cmn.h"
#include "msp_errors.h"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int16.hpp"
#include "user_interfaces/msg/state.hpp"

using namespace std;
using namespace VA;
using namespace aiui_v2;

IAIUIAgent *agent = nullptr;
MyListener listener;
AudioRecorder *audioRecorder; // 与录音相关的
AudioPlayer *audioPlayer;	// 与音频播放相关
char awake_words[30] = "你好睿娜";//"你好睿娜"; //"你好优拉"//"海兔海兔"//小海同学
#define MAX_BUFFER 2097152 

bool if_print_proc_log = false;
bool if_print_event_log = false;
bool if_save_record_file = false; // 是否保存音频到本地
bool wait_for_awake_word = false;
bool no_tts = false;
aiui_interaction *aiui_copy = new aiui_interaction();;

static RingBuffer buffer_source(MAX_BUFFER);
int read_flag = 0;

int is_boot = 0;	   // 是否启动
int major_mic_id = -1; // 主麦克风id,在未设置主麦前，其为-1，表示无主麦
int led_id = 0;	   // 当前要点亮灯光
int mic_angle = 0;	   // 当前唤醒角度

int speak_flag = 0;

int awake_flag = 0 ;
int nlp_flag = 0 ;
int dts_flag = 1 ;


// ros2
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr iat_question_text_pub;
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr iat_answer_text_pub;
rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr mic_angle_pub;
rclcpp::Publisher<user_interfaces::msg::State>::SharedPtr speak_pos_pub;  // 说话状态
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr speech2tts_xfold_pub; // 语音识别结果发送
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr tts2speech_pub;
auto tts_tts = std_msgs::msg::String();
auto result_question_msg = std_msgs::msg::String();
auto result_answer_msg = std_msgs::msg::String();
auto mic_angle_msg = std_msgs::msg::UInt16();
auto speak_pos_msg = user_interfaces::msg::State();
static void GenerateMACAddress(char *mac); // 设备唯一ID, 可自己重新实现
void gTTS(string text);
// void gTTS(string text, aiui_interaction *aiui);

void gWakeup();
void gSleep();
void clearAudioFile(char *fileName);
aiui_interaction::aiui_interaction(/* args */)
{
}

aiui_interaction::~aiui_interaction()
{
}

string aiui_interaction::readFileAsString(const string &path)
{
	std::ifstream t(path, std::ios_base::in | std::ios::binary);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	return str;
}

void aiui_interaction::createAgent()
{
	string appid = "5cf670f6";//"03a46fe2";//"318e3336";		         // 2cc177e0                         //ＸＺＦＯ１　318e3336　　//小贝
	string key = "2f06f589ee7b663dac6e47406ea6a2e7";//"96a160473b908b2d99b2368ce6335270";//"d6310d8d5e44b73606d16f7c76ad6897"; // b0668ba28ee7fb6ada6bea8ff02f3147 //ＸＺＦＯ１　d6310d8d5e44b73606d16f7c76ad6897//
	Json::Value paramJson;
	Json::Value appidJson;

	appidJson["appid"] = appid;
	appidJson["key"] = key;
	string fileParam = FileUtil::readFileAsString(CFG_FILE_PATH);
	Json::Reader reader;
	if (reader.parse(fileParam, paramJson, false))
	{
		paramJson["login"] = appidJson;

		// for ivw support
		string wakeup_mode = paramJson["speech"]["wakeup_mode"].asString();

		// 如果在aiui.cfg中设置了唤醒模式为ivw唤醒，那么需要对设置的唤醒资源路径作处理，并且设置唤醒的libmsc.so的路径为当前路径
		if (wakeup_mode == "ivw")
		{

			// readme中有说明，使用libmsc.so唤醒库，需要调用MSPLogin()先登录
			//  string lgiparams = "appid=5d3fbe80,engine_start=ivw";
			//  string lgiparams = "appid=5d3fbe80";
			//  MSPLogin(NULL, NULL, lgiparams.c_str());
			string ivw_res_path = paramJson["ivw"]["res_path"].asString();
			if (!ivw_res_path.empty())
			{
				ivw_res_path = "fo|" + ivw_res_path;
				paramJson["ivw"]["res_path"] = ivw_res_path;
			}

			string ivw_lib_path = "libmsc.so";

			paramJson["ivw"]["msc_lib_path"] = ivw_lib_path;
		}
		// end
		const char *login_config = "appid = 5cf670f6";//5e159d11"; // 登录参数
		int ret = 0;
		ret = MSPLogin(NULL, NULL, login_config); // 第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
		if (0 != ret)
		{
			printf(">>>>>MSP登录失败：%d\n", ret);
		}
		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);
		agent = IAIUIAgent::createAgent(paramStr.c_str(), &listener);
		// globalAgent = agent;
		audioPlayer = new AudioPlayer();
		setParams();
		gTTS("系统启动成功");
	}
	else
	{
		cout << ">>>>>aiui.cfg 读取错误!" << endl;
	}
}

// 设置语义后合成时的发音人
void aiui_interaction::setParams()
{
	char *setParams = "{\"audioparams\":{\"vcn\":\"x4_lingxiaoxuan_chat\"}}";//x2_xiaojuan  x4_yeting
	IAIUIMessage *setMsg = IAIUIMessage::create(AIUIConstant::CMD_SET_PARAMS, 0, 0, setParams, NULL);
	agent->sendMessage(setMsg);
}

// 启动AIUI服务，此接口是与stop()对应，调用stop()之后必须调用此接口才能继续与SDK交互，如果你没有调用过stop(),就不需要调用该接口
void aiui_interaction::agent_start()
{
	if (NULL != agent)
	{
		IAIUIMessage *startMsg = IAIUIMessage::create(AIUIConstant::CMD_START);
		agent->sendMessage(startMsg);
		startMsg->destroy();
	}
}

// 停止AIUI服务
void aiui_interaction::agent_stop()
{
	if (NULL != agent)
	{
		IAIUIMessage *stopMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP);
		agent->sendMessage(stopMsg);
		stopMsg->destroy();
	}
}

void aiui_interaction::app_destory()
{
	if (audioRecorder != NULL)
	{
		audioRecorder->~AudioRecorder();
		audioRecorder = NULL;
	}
	agent_stop();
}
void aiui_interaction::stop_recorder()
{
	if (NULL != agent)
	{
		cout << ">>>>>关闭麦克风\n"
		<< endl;
		audioRecorder->stopRecord();
	}
}
void aiui_interaction::recorder_creat()
{
	if (agent == NULL)
	{
		cout << ">>>>>未创建麦克风录音代理\n"
		     << endl;
		return;
	}
	if (audioRecorder == NULL)
	{
		audioRecorder = new AudioRecorder(TEST_RECORD_PATH);
	}
}

void aiui_interaction::recorder_start()
{
	bool first_log = false;
	while (!AudioRecorder::if_success_boot)
	{
		if (AudioRecorder::if_awake)
		{
			break;
		}
		if (!first_log)
		{
			cout << ">>>>>请使用唤醒词唤醒" << endl;
			first_log = true;
		}
		sleep(0.5);
	}
	audioRecorder->startRecord();
	if (if_save_record_file)
	{
		audioRecorder->startRecordOriginal();
	}
}

// 接收用户输入命令，调用不同的测试接口
void aiui_interaction::readCmd()
{
	string cmd;
	while (1)
	{
		cin >> cmd;
		if (cmd == "q")
		{
			app_destory();
			break;
		}
		else if (cmd == "w")
		{
			cout << ">>>>>上一个问题是:" << result_question_msg.data << endl;
			cout << ">>>>>上一个答案是:" << result_answer_msg.data << endl;
		}
	}
}
//
void aiuiflagCallback(const std_msgs::msg::String::SharedPtr msg)
{
	std::string dataString = msg->data;
	// aiui_interaction aiui;
	if (dataString.find("开始") != dataString.npos)
	{
		gTTS("开始交互");
		aiui_copy->agent_start();
	}
	else if (dataString.find("停止") != dataString.npos)
	{
		gTTS("停止交互");
		aiui_copy->agent_stop();
	}
}

void gttsflagCallback(const std_msgs::msg::String::SharedPtr msg)
{
	aiui_copy->agent_start();
	cout << "播报内容:" << msg->data << endl;
	std::string dataString = msg->data;
	gTTS(dataString);
}

void speakendCallback(const user_interfaces::msg::State::SharedPtr msg)
{
	if(msg->speak_flag == 0) {
		// aiui_copy->recorder_start();
    		// std::cout << "Received speak flag: " << static_cast<int>(msg->speak_flag) << std::endl;
  	}
	// 
}
int main(int argc, char *argv[])
{

	rclcpp::init(argc, argv);
	// rclcpp::Rate loop_rate(10);
	auto node = rclcpp::Node::make_shared("iat_publish");
	rclcpp::Subscription<std_msgs::msg::String>::SharedPtr aiuiflag_sub;
	rclcpp::Subscription<std_msgs::msg::String>::SharedPtr gttsflag_sub;
	rclcpp::Subscription<user_interfaces::msg::State>::SharedPtr speakend_sub;
	iat_question_text_pub = node->create_publisher<std_msgs::msg::String>("iat_question_text", 10);
	iat_answer_text_pub = node->create_publisher<std_msgs::msg::String>("iat_answer_text", 10);
	mic_angle_pub = node->create_publisher<std_msgs::msg::UInt16>("voice_angle", 10);
	speak_pos_pub = node->create_publisher<user_interfaces::msg::State>("speak_pos", 10);
	// gtts 播报请求
	speakend_sub = node->create_subscription<user_interfaces::msg::State>("speak_state", 1000, speakendCallback);
	aiuiflag_sub = node->create_subscription<std_msgs::msg::String>("voice_interaction", 1000, aiuiflagCallback); // subscribe answer reault
	gttsflag_sub = node->create_subscription<std_msgs::msg::String>("tts2speech_xfold", 1000, gttsflagCallback);
	
    speech2tts_xfold_pub = node->create_publisher<std_msgs::msg::String>("speech2tts_xfold", 10); // 
	tts2speech_pub = node->create_publisher<std_msgs::msg::String>("tts2tts_xfold", 10);	    //
	
    result_question_msg.data = " ";
	result_answer_msg.data = " ";

	std::cout << "Version: " << getVersion() << std::endl;
	AIUISetting::setAIUIDir(TEST_ROOT_DIR);
	AIUISetting::setMscDir(MSC_DIR);
	AIUISetting::setNetLogLevel(aiui_debug);
	AIUISetting::setLogLevel(aiui_debug);
	char mac[64] = {0};
	GenerateMACAddress(mac); // 设备唯一ID, 可自己重新实现
	printf("sn is: %s\n", mac);

	// 为每一个设备设置对应唯一的SN（最好使用设备硬件信息(mac地址，设备序列号等）生成），以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数
	AIUISetting::setSystemInfo(AIUI_KEY_SERIAL_NUM, mac);

	// aiui_interaction aiui;
	cout << ">>>>>创建AIUI代理Agent\n"
	     << endl;
	aiui_copy->createAgent();
	cout << ">>>>>正在准备开启麦克风" << endl;
	aiui_copy->recorder_creat();
	sleep(2);
	set_awake_word(awake_words);
	cout << ">>>>>开启录音" << endl;
	gTTS("请使用唤醒词唤醒");
	aiui_copy->recorder_start();
	// aiui_copy->stop_recorder();

	rclcpp::spin(node);
	// aiui.readCmd();
}

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>

static void GenerateMACAddress(char *mac)
{
	// reference: https://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program/35242525
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		return;
	};

	struct ifconf ifc;
	char buf[1024];
	int success = 0;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
	{
		return;
	}

	struct ifreq *it = ifc.ifc_req;
	const struct ifreq *const end = it + (ifc.ifc_len / sizeof(struct ifreq));
	struct ifreq ifr;

	for (; it != end; ++it)
	{
		strcpy(ifr.ifr_name, it->ifr_name);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
		{
			if (!(ifr.ifr_flags & IFF_LOOPBACK))
			{ // don't count loopback
				if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
				{
					success = 1;
					break;
				}
			}
		}
		else
		{
			return;
		}
	}

	unsigned char mac_address[6];
	if (success)
		memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
}

void gWakeup()
{
	if (NULL != agent)
	{
		IAIUIMessage *wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_WAKEUP);
		agent->sendMessage(wakeupMsg);
		wakeupMsg->destroy();
	}
}

void gSleep()
{
	gTTS("我去休息了");
	if (NULL != agent)
	{
		IAIUIMessage *sleepMsg = IAIUIMessage::create(AIUIConstant::CMD_RESET_WAKEUP);
		agent->sendMessage(sleepMsg);
		sleepMsg->destroy();
	}
	audioPlayer->Clear_Write();
}

void gTTS(string text)
{
	// aiui_interaction aiui;
	// aiui_copy->stop_recorder();
    cout << ">>>>>gTTS:" << text << endl;
    dts_flag = 1;
	if (NULL != agent) //&& speak_flag == 1
	{
		Buffer *textData = Buffer::alloc(text.length());
		text.copy((char *)textData->data(), text.length());
		string paramStr = "vcn=x4_lingxiaoxuan_chat"; // x2_yezi x2_pengfei x2_qige x2_yifei
		paramStr += ",speed=40";
		paramStr += ",pitch=50";
		paramStr += ",volume=80";
		paramStr += ",aue=speex-wb;7";

		IAIUIMessage *ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS, AIUIConstant::START, 0, paramStr.c_str(), textData);
		

		agent->sendMessage(ttsMsg);
		ttsMsg->destroy();
	}
}

void clearAudioFile(char *fileName)
{
	FILE *pFile = fopen(fileName, "w+");
	unsigned char *buf;
	fwrite(buf, sizeof(char), 0, pFile);
	fclose(pFile);
	pFile = NULL;
}

void *read_and_play1() // void *
{
	if (buffer_source.get_length() > 0)
	{
		read_flag = 1; // pthread_mutex_unlock(&mut);
		char data_to_speech[200000];
		int lenth = buffer_source.get_length();
		int res1 = buffer_source.RingBuff_Tx(data_to_speech, buffer_source.get_length());
		read_flag = 0;
		if (res1 != -1)
		{
			audioPlayer->Write((unsigned char *)data_to_speech, res1);
		}
	}
}

void set_awake_word_once(std::string awake_word)
{
	for (int i = 0; i < 5; i++)
	{
		awake_word.pop_back();
	}
	awake_word.erase(0, 1);
	set_awake_word((char *)awake_word.c_str());
}

int AudioRecorder::business_proc_callback(business_msg_t businessMsg)
{
	int res = 0;
	char fileName[] = PCM_FILE_PATH;
	char fileName_ori[] = ORIPCM_FILE_PATH;
	if (if_print_proc_log)
	{
		printf("business proc modId = %d, msgId = %d, size = %d\n", businessMsg.modId, businessMsg.msgId, businessMsg.length);
	}
	switch (businessMsg.modId)
	{
	case 0x01:
		if (businessMsg.msgId == 0x01) //
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				// printf(">>>>>您已开启录音\n");
			}
		}
		else if (businessMsg.msgId == 0x02) // recorder返回的降噪后的音频信息
		{

			int len = PCM_MSG_LEN;
			Buffer *buffer = Buffer::alloc(len); // 申请的内存会在sdk内部释放

			if (if_print_proc_log)
			{
				if (buffer == NULL)
				{
					cout << "buffer is null" << endl;
				}
				else
				{
					cout << "buffer alloced successfully" << endl;
				}
				cout << "data size:" << businessMsg.length << "len:" << len << endl;
			}
			if (businessMsg.length < len)
			{
				len = businessMsg.length;
			}
			// sleep(1);
			memcpy(buffer->data(), businessMsg.data, len);

			IAIUIMessage *writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE, 0, 0, "data_type=audio,sample_rate=16000", buffer);

			if (NULL != agent)
			{
				agent->sendMessage(writeMsg);
			}
			else
			{
				cout << ">>>>>agent未创建\n"
				     << endl;
			}

			writeMsg->destroy();

			if (if_save_record_file && businessMsg.length > 0)
			{
				get_denoised_sound("./aiui_vvui_deno.pcm", businessMsg.data);
			}

			// ssize_t ret = Buffer::dealloc(buffer);
		}
		else if (businessMsg.msgId == 0x03)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				// printf(">>>>>您已停止录音\n");
			}
		}
		else if (businessMsg.msgId == 0x06) // recorder返回的8路的原始音频
		{
			if (if_save_record_file)
			{ //
				get_original_sound("./aiui_vvui_ori.pcm", businessMsg.data);
			}
		}

		else if (businessMsg.msgId == 0x04)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf(">>>>>开/关原始音频成功\n");
			}
		}
		else if (businessMsg.msgId == 0x05)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				// printf(">>>>>识音角度设置成功\n");
			}
		}
		else if (businessMsg.msgId == 0x07)
		{
			unsigned char key[] = "beam";
			int major_id = whether_set_succeed(businessMsg.data, key);
			printf(">>>>>主麦克风id为%d号麦克风\n", major_id);
		}
		else if (businessMsg.msgId == 0x08)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf("\n>>>>>设置主麦克风成功\n");
			}
		}
		else if (businessMsg.msgId == 0x09)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf("\n>>>>>设置灯光成功\n");
			}
		}
		break;
	case 0x02:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key1[] = "beam";
			unsigned char key2[] = "angle";
			major_mic_id = get_awake_mic_id(businessMsg.data, key1);
			mic_angle = get_awake_mic_angle(businessMsg.data, key2);
			if (major_mic_id <= 5 && major_mic_id >= 0 && mic_angle <= 360 && mic_angle >= 0)
			{
				printf("--------------\n");
                if_awake = 1;
				led_id = get_led_based_angle(mic_angle);
				int ret1 = set_major_mic_id(major_mic_id);
				int ret2 = set_target_led_on(led_id);
				printf(">>>>>第%d个麦克风被唤醒\n", major_mic_id);
				printf(">>>>>唤醒角度为:%d\n", mic_angle);
				printf(">>>>>已点亮%d灯\n", led_id);
				
                if (awake_flag == 0){
                    aiui_copy->agent_start();
                    awake_flag = 1; 
                    gWakeup();
                }
                
				gTTS("嗯,我在");
				
                if (if_save_record_file)
				{
					clearAudioFile("./aiui_vvui_deno.pcm");
					clearAudioFile("./aiui_vvui_ori.pcm");
				}
				mic_angle_msg.data = mic_angle;
				speak_pos_msg.target_pos = mic_angle;
				speak_pos_pub->publish(speak_pos_msg);
				mic_angle_pub->publish(mic_angle_msg);
			}
		}
		else if (businessMsg.msgId == 0x08)
		{
			unsigned char key1[] = "errstring";
			int result = whether_set_awake_word(businessMsg.data, key1);
			if (result == 0)
			{
				printf(">>>>>唤醒词设置成功\n");
			}
			else
			{
				printf(">>>>>唤醒词设置失败\n");
			}
		}
		break;
	case 0x03:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "status";
			int status = whether_set_succeed(businessMsg.data, key);
			char protocol_version[40];
			int ret = get_protocol_version(businessMsg.data, protocol_version);
			printf(">>>>>麦克风%s,软件版本为:%s,协议版本为:%s\n", (status == 0 ? "正常工作" : "正在启动"), get_software_version(), protocol_version);
			if (status == 1) // 传输配置文件
			{
				char *fileName = CONFIG_FILE_PATH;
				send_resource_info(fileName, 0);
			}
			else if (status == 0) // 已完成启动
			{
				AudioRecorder::if_success_boot = true;
				is_boot = 1;
			}
		}
		break;
	case 0x04:
		if (businessMsg.msgId == 0x01)
		{
			whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x02) // 3605D请求下载文件
		{
			char *fileName = SOURCE_FILE_PATH;
			send_resource_info(fileName, 1);
		}
		else if (businessMsg.msgId == 0x03) // 文件接收结果
		{
			whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x05) // 3605D请求下载文件
		{
			char fileName[] = SOURCE_FILE_PATH;
			send_resource(businessMsg.data, fileName, 1);
		}
		else if (businessMsg.msgId == 0x04) // 3605D上报升级结果
		{
			whether_upgrade_succeed(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x08) // 获取升级配置文件
		{
			printf("config.json: %s", businessMsg.data);
		}
		break;
	default:
		break;
	}
	return 0;
}

string mSyncSid;
// 事件回调接口，SDK状态，文本，语义结果等都是通过该接口抛出
void MyListener::onEvent(const IAIUIEvent &event)
{
	if (if_print_event_log)
	{
		cout << "onEvent" << endl;
	}
	switch (event.getEventType())
	{
	cout << ">>>>>event.getEventType():" << event.getEventType() << endl;
    // SDK 状态回调
	case AIUIConstant::EVENT_STATE: // 服务状态事件
	{
		switch (event.getArg1())
		{
		case AIUIConstant::STATE_IDLE:
		{
			cout << ">>>>>EVENT_STATE:"
			     << "IDLE" << endl;
		}
		break;

		case AIUIConstant::STATE_READY:
		{
			cout << ">>>>>EVENT_STATE:"
			     << "READY" << endl;
		}
		break;

		case AIUIConstant::STATE_WORKING:
		{
			cout << ">>>>>EVENT_STATE:"
			     << "WORKING" << endl;
		}
		break;
		}
	}
	break;

	/*唤醒事件回调唤醒事件
    arg1字段取值(自1051版本开始支持)：
    0 => 内部语音唤醒
    1 => 外部手动唤醒（外部发送CMD_WAKEUP）。
    info字段为唤醒结果JSON字符串。*/
	case AIUIConstant::EVENT_WAKEUP:
	{
		// cout << "EVENT_WAKEUP:" << event.getInfo() << endl;
		cout << ">>>>>麦克风已唤醒，可进行对话" << endl;
	}
	break;

	// 休眠事件回调准备休眠事件,当出现交互超时，服务会先抛出准备休眠事件，用户可在接收到该事件后的10s内继续交互。若10s内存在有效交互，则重新开始交互计时；若10s内不存在有效交互，则10s后进入休眠状态。
	case AIUIConstant::EVENT_SLEEP:
	{
		cout << ">>>>>麦克风准备进入休眠模式，将休眠" << endl;
		// cout <<"VENT_SLEEP:arg1=" << event.getArg1() << endl;
	}
	break;

	// VAD事件回调，如找到前后端点VAD事件当检测到输入音频的前端点后，会抛出该事件，用arg1标识前后端点或者音量信息:0(前端点)、1(音量)、2(后端点)、3（前端点超时）当arg1取值为1时，arg2为音量大小。
	case AIUIConstant::EVENT_VAD:
	{
		switch (event.getArg1())
		{
		case AIUIConstant::VAD_BOS:
		{
			// cout << "EVENT_VAD:" << "BOS" << endl;
			cout << ">>>>>检测到对话开始" << endl;
		}
		break;

		case AIUIConstant::VAD_EOS:
		{
			cout << ">>>>>检测到本轮对话结束" << endl;
		}
		break;

		case AIUIConstant::VAD_VOL:
		{
			// event.getData
			// cout << "EVENT_VAD:" << "VOL " << endl;
			// cout << "EVENT_VAD:" << "EVENT_INFO:" << event.getInfo() << endl;
			// cout << "EVENT_VAD:" << "GET_ARG2  :" << event.getArg2() << endl;
			// cout << "EVENT_VAD:" << "GET_ARG2  :" << event.getArg2() << endl;
		}
		break;
		}

		// cout << "EVENT_VAD:"<< "EVENT_RESULT:" << event.getData()->getString("result", "") << endl;
	}
	break;
	// case AIUIConstant::EVENT_TTS:{
	// 	cout << "AIUIConstant::EVENT_TTS" << endl;

	// }
	// 最重要的结果事件回调
	case AIUIConstant::EVENT_RESULT: // data字段携带结果数据，info字段为描述数据的JSON字符串。
	{
		// cout << "AIUIConstant::EVENT_RESULT" << endl;
		Json::Value bizParamJson;
		Json::Reader reader;

		if (!reader.parse(event.getInfo(), bizParamJson, false))
		{
			cout << "parse error!" << endl
			     << event.getInfo() << endl;
			break;
		}
		Json::Value data = (bizParamJson["data"])[0];
		Json::Value params = data["params"];
		Json::Value content = (data["content"])[0];
		string sub = params["sub"].asString();
		// sub字段表示结果的类别，如iat听写，nlp语义结果
#if if_print_proc_log
		cout << ">>>>>当前事件状态:" << data << endl;
#endif
		// cout <<  << endl;
		if (sub == "nlp")
		{
			Json::Value empty;
			Json::Value contentId = content.get("cnt_id", empty);

			if (contentId.empty())
			{
				cout << "Content Id is empty" << endl;
				break;
			}

			string cnt_id = contentId.asString();
			int dataLen = 0;
			const char *buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);
			string resultStr;
			//---------------------

			//+++++++++++++++++++++
			if (NULL != buffer)
			{
				resultStr = string(buffer, dataLen);
				// cout<<resultStr<<endl;
				Json::Reader resultReader;
				Json::Value resultJson;
				resultReader.parse(resultStr, resultJson);
				Json::Value result_category = resultJson["intent"]["category"];
				std::string res_str = result_category.toStyledString();
				std::string skill_id = "\"OS1763379980.sleep1\"\n";

				if (res_str == skill_id)
				{
					// cout << "///////////////////////////////////trun to sleep model" << endl;
					gSleep();
				}

				if (resultStr.size() > 20)
				{
					Json::Value result_question = resultJson["intent"]["text"];
					// Json::Value result_question = resultJson["intent"]["question"]["text"];
					std::string res_question_str = result_question.toStyledString();

					Json::Value result_answer = resultJson["intent"]["answer"]["text"];
					std::string res_answer_str = result_answer.toStyledString();
					cout << resultJson << endl;
					cout << ">>>>>问题是:" << res_question_str << endl;
					cout << ">>>>>答案是:" << res_answer_str << endl;

					result_question_msg.data = res_question_str;
					result_answer_msg.data = res_answer_str;
					iat_question_text_pub->publish(result_question_msg);
					iat_answer_text_pub->publish(result_answer_msg);
					
                    
                    // xf 语音合成 唤醒一次交互一次
                    if (awake_flag == 1){
                        nlp_flag = 1;
                        if (result_answer.isNull())
                        {
                            // gTTS("这个问题我还没有学会");
                            // speech2tts_xfold_pub->publish(result_question_msg);
                            break;
                        }
                        // gTTS(res_answer_str);
                    }
                    // 
                    // tts_tts.data = res_answer_str;
	                // tts2speech_pub->publish(tts_tts);

					
                    
                    // 
					if (wait_for_awake_word)
					{
						set_awake_word_once(res_question_str);
						wait_for_awake_word = false;
						no_tts = true;
						gTTS("设置成功,你可以叫我" + res_question_str);
					}
					std::string result_hope("那给我起个4到6个字的新名字吧");
					if (res_answer_str.find(result_hope) != string::npos)
					{

						wait_for_awake_word = true;
					}
					else
					{
						no_tts = false;
					}

					// gTTS(res_answer_str);
				}
			}
			else
			{
				cout << "buffer is NULL" << endl;
			}
		}
		else if (sub == "tts" && !no_tts) //&& speak_flag == 1
		{
			// aiui_copy->stop_recorder();
			static int count_tts = 0;
			// cout << event.getInfo() << endl;

			string cnt_id = content["cnt_id"].asString();

			int dataLen = 0;
			const char *data = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

			int dts = content["dts"].asInt();
            cout << ">>>>>dts:" << dts << endl;
            cout << ">>>>>nlp_flag:" << nlp_flag << endl;
            cout << ">>>>>dts_flag:" << dts_flag << endl;
            cout << ">>>>>awake_flag:" << awake_flag << endl;

			string errorinfo = content["error"].asString();

			if (2 == dts && errorinfo == "AIUI DATA NULL")
			{
				// 满足这两个条件的是空结果,不处理,直接丢弃
			}
			else if (3 == dts)
			{
				// cout << "tts result receive dts = 3 result" << endl;
				mTtsFileHelper->createWriteFile("tts", ".pcm", false);
				mTtsFileHelper->write((const char *)data, dataLen, 0, dataLen);
				mTtsFileHelper->closeFile();
			}
			else
			{
				if (0 == dts)
				{
					sleep(0.0000001);
					audioPlayer->Clear_Write();
					mTtsFileHelper->createWriteFile("tts", ".pcm", false);
				}
				if (read_flag == 0 && dts_flag == 1)
				{

					// each_time_read = dataLen;
#if if_print_proc_log
					cout << "[**import location] buffer's write location:" << buffer_source.get_tail() << endl;
#endif
					int res = buffer_source.RingBuff_Rx((char *)data, dataLen);
#if if_print_proc_log
					cout << "[**import log**] write buffer's curret length:" << buffer_source.get_length() << endl;
#endif
					read_and_play1();
					mTtsFileHelper->write((const char *)data, dataLen, 0, dataLen);

					if (2 == dts)
					{
						mTtsFileHelper->closeWriteFile();
                        
                        dts_flag = 0;
                        if (nlp_flag == 1){
                            awake_flag = 0;
                            nlp_flag = 0;
                            aiui_copy->agent_stop();
                        }
                        cout << ">>>>>new_nlp_flag:" << nlp_flag << endl;
                        cout << ">>>>>new_dts_flag:" << dts_flag << endl;
                        cout << ">>>>>new_awake_flag:" << awake_flag << endl;
                        printf("==============\n");
					}
				}
			}
		}
	}
	break;

	/*上传资源数据的返回结果某条CMD命令对应的返回事件,对于除CMD_GET_STATE外的有返回的命令，都会返回该事件，
    用arg1标识对应的CMD命令，arg2为返回值，0表示成功，
    info字段为描述信息。*/
	case AIUIConstant::EVENT_CMD_RETURN:
	{
		// cout << "onEvent --> EVENT_CMD_RETURN: arg1 is " << event.getArg1() << endl;
		if (AIUIConstant::CMD_SYNC == event.getArg1())
		{
			int retcode = event.getArg2();
			int dtype = event.getData()->getInt("sync_dtype", -1);

			// cout << "onEvent --> EVENT_CMD_RETURN: dtype is " << dtype << endl;

			switch (dtype)
			{
			case AIUIConstant::SYNC_DATA_STATUS:
				break;

			case AIUIConstant::SYNC_DATA_ACCOUNT:
				break;

			case AIUIConstant::SYNC_DATA_SCHEMA:
			{
				string sid = event.getData()->getString("sid", "");
				string tag = event.getData()->getString("tag", "");

				mSyncSid = sid;

				if (AIUIConstant::SUCCESS == retcode)
				{
					cout << "sync schema success." << endl;
				}
				else
				{
					cout << "sync schema error=" << retcode << endl;
				}

				cout << "sid=" << sid << endl;
				cout << "tag=" << tag << endl;
			}
			break;

			case AIUIConstant::SYNC_DATA_SPEAKABLE:
				break;

			case AIUIConstant::SYNC_DATA_QUERY: // 查询结果
			{
				if (AIUIConstant::SUCCESS == retcode)
				{
					cout << "sync status success" << endl;
				}
				else
				{
					cout << "sync status error=" << retcode << endl;
				}
			}
			break;
			}
		}
		else if (AIUIConstant::CMD_QUERY_SYNC_STATUS == event.getArg1())
		{
			int syncType = event.getData()->getInt("sync_dtype", -1);
			if (AIUIConstant::SYNC_DATA_QUERY == syncType)
			{
				string result = event.getData()->getString("result", "");
				cout << "result:" << result << endl;

				if (0 == event.getArg2())
				{
					cout << "sync status:success." << endl;
				}
				else
				{
					cout << "sync status error:" << event.getArg2() << endl;
				}
			}
		}
		else if (AIUIConstant::CMD_BUILD_GRAMMAR == event.getArg1())
		{
			if (event.getArg2() == 0)
			{
				cout << "build grammar success." << endl;
			}
			else
			{
				cout << "build grammar error, errcode = " << event.getArg2() << endl;
				cout << "error reasion is " << event.getInfo() << endl;
			}
		}
		else if (AIUIConstant::CMD_UPDATE_LOCAL_LEXICON == event.getArg1())
		{
			if (event.getArg2() == 0)
			{
				cout << "update lexicon success" << endl;
			}
			else
			{
				cout << "update lexicon error, errcode = " << event.getArg2() << endl;
				cout << "error reasion is " << event.getInfo() << endl;
			}
		}
	}
	break;

	case AIUIConstant::EVENT_ERROR: // arg1字段为错误码，info字段为错误描述信息。
	{
		cout << "EVENT_ERROR:" << dec << event.getArg1() << endl;
		cout << " ERROR info is " << event.getInfo() << endl;
	}
	break;
	}
}