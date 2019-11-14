#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
class Blur
{
public:
	/*
	Hàm làm trơn ảnh xám
	sourceImage: ảnh input
	destinationImage: ảnh output
	method: phương pháp detect
	kWidth, kHeight: kích thước kernel
	0: Trung bình
	1: Trung vị
	2: Gaussian
	Hàm trả về
	0: nếu detect thành công
	1: nếu detect thất bại (không đọc được ảnh input,...)
	*/
	int BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method);
	Blur();
	~Blur();
};

