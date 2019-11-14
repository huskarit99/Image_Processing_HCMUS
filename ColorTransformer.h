#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>
using namespace cv;
using std::exception;
class ColorTransformer
{
public:
	/*
	Hàm nhận vào một ảnh, thay đổi độ sáng của ảnh này và lưu kết quả vào ảnh mới
	Tham so:
		sourceImage	: ảnh ban đầu
		destinationImage: ảnh kết quả
		b	: giá trị số nguyên dùng để thay đổi độ sáng của ảnh
	Hàm trả về:
		0: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		1: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b);

	/*
	Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
		c	: giá trị số thực dùng để thay đổi độ tương phản của ảnh
	Hàm trả về:
		0: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		1: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c);


	/*
	Hàm tính lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		sourceImage : ảnh ban đầu
		histMatrix : ma trận histogram (nChannels x 256), mỗi dòng là 1 mảng 256 phần tử histogram của từng kênh màu
	Hàm trả về:
		0: Nếu thành công thì trả về matrix kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		1: Nếu không tính được histogram hoặc ảnh input không tồn tại
	*/
	int CalcHistogram(const Mat& sourceImage, Mat& histMatrix);

	/*
	Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		image : ảnh dùng để tính histogram
		histImage : ảnh histogram
	Hàm trả về:
		0: Nếu thành công vẽ được histogram
		1: Nếu không vẽ được histogram
	*/
	int HistogramEqualization(const Mat& sourceImage, Mat& destinationImage);



	/*
Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
Tham so :
	histMatrix : ma trận histogram đã tính được
	histImage : ảnh histogram được vẽ
Hàm trả về:
	0: Nếu thành công vẽ được histogram
	1: Nếu không vẽ được histogram
*/
	int DrawHistogram(const Mat& histMatrix, Mat& histImage);


	/*
	Hàm so sánh hai ảnh
	Tham so :
		image1 : ảnh thứ nhất
		image2 : ảnh thứ hai
	Hàm trả về:
		độ đo sự tương đồng giữa hai ảnh
	*/
	float CompareImage(const Mat& image1, Mat& image2, int type);


	ColorTransformer();
	~ColorTransformer();
};

