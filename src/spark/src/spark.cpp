#include "rclcpp/rclcpp.hpp"
#include "chat_msgs/msg/content.hpp"

class SparkNode : public rclcpp::Node
{
public:
	SparkNode() : Node("spark"){
        subscription_ = this->create_subscription<chat_msgs::msg::Content>("listen", 10, std::bind(&SparkNode::SparkCallback, this, std::placeholders::_1));
		publisher_ = create_publisher<chat_msgs::msg::Content>("spark", 10);
	}

	void SparkCallback(const chat_msgs::msg::Content::ConstSharedPtr msg)
	{
		RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->content.c_str());


		auto message = chat_msgs::msg::Content();
		message.content = msg->content;
		publisher_->publish(message);
	}

private:
	rclcpp::Subscription<chat_msgs::msg::Content>::SharedPtr subscription_;
	rclcpp::Publisher<chat_msgs::msg::Content>::SharedPtr publisher_;
};

int main(int argc, char* argv[])
{
    setlocale(LC_ALL,"");
	rclcpp::init(argc, argv);
	auto node = std::make_shared<SparkNode>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}