#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "speak_offline.h"
#include "chat_msgs/msg/content.hpp"


class SpeakNode : public rclcpp::Node
{
public:
	SpeakNode() : Node("my_node"){
		subscription_ = create_subscription<std_msgs::msg::String>("TopicA", 10, std::bind(&SpeakNode::topicCallback, this, std::placeholders::_1));
		publisher_ = create_publisher<std_msgs::msg::String>("TopicB", 10);
	}

	// void topicCallback(const chat_msgs::msg::Content )
	void topicCallback(const std_msgs::msg::String::SharedPtr msg)
	{
		RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());

		auto message = std_msgs::msg::String();
		message.data = "Hello from TopicB";
		publisher_->publish(message);
	}

private:
	rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
	rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
	rclcpp::init(argc, argv);
	auto node = std::make_shared<SpeakNode>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}