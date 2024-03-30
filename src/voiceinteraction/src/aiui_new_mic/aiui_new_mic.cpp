#include "rclcpp/rclcpp.hpp"
#include "my_aiui.h"

using namespace std;

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    MyAIUI aiui_agent;
    cout << ">>>>>创建AIUI代理Agent\n" << endl;
    aiui_agent.createAgent();
    cout << ">>>>>开启AIUI交互\n" << endl;
    aiui_agent.agentStart();
    cout << ">>>>>唤醒AIUI\n" << endl;
    aiui_agent.wakeup();
    cout << ">>>>>请输入指令，q退出\n" << endl;
    aiui_agent.readCmd();
    rclcpp::shutdown();
    return 0;
}