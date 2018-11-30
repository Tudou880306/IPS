#pragma once
#include <opencv2/opencv.hpp>			// C++
#include <opencv2/core/version.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <mutex>
class YoloMark
{
public:
	YoloMark() ;
	~YoloMark();

	static YoloMark* GetInstance();
	void start(std::string imgfile, std::string trainname, std::string objnames);
private:
	static YoloMark *instance;
public:
	bool quit;

};



