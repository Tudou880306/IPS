#pragma once
#include "stdafx.h"
#include <array>
#include <vector>
#include<cmath>
#include "stdio.h"
#include<iostream>
#include <map>
#ifdef _WIN32
#define OPENCV
#define GPU
#endif
#include "yolo_v2_class.hpp"	// imported functions from DLL

class ShipDetector
{
public:
	ShipDetector();
	~ShipDetector();

private:
	std::string filename;
	std::string  names_file;
	std::string  cfg_file;
	std::string  weights_file;
	Detector *detector;

public:
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年7月24日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int LoadXml(const char* xml);
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年7月24日
	5 *@param[out] 
	6 *@param[in]  null  
	7 *@return      
	8 ****************************************/
	int InitHandle();
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年7月24日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int DetectorImage(std::string path, std::vector<bbox_t>&targetinfo);
	int DetectorImage(cv::Mat mat_img, std::vector<bbox_t>&targetinfo);
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年7月24日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int ReleaseHandle();
};
