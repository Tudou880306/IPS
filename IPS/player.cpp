#include "stdafx.h"
#include "player.h"
#include <thread>
#include <mutex>
std::mutex m_lock;
void Player::init(char* weightname)
{
	 handle = SDLib_Init(weightname);
}

void imagePlayer::start()
{
	SDLib_StartProcess_Mat(handle, cur_frame, targetinfo);
	outresult(cur_frame, targetinfo);
	//out_frame = cur_frame.clone();
}

//void imagePlayer::exit()
//{
//	SDLib_Release(handle);
//}
void output( videoPlayer* player)
{
	while (1)
	{
		std::thread t_detect, t_cap;
		cv::Mat cap_frame, m_CvvImages_temp;
		while (!player->cur_frame.empty())
		{
			if (!t_cap.joinable())
			{
				t_cap = std::thread([&]()
				{
					while (1)
					{
						OutputDebugString(_T("===============t_cap\r\n"));
						if (player->isplay)
						{
							if (player->cap.isOpened())
							{
								m_lock.lock();
								player->cap >> cap_frame;
								m_lock.unlock();
							}
							else
								cap_frame = player->cur_frame.clone();
							if (player->flag)
							{
								//mt1.lock();
								//cur_frame1.copyTo(m_CvvImages1_temp); //¸´ÖÆ¸ÃÖ¡Í¼Ïñ 
								m_CvvImages_temp = cap_frame.clone();
								player->flag = false;
								//mt1.unlock();
							}
							Sleep(25);
						}
						else
							Sleep(1000);
					}
				});
			}
			if (!t_detect.joinable())
			{
				t_detect = std::thread([&]()
				{
					player->flag = true;

					while (1)
					{
						while (!player->cur_frame.empty() && player->isplay)
						{
							//flag = true;
							OutputDebugString(_T("===============t_detect\r\n"));

							if (!player->flag)
							{
								SDLib_StartProcess_Mat(player->handle, m_CvvImages_temp, player->targetinfo);
								//imshow("", m_CvvImages_temp);
								//cv::waitKey(30);
								player->out_frame = m_CvvImages_temp;
								player->outresult(m_CvvImages_temp, player->targetinfo);
								//player->temp = 3;
								//result_queue.push(targetinfo);
								//mt_temp.lock();
								////m_CvvImages1_temp.copyTo(m_CvvImages1);
								//m_CvvImages = m_CvvImages_temp.clone();

								//mt_temp.unlock();
								player->flag = true;

							}
							cv::waitKey(3);
							Sleep(5);
						}
						Sleep(500);
					}
				});
			}
			Sleep(10);
		}
	}
}
void videoPlayer::init()
{
	std::thread t1(output, this);
	t1.detach();
}
void videoPlayer::start()
{
	
	USES_CONVERSION;
	m_lock.lock();
	cap.open(T2A(file.GetBuffer(0)));
	cap >> cur_frame;
	m_lock.unlock();
	isplay = true;
}

void videoPlayer::pause()
{
	isplay = !isplay;
}

void videoPlayer::stop()
{
	isplay = false;
}



