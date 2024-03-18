#ifndef __MYLISTENER_HPP__
#define __MYLISTENER_HPP__

#include "aiui/AIUI_V2.h"
#include "FileUtil.h"
using namespace aiui_v2;

class MyListener : public IAIUIListener
{
private:
	FileUtil::DataFileHelper* mTtsFileHelper;

public:
	void onEvent(const IAIUIEvent& event);

	MyListener();

	~MyListener();
};

#endif