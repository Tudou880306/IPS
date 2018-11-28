#pragma once
#include "stdafx.h"
#include "ShipDetectorDll.h"

class Player
{
public:
	void init(char* weightname);
	virtual void start() {};
	virtual void stop() {};
	virtual void pause() {};

public:
	void* handle;
	std::vector<bbox_t> targetinfo;
	void(*outresult) (cv::Mat temp, std::vector<bbox_t> targetinfo);
public:
	cv::Mat cur_frame;
	cv::Mat out_frame;
	int temp = 0;
	int playernum = -1;
};

class imagePlayer :public Player
{
public:
	void start();
	void stop() {};

};

class videoPlayer :public Player
{
public:
	videoPlayer() { isplay = false; };
public:
	void init();
	void start();
	void stop() ;
	void pause() ;
	
private:
	//void* output();
public:
	cv::VideoCapture cap;
	 
	bool isplay;
	bool flag;
public:
	CString file;
};

__interface Ifactory
{
	Player* CreatePlayer();
};

class imagePlayerFactory:public Ifactory
{
public:
	Player* CreatePlayer() 
	{
		return new imagePlayer();
	};
};

class videoPlayerFactory :public Ifactory
{
public:
	Player* CreatePlayer()
	{
		return new videoPlayer();
	}
};
