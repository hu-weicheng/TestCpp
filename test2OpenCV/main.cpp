#include <direct.h>
#include <stdio.h>
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

//debug下在lib文件的名称后加d，release下不加d。




int main()
{
#pragma execution_character_set("utf-8")
	//system("chcp 65001");
	//setlocale(LC_ALL, ""); // ✅ 辅助解决本地化字符问题

	// 打印当前工作目录
	//char buffer[256];
	//_getcwd(buffer, 256);
	//std::cout << "当前工作目录: " << buffer << std::endl;

	//std::cout << "当前工作目录: " << std::filesystem::current_path() << std::endl;

	string file = "C:/Users/Rise8/source/repos/RaiseMathLib/test2/test.jpg";
	cout << std::filesystem::exists(file) << endl;
	// 打印 OpenCV 构建信息
	//std::cout << cv::getBuildInformation() << std::endl;

	cv::String filename = "./test.jpg";
	//Mat img = imread(file);//
	//Mat img = imread("test.jpg");
	//
	Mat img = imread(filename);

	if (img.empty())
	{
		std::cout << "image read is null" << std::endl;
		return -10;
	}

	Mat small;
	resize(img, small, Size(640, 480));  // ✅ 缩放到指定尺寸

	namedWindow("test", WINDOW_AUTOSIZE);
	resizeWindow("test", 640, 480);      // ✅ 指定窗口大小（宽 × 高）
	//imshow("【原图】", img);
	imshow("test", small);

	waitKey(0);
	return 0;
}
