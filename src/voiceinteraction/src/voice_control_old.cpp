#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>   

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "chat_msgs/msg/state.hpp"

#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#define START 1
#define END 6

geometry_msgs::msg::Twist twist;
rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub;
std_msgs::msg::String string_msg;
std_msgs::msg::String string_msg_face;
std_msgs::msg::String string_msg_base;
std_msgs::msg::String string_msg_body;
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr tts_text_pub;
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr face_pub;
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr base_pub;
rclcpp::Publisher<std_msgs::msg::String>::SharedPtr body_pub;
int cmd_type = 0;
uint8_t step = 0;   //场景流程
uint8_t rc_num = 0; //遥控器参数
uint8_t rc_numl = 0;//上一次值

void iattextQuestionCallback(const std_msgs::msg::String::SharedPtr msg)
{
    std::cout<< "收到问题" << msg->data.c_str() << std::endl;

    const char* text;
    const char* face_text;
    const char* base_text;
    const char* body_text;
    text = " ";
    face_text = " ";
    base_text = " ";
    body_text = " ";

   std::string dataString = msg->data;
    //－－－－－－－－底盘功能指令－－－－－－－
    if(dataString.find("前进") != dataString.npos)
    {
        char forwordString[40] = "开始前进";
        text = forwordString;
        base_text = "前进";
        std::cout<<text<<std::endl;
        cmd_type = 1;
    }
    else if(dataString.find("后退") != dataString.npos)
    {
        char backwordString[40] = "开始后退";
        text = backwordString;
        base_text = "后退";
        std::cout<<text<<std::endl;
        cmd_type = 2;
    }
    else if(dataString.find("左转") != dataString.npos)
    {
        char leftString[40] = "开始左转";
        text = leftString;
        base_text = "左转";
        std::cout<<text<<std::endl;
        cmd_type = 3;
    }
    else if(dataString.find("右转") != dataString.npos)
    {
        char rightdString[40] = "开始右转";
        text = rightdString;
        base_text = "右转";
        std::cout<<text<<std::endl;
        cmd_type = 4;
    }
    else if(dataString.find("停止") != dataString.npos)
    {
        char stopString[40] = "收到，我正在停止!";
        text = stopString;
        base_text = "停止";
        body_text = "停止";
        std::cout<<text<<std::endl;
        cmd_type = 5;
    }
    else if(dataString.find("左前方") != dataString.npos)
    {

        base_text = "左前方";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    } 
    else if(dataString.find("右前方") != dataString.npos)
    {

        base_text = "右前方";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    else if(dataString.find("暂停") != dataString.npos)
    {

        base_text = "暂停";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    } 
    else if(dataString.find("继续") != dataString.npos)
    {

        base_text = "继续";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    else if(dataString.find("定位") != dataString.npos)//定位充电桩
    {

        base_text = "定位";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    else if(dataString.find("关机") != dataString.npos)
    {

        base_text = "关机";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    else if(dataString.find("重启") != dataString.npos)
    {

        base_text = "重新启动";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    else if(dataString.find("充电") != dataString.npos)
    {

        base_text = "充电桩";
        std::cout<<base_text<<std::endl;
        cmd_type = 6;
    }
    //表情初始化
    else if(dataString.find("初始化") != dataString.npos)
    {
        char eyeString[] = "初始化";
        face_text = eyeString;
        std::cout<<face_text<<std::endl;
        cmd_type = 6;
    }

//－－－－－－－－－手臂动作－－－－－－－－－－－
    else if(dataString.find("您好") != dataString.npos)
    {
        char mouthString[] = "收到";
        face_text = mouthString;
        text = "你好呀！";
        char bodyString[] = "你好";
        body_text = bodyString;
        std::cout<<face_text<<std::endl;
        cmd_type = 18;
    }
    else if(dataString.find("再见") != dataString.npos)
    {
        char mouthString[] = "收到";
        face_text = mouthString;
        text = "下次见";
        body_text = "再见";
        std::cout<<face_text<<std::endl;
        cmd_type = 18;
    }
    else if(dataString.find("握手") != dataString.npos)
    {
        char mouthString[] = "收到";
        face_text = mouthString;
        text = "my,friend";
        body_text = "握手";
        std::cout<<face_text<<std::endl;
        cmd_type = 18;
    }

//－－－－－－－视频拍摄－－－－－－－－－－－－－
    
    //语音控制地点修改部分
    //开始介绍------到大门
    else if((dataString.find("准备") != dataString.npos))
    {
        step = START;
        text = "前往初始站点";
        base_text = "初始站点";
        std::cout<<"到初始站点"<<std::endl;
    }
    else if((dataString.find("介绍一下这里") != dataString.npos))
    {
        step = 2;
        text = "尊敬的各位领导各位来宾:大家好，我是展厅讲解员尤拉。被你发现了，我和人类不一样，我是一个机器人。今天将由我带您参观我的家，也就是中原动力的展厅。";
        base_text = "展厅";
        std::cout<<"到展厅"<<std::endl;
    }
    //双足机器人
    else if((dataString.find("行走") != dataString.npos)) //|| (dataString.find("机器人") != dataString.npos)
    {
        step = 3;
        text = "接下来，我将带您徜徉在机器人的海洋中，请跟紧我！现在进入您视野的是双足行走家族。";
        base_text = "双足机器人";
        std::cout<<"双足机器人"<<std::endl;
    }
    //关节模组
    else if((dataString.find("关节") != dataString.npos))
    {
        step = 4;
        text = "接下来，我将带领大家参观我们研发的关节模组";
        base_text = "关节模组";
        std::cout<<"关节模组"<<std::endl;
    }
    //智能代步车
    else if((dataString.find("智能") != dataString.npos))
    {
        step = 5;
        text = "接下来，您看到的是中原动力研发团队贴合康养市场需求，研发的一款智能代步车。";
        base_text = "智能代步车";
        std::cout<<"智能代步车"<<std::endl;
    }
    //视觉工作站
    else if((dataString.find("视觉工作站") != dataString.npos))
    {
        step = 6;
        text = "接下来，我将带领大家参观我们的视觉工作站。";
        base_text = "视觉工作站";
        std::cout<<"视觉工作站"<<std::endl;
    }
    //视觉检测工作站
    else if((dataString.find("检测") != dataString.npos))
    {
        step = 7;
        text = "接下来，我将带领大家参观我们的视觉检测工作站。";
        base_text = "视觉检测工作站";
        std::cout<<"视觉检测工作站"<<std::endl;
    }
    //城市管养机器人
    else if((dataString.find("城市管养") != dataString.npos))
    {
        step = 8;
        text = "接下来，您看到的是已经上过多次新闻的城市管养系列机器人。";
        base_text = "城市管养机器人";
        std::cout<<"城市管养机器人"<<std::endl;
    }
    //移动底盘
    else if((dataString.find("移动底盘") != dataString.npos))
    {
        step = 9;
        text = "接下来，您看到是中原动力自主研发制造的多功能移动底盘。";
        base_text = "移动底盘";
        std::cout<<"移动底盘"<<std::endl;
    }
    //智慧城市管理系统
    else if((dataString.find("智慧城市") != dataString.npos))
    {
        step = 10;
        text = "下面您看到的是中原动力“原筑云”智慧城市管理系统";
        base_text = "智慧城市管理系统";
        std::cout<<"智慧城市管理系统"<<std::endl;
    }
    //谢谢---充电桩
    else if((dataString.find("谢谢你的介绍") != dataString.npos))
    {   
        step = END;
        text = "感谢各位贵宾的聆听，本次参观到此结束，我们下次见！";
        base_text = "充电桩"; 
        std::cout<<"谢谢"<<std::endl;
    }
    else if((dataString.find("开启录音") != dataString.npos))
    {   
        text = "开启录音";
        std::cout<<"开启录音"<<std::endl;
    }
    else {
        // ROS_WARN("unrecognized command");
        // std::cout<< "unrecognized command" << std::endl;
        std::cout<< msg->data.c_str() << std::endl;
        // text = msg->data.c_str();
        face_text = " ";
        base_text = " ";
        body_text = " ";
    }

    
	string_msg.data = text;
	tts_text_pub->publish(string_msg);

    string_msg_face.data = face_text;
    face_pub->publish(string_msg_face);

    string_msg_base.data = base_text;
    base_pub->publish(string_msg_base);

    string_msg_body.data = body_text;
    body_pub->publish(string_msg_body);
}

void iattextAnswerCallback(const std_msgs::msg::String::SharedPtr msg)
{
    std::cout<< "收到回答" << msg->data.c_str() << std::endl;
}

void updateState(chat_msgs::msg::State::SharedPtr msg)
{
    rc_num = msg->step;
    if(rc_num != rc_numl)
    {
        // 按键C或D按下
        if(rc_num == START||rc_num == END)
            step =rc_num;
        // 下一个场景或上一个场景
        else 
            step = step+(rc_num-rc_numl);

        const char* text;
        const char* face_text;
        const char* base_text;
        const char* body_text;
        text = " ";
        face_text = " ";
        base_text = " ";
        body_text = " ";
        //遥控器控制修改部分
        if(step == 1)
        {
            text = "前往初始站点";
            base_text = "初始站点";
            std::cout<<"到初始站点"<<std::endl;
        }
        // 展厅，讲解+动作
        else if(step == 2)
        {
            text = "尊敬的各位领导各位来宾:大家好，我是展厅讲解员尤拉。被你发现了，我和人类不一样，我是一个机器人。今天将由我带您参观我的家，也就是中原动力的展厅。";
            base_text = "展厅";
            std::cout<<"到展厅"<<std::endl;
        }
        //双足机器人
        else if(step == 3)
        { 
            text = "接下来，我将带您徜徉在机器人的海洋中，请跟紧我！现在进入您视野的是双足行走家族。";
            base_text = "双足机器人";
            std::cout<<"到双足机器人"<<std::endl;
        }
        //关节模组
        else if(step == 4)
        {
            text = "接下来，我将带领大家参观我们研发的关节模组";
            base_text = "关节模组";
            std::cout<<"到关节模组"<<std::endl;
        }
        //智能代步车
        else if(step == 5)
        {
            text = "接下来，您看到的是中原动力研发团队贴合康养市场需求，研发的一款智能代步车。";
            base_text = "智能代步车";
            std::cout<<"智能代步车"<<std::endl;
        }
        //视觉工作站
        else if(step == 6)
        {
            text = "接下来，我将带领大家参观我们的视觉工作站。";
            base_text = "视觉工作站";
            std::cout<<"视觉工作站"<<std::endl;
        }
        //视觉检测工作站
        else if(step == 7)
        {
            text = "接下来，我将带领大家参观我们的视觉检测工作站。";
            base_text = "视觉检测工作站";
            std::cout<<"视觉检测工作站"<<std::endl;
        }
        //城市管养机器人
        else if(step == 8)
        {
            text = "接下来，您看到的是已经上过多次新闻的城市管养系列机器人。";
            base_text = "城市管养机器人";
            std::cout<<"城市管养机器人"<<std::endl;
        }
        //移动底盘
        else if(step == 9)
        {
            text = "接下来，您看到是中原动力自主研发制造的多功能移动底盘。";
            base_text = "移动底盘";
            std::cout<<"移动底盘"<<std::endl;
        }
        //智慧城市管理系统
        else if(step == 10)
        {
            text = "下面您看到的是中原动力“原筑云”智慧城市管理系统";
            base_text = "智慧城市管理系统";
            std::cout<<"智慧城市管理系统"<<std::endl;
        }
        //谢谢---充电桩
        else if(step == 11)
        {     
            text = "我的讲解完毕，谢谢大家，欢迎下次再来";
            base_text = "充电桩"; 
            std::cout<<"谢谢"<<std::endl;
        }
        // else {
        //     text = " ";
        //     face_text = " ";
        //     base_text = " ";
        //     body_text = " ";
        // }
        string_msg.data = text;
        tts_text_pub->publish(string_msg);

        string_msg_face.data = face_text;
        face_pub->publish(string_msg_face);

        string_msg_base.data = base_text;
        base_pub->publish(string_msg_base);

        string_msg_body.data = body_text;
        body_pub->publish(string_msg_body);
    }
    rc_numl = rc_num;
    // printf("step:%2d\n",step);
}

int main(int argc, char* argv[])
{
    float control_speed = 0;
	float control_turn = 0;	
    rclcpp::init(argc,argv);
    auto node = rclcpp::Node::make_shared("voice_controller");
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr iat_text_sub;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr iat_question_text_sub;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr iat_answer_text_sub;
    rclcpp::Subscription<chat_msgs::msg::State>::SharedPtr ulaa_state_sub;
    //iat_text_sub = node->create_subscription<std_msgs::msg::String>("iat_text", 1000,iattextCallback); //subscribe voice to text reault
    iat_question_text_sub = node->create_subscription<std_msgs::msg::String>("iat_question_text", 1000, iattextQuestionCallback); //subscribe question reault
    iat_answer_text_sub   = node->create_subscription<std_msgs::msg::String>("iat_answer_text", 1000, iattextAnswerCallback); //subscribe answer reault
    ulaa_state_sub = node->create_subscription<chat_msgs::msg::State>("state",10,updateState);//subscribe ulaa state
	tts_text_pub = node->create_publisher<std_msgs::msg::String>("tts_text", 1000);  //publish text to voice string
    face_pub     = node->create_publisher<std_msgs::msg::String>("action", 10); 
    base_pub     = node->create_publisher<std_msgs::msg::String>("pub_msgs", 10); 
    body_pub     = node->create_publisher<std_msgs::msg::String>("voiceCmd", 10); 
	cmd_vel_pub  = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 1);  
    RCLCPP_INFO(node->get_logger(), "Wait Command...");
    rclcpp::Rate loop_rate(50);
    while(rclcpp::ok()){
        switch (cmd_type)
        {
        case 1:
            control_speed = 0.1; 
            control_turn = 0.0;
            break;
        case 2:
            control_speed = -0.1; 
            control_turn = 0.0;
            break;
        case 3:
            control_speed = 0.1; 
            control_turn = 0.5;
            break;   
        case 4:
            control_speed = 0.1; 
            control_turn = -0.5;
            break;  
        case 5:
            control_speed = 0.0; 
            control_turn = 0.0;
            break; 
        default:
            break;
        }
        twist.linear.x = control_speed; 
        twist.linear.y = 0; 
        twist.linear.z = 0;
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = control_turn;
        cmd_vel_pub->publish(twist);
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }
    
	return 0;
}
