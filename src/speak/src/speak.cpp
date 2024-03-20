#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "speak_offline/speak_offline.h"
#include "chat_msgs/msg/content.hpp"


class SpeakNode : public rclcpp::Node
{
public:
	SpeakNode() : Node("speak"){
		subscription_ = create_subscription<chat_msgs::msg::Content>("listen", 10, std::bind(&SpeakNode::SpeakCallback, this, std::placeholders::_1));
		publisher_ = create_publisher<chat_msgs::msg::Content>("speak", 10);
	}

	void SpeakCallback(const chat_msgs::msg::Content msg)
	{
		RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg.content);

		auto message = chat_msgs::msg::Content();
		message.content = msg.content;
		publisher_->publish(message);
	}

private:
	rclcpp::Subscription<chat_msgs::msg::Content>::SharedPtr subscription_;
	rclcpp::Publisher<chat_msgs::msg::Content>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
	rclcpp::init(argc, argv);
	auto node = std::make_shared<SpeakNode>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}