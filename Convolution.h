#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
class Convolution
{
	vector<float> _kernel;// lưu giá trị của ma trận kernel dùng để tích chập
	int _kernelWidth;//chiều rộng kernel
	int _kernelHeight;//chiều dài kernel
public:
	//trả về kernel 
	vector<float> GetKernel();
	//set kernel, chiều rộng, chiều dài kernel phải là số lẻ
	void SetKernel(vector<float> kernel, int kWidth, int kHeight);

	/*
	Hàm tính convolution của 1 ảnh xám với kernel được xác định trước
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	0: nếu tính thành công
	1: nếu tính thất bại (không đọc được ảnh input,...)
	*/
	int DoConvolution(const Mat& sourceImage, Mat& destinationImage);
	Convolution();
	~Convolution();
};

