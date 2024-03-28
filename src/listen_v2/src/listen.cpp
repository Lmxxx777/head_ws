#include "rclcpp/rclcpp.hpp"
#include "string.h"
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
		voice_path = "/home/imi/head_ws/src/listen_v2/resource/listen.wav";
		userwords_path = "/home/imi/head_ws/src/listen_v2/bin/userwords.txt";
		subscription_ = create_subscription<std_msgs::msg::Int8>("head_console", 10, std::bind(&ListenNode::ListenCallback, this, std::placeholders::_1));
		publisher_ = create_publisher<chat_msgs::msg::Content>("listen", 10);
		bool ini_ok = initialization(userwords_path);
		if(ini_ok)
			RCLCPP_INFO(this->get_logger(), "listen_v2 node initialize successfully......");
	}

	void ListenCallback(const std_msgs::msg::Int8::ConstSharedPtr msg)
	{
		RCLCPP_INFO(this->get_logger(), "head command : %d", msg.get()->data);
		const int head_command = msg.get()->data;
        if (head_command == 1){
            char * content = voiceRecorder(mic1_file2, voice_path);
			if(strlen(content) > 0){
				auto pub_msg = chat_msgs::msg::Content();
				pub_msg.content = content;
				pub_msg.length = strlen(content);
				RCLCPP_INFO(this->get_logger(), "voiceRecorder content : %s", content);
				publisher_->publish(pub_msg);
			}
        }
	}

private:
    int mic1_file2;
    const char * voice_path;
	const char * userwords_path;
	rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr subscription_;
	rclcpp::Publisher<chat_msgs::msg::Content>::SharedPtr publisher_;
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