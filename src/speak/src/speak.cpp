#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "speak_offline/speak_offline.h"
#include "chat_msgs/msg/content.hpp"
#include "speak_offline/sound_control.h"

// #define ANSI_COLOR_RED     "\x1b[31m"
// #define ANSI_COLOR_GREEN   "\x1b[32m"
// #define ANSI_COLOR_YELLOW  "\x1b[33m"
// #define ANSI_COLOR_RESET   "\x1b[0m"

class SpeakNode : public rclcpp::Node
{
public:
	SpeakNode() : Node("speak"){
		subscription_ = create_subscription<chat_msgs::msg::Content>("spark", 10, std::bind(&SpeakNode::SpeakCallback, this, std::placeholders::_1));
		// publisher_ = create_publisher<chat_msgs::msg::Content>("speak", 10);
		RCLCPP_INFO(this->get_logger(), "speak node initialize successfully......");
	}

	void SpeakCallback(const chat_msgs::msg::Content::ConstSharedPtr msg)
	{
		RCLCPP_INFO(this->get_logger(), "answer: %s", msg->content.c_str());
		const char *src_text = msg->content.c_str();
		// const char *src_text = "床前明月光，疑是地上霜。举头望明月，低头思故乡。";
		int ret = VF.VoiceOfflineGenerator(src_text, voice_save_path);
		if (MSP_SUCCESS == ret)
		{
			auto message = chat_msgs::msg::Content();
			message.content = msg->content;
			// publisher_->publish(message);
			bool play_ok = PlayWAV("/home/lmx/head_ws/src/speak/resource/speak.wav");
			if(play_ok)
				RCLCPP_INFO(this->get_logger(), "VoiceOfflineGenerator publish successfully......");
			// RCLCPP_INFO(this->get_logger(), ANSI_COLOR_GREEN "VoiceOfflineGenerator publish successfully......" ANSI_COLOR_RESET);
		}
		else{
			RCLCPP_INFO(this->get_logger(), "VoiceOfflineGenerator  fail!!!!!!  text_to_speech failed, error code: %d.\n", ret);
			// RCLCPP_ERROR(this->get_logger(), ANSI_COLOR_RED "VoiceOfflineGenerator fail!!!!!!" ANSI_COLOR_RESET);
		}
	}

private:
	VoiceOffline VF;
	const char* voice_save_path = "/home/lmx/head_ws/src/speak/resource/speak.wav";
	rclcpp::Subscription<chat_msgs::msg::Content>::SharedPtr subscription_;
	// rclcpp::Publisher<chat_msgs::msg::Content>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
	setlocale(LC_ALL,"");
	rclcpp::init(argc, argv);
	auto node = std::make_shared<SpeakNode>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}