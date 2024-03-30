#include "rclcpp/rclcpp.hpp"
#include "string.h"
#include <string>
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/int8.hpp"
#include "chat_msgs/msg/content.hpp"
#include "voice_record/voice2text.h"

// #define ANSI_COLOR_RED     "\x1b[31m"
// #define ANSI_COLOR_GREEN   "\x1b[32m"
// #define ANSI_COLOR_YELLOW  "\x1b[33m"
// #define ANSI_COLOR_RESET   "\x1b[0m"

class ListenNode : public rclcpp::Node
{
public:
	ListenNode() : Node("listen"){
		mic1_file2 = 1;
		period = 5;
		voice_path = "/home/lmx/head_ws/src/listen_v2/resource/listen.wav";
		userwords_path = "/home/lmx/head_ws/src/listen_v2/bin/userwords.txt";
		
		subscription_ = create_subscription<std_msgs::msg::Int8>("head_console", 10, std::bind(&ListenNode::ListenCallback, this, std::placeholders::_1));
		subscription_awake = create_subscription<std_msgs::msg::Int8>("awake", 10, std::bind(&ListenNode::AwakeCallback, this, std::placeholders::_1));
		publisher_ = create_publisher<chat_msgs::msg::Content>("listen", 10);
		publisher_gTTS = create_publisher<std_msgs::msg::String>("gTTS", 10);
		publisher_awake = create_publisher<std_msgs::msg::Int8>("order", 10);

		bool ini_ok = initialization(userwords_path);
		if(ini_ok)
			RCLCPP_INFO(this->get_logger(), "listen_v2 node initialize successfully......");
	}

	void AwakeCallback(const std_msgs::msg::Int8::ConstSharedPtr msg)
	{
		
		if(msg.get()->data == 1){
			RCLCPP_INFO(this->get_logger(), "唤醒成功，开始听命令 : %d", msg.get()->data);
			char * content = voiceRecorder(mic1_file2, 15, voice_path);
			std::string con = content;
			printf("%s\n",con.c_str());

			auto pub_msg = std_msgs::msg::Int8();
			pub_msg.data = 9;
			if(con.find("对话") != std::string::npos){
			// if(content == "对话"){
				pub_msg.data = 1;
			}
			if(con.find("默认表情") != std::string::npos){
				pub_msg.data = 2;
			}
			if(con.find("开始面部模仿") != std::string::npos){
				pub_msg.data = 3;
			}
			if(con.find("关闭面部模仿") != std::string::npos){
				pub_msg.data = 4;
			}
			RCLCPP_INFO(this->get_logger(), "指令: %d", pub_msg.data);
			publisher_awake->publish(pub_msg);
		}
		else{
			RCLCPP_INFO(this->get_logger(), "没有唤醒: %d", msg.get()->data);
		}
	}


	void ListenCallback(const std_msgs::msg::Int8::ConstSharedPtr msg)
	{
		RCLCPP_INFO(this->get_logger(), "head command : %d", msg.get()->data);
		const int head_command = msg.get()->data;
        if (head_command == 1){
            char * content = voiceRecorder(mic1_file2, 20, voice_path);
			if(strlen(content) > 0){
				bool gTTS = true; // TODO:--------------------
				if(gTTS){
					auto pub_msg = std_msgs::msg::String();
					pub_msg.data = content;
					RCLCPP_INFO(this->get_logger(), "gTTS content : %s", content);
					publisher_gTTS->publish(pub_msg);

					auto msg = chat_msgs::msg::Content();
					msg.content = content;
					msg.length = strlen(content);
					publisher_->publish(msg);
				}
				else{
					auto pub_msg = chat_msgs::msg::Content();
					pub_msg.content = content;
					pub_msg.length = strlen(content);
					RCLCPP_INFO(this->get_logger(), "voiceRecorder content : %s", content);
					publisher_->publish(pub_msg);
				}
			}
        }
	}

private:
    int mic1_file2;
	int period;
    const char * voice_path;
	const char * userwords_path;
	rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr subscription_;
	rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr subscription_awake;

	rclcpp::Publisher<chat_msgs::msg::Content>::SharedPtr publisher_;
	rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_gTTS;
	rclcpp::Publisher<std_msgs::msg::Int8>::SharedPtr publisher_awake;
};

int main(int argc, char *argv[])
{
	setlocale(LC_ALL,"");
	rclcpp::init(argc, argv);
	auto node = std::make_shared<ListenNode>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}