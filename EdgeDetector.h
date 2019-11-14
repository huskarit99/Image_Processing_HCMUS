#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
class EdgeDetector
{
public:
	/*
	Hàm phát hiện biên cạnh của ảnh xám
	sourceImage: ảnh input
	destinationImage: ảnh output
	method: phương pháp detect
	kWidth, kHeight: kích thước kernel
		1: Sobel
		2: Prewitt
		3: Laplace
	Hàm trả về
		0: nếu detect thành công
		1: nếu detect thất bại (không đọc được ảnh input,...)
	*/
	int DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method);
	EdgeDetector();
	~EdgeDetector();
};

