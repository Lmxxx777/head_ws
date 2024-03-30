#include <MyListener.hpp>

MyListener::MyListener()
{
	mTtsFileHelper = new FileUtil::DataFileHelper("");
}

MyListener::~MyListener()
{
	if (mTtsFileHelper != NULL)
	{
		delete mTtsFileHelper;
		mTtsFileHelper = NULL;
	}
}