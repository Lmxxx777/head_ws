#ifndef __AIUI_INTERACTION_HPP__
#define __AIUI_INTERACTION_HPP__
#include "aiui/AIUI_V2.h"
#include "json/json.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <AudioRecorder.h>
#include <AudioPlayer.h>
#include <FileUtil.h>

using namespace std;

class aiui_interaction
{
private:
    /* data */
    string readFileAsString(const string& path);
        
public:
    aiui_interaction(/* args */);
    ~aiui_interaction();
    //创建AIUI 代理，通过AIUI代理与SDK发送消息通信
	void createAgent();
    void setParams();
    void agent_start();
    void agent_stop();
    void app_destory();
    void recorder_creat();
    void recorder_start();
    void readCmd();
    void stop_recorder();
};

#endif
