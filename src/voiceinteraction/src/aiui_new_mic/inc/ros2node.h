#ifndef _ROS2NODE_H_
#define _ROS2NODE_H_

#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int16.hpp"
#include "std_msgs/msg/int16.hpp"
#include "user_interfaces/msg/state.hpp"

using namespace std;

class AIUINewMic : public rclcpp::Node
{
public:
    AIUINewMic(std::string name) : Node(name) {
        RCLCPP_INFO(this->get_logger(), "节点创建 %s.", name.c_str());
        iat_question_text_pub = this->create_publisher<std_msgs::msg::String>("iat_question_text", 10);
	    iat_answer_text_pub   = this->create_publisher<std_msgs::msg::String>("iat_answer_text", 10);
	    mic_angle_pub   	  = this->create_publisher<std_msgs::msg::UInt16>("voice_angle", 10);
	    speak_state_pub       = this->create_publisher<user_interfaces::msg::State>("speak_state",10);
        gpt_pub               = this->create_publisher<std_msgs::msg::String>("chat_sub", 10); //send gpt
        vits_pub              = this->create_publisher<std_msgs::msg::String>("chat_pub", 10); //send vits
        body_pub     = this->create_publisher<std_msgs::msg::String>("voiceCmd", 10);//手臂动作发布
        ulaa_state_sub = create_subscription<user_interfaces::msg::State>(
        "state",100,std::bind(&AIUINewMic::updateState,this,std::placeholders::_1));
        tts_text_sub = create_subscription<std_msgs::msg::String>(
        "tts_text",100,std::bind(&AIUINewMic::subTtsText,this,std::placeholders::_1));
        // 创建定时器，500ms为周期，定时发布
        // timer_ = this->create_wall_timer(std::chrono::milliseconds(20), std::bind(&AIUINewMic::timer_callback, this));
    }

    void pubMicAngleMsg(uint16_t angle) 
    {
        mic_angle_msg.data = angle;
        mic_angle_pub->publish(mic_angle_msg);
    }
    void pubQuestionMsg(std::string msg) 
    {
        result_question_msg.data = msg;
        iat_question_text_pub->publish(result_question_msg);
        gpt_pub -> publish(result_question_msg); //send2gpt
    }
    void pubAnswerMsg(std::string msg) 
    {
        result_answer_msg.data = msg;
        iat_answer_text_pub->publish(result_answer_msg);
    }
    void updateState(user_interfaces::msg::State::SharedPtr msg)
    {
        // if(msg->target_status == "到达仿生人形机器人实验室")
        // {
        school_msg = msg->target_status;
        // cout << school_msg << endl;
        // }
    }
    void speakVitsMsg(std::string msg) 
    {
        speak_msg.data = msg;
        vits_pub->publish(speak_msg);
    }

    std::string school_msg;
    user_interfaces::msg::State ulaa_state; 
    std_msgs::msg::String string_msg_body;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr body_pub;//手臂动作发布
    rclcpp::Publisher<user_interfaces::msg::State>::SharedPtr speak_state_pub; // 说话状态发布
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr tts_text_sub;//tts—text订阅
    void subTtsText(std_msgs::msg::String::SharedPtr msg);
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr iat_question_text_pub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr iat_answer_text_pub;
    rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr mic_angle_pub;
    rclcpp::Subscription<user_interfaces::msg::State>::SharedPtr ulaa_state_sub;//ulaa状态订阅
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gpt_pub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr vits_pub;
    // 声名定时器指针
    rclcpp::TimerBase::SharedPtr timer_;

    std_msgs::msg::String result_question_msg;
    std_msgs::msg::String result_answer_msg;
    std_msgs::msg::UInt16 mic_angle_msg;
    user_interfaces::msg::State speak_pos_msg;
    std_msgs::msg::String speak_msg;


    void timer_callback() {

    }
};



#endif /* _ROS2NODE_H_ */
