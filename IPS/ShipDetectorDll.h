#pragma once
#include "yolo_console_dll.h"


#ifndef SHIPDETECTOR_H_
#define SHIPDETECTOR_H_


#ifdef SHIPDETECTOR_EXPORTS
#define SHIPDETECTOR_API extern "C" _declspec(dllexport) 
#else
#define SHIPDETECTOR_API extern "C" _declspec(dllimport) 
#endif



/****************************************!
2 *@brief
3 *@author mayn
4 *@date   2018Äê5ÔÂ26ÈÕ
5 *@param[out]
6 *@param[in]
7 *@return
8 ****************************************/
SHIPDETECTOR_API void* SDLib_Init(char* weightname);
SHIPDETECTOR_API int SDLib_StartProcess(void *fhandl, std::string path, std::vector<bbox_t>&targetinfo);
SHIPDETECTOR_API int SDLib_StartProcess_Mat(void *fhandl, cv::Mat mat, std::vector<bbox_t>&targetinfo);
SHIPDETECTOR_API int SDLib_Release(void *fhandl);
#endif