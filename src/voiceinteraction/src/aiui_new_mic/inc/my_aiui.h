#ifndef _MY_AIUI_H_
#define _MY_AIUI_H_

#undef AIUI_LIB_COMPILING

#include "aiui/AIUI_V2.h"
#include <string.h>

class MyAIUI
{
private:
    void reset_wakeup();
    // void writeAudio();
    void writeAudioFromLocal();
    // void stopAudio();
    void writeText();
    // void testTTs();
    void testTTs2();
    // void destroy();

public:
    MyAIUI();
    ~MyAIUI();
    // AIUI 代理 交互
	void createAgent();
    void agentStart();
    void agentStop();
    void wakeup();
    void readCmd();
};
#endif /* _MY_AIUI_H_ */
