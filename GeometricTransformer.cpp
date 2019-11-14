#include "GeometricTransformer.h"

void AffineTransform::Translate(float dx, float dy) {
	float value[3][3] = { {1, 0, dx}
						,{0, 1, dy}
						, {0, 0, 1} };
	Mat matTranslate = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matTranslate * this->_matrixTransform;
}

void AffineTransform::Rotate(float angle) {
	float cosAngle = cos(angle * _PI / 180);
	float sinAngle = sin(angle * _PI / 180);
	float value[3][3] = { {cosAngle, -sinAngle, 0}
						,{sinAngle, cosAngle, 0}
						, {0, 0, 1} };
	Mat matRotate = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matRotate * this->_matrixTransform;
}

void AffineTransform::Scale(float sx, float sy) {
	float value[3][3] = { {sx, 0, 0}
						,{0, sy, 0}
						, {0, 0, 1} };
	Mat matScale = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matScale * this->_matrixTransform;
}

void AffineTransform::TransformPoint(float& x, float& y) {
	float v[] = { x, y, 1 };
	Mat matResult = this->_matrixTransform * (Mat(3, 1, CV_32FC1, v));
	x = matResult.ptr<float>(0)[0];
	y = matResult.ptr<float>(0)[1];
}

Mat AffineTransform::getmatrixTransform()
{
	return this->_matrixTransform;
}

void AffineTransform::setmatrixTransform(Mat matrixTransform) {
	this->_matrixTransform = matrixTransform;
}

AffineTransform::AffineTransform() {
	float value[3][3] = { {1, 0, 0}
						,{0, 1, 0}
						, {0, 0, 1} };
	this->_matrixTransform = Mat(3, 3, CV_32FC1, value).clone();
	/*cout << this->_matrixTransform;*/
}

AffineTransform::~AffineTransform() {
	this->_matrixTransform.release();
}

PixelInterpolate::PixelInterpolate() {}
PixelInterpolate::~PixelInterpolate() {}

NearestNeighborInterpolate::NearestNeighborInterpolate() {}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	int l = (int)round(tx);
	int k = (int)round(ty);
	uchar* p = pSrc + srcWidthStep * l + nChannels * k;
	uchar res = p[0];

	return res;
}

BilinearInterpolate::BilinearInterpolate() {}
BilinearInterpolate::~BilinearInterpolate() {}

uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	int l = (int)floor(tx);
	int k = (int)floor(ty);
	float a = tx - l;
	float b = ty - k;

	uchar* p1 = pSrc + srcWidthStep * l + nChannels * k;
	uchar* p2 = pSrc + srcWidthStep * (l + 1) + nChannels * k;
	uchar* p3 = pSrc + srcWidthStep * l + nChannels * (k + 1);
	uchar* p4 = pSrc + srcWidthStep * (l + 1) + nChannels * (k + 1);

	int flk = p1[0];
	int fl1k = p2[0];
	int flk1 = p3[0];
	int fl1k1 = p4[0];

	float sum = (1 - a) * (1 - b) * flk + a * (1 - b) * fl1k\
		+ (1 - a) * b * flk1 + a * b * fl1k1;
	uchar res = round(sum);
	return res;

}

GeometricTransformer::GeometricTransformer() {}
GeometricTransformer::~GeometricTransformer() {}

int GeometricTransformer::Transform(
	const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	if (beforeImage.empty())
	{
		// Trả với giá trị 0 nếu source image rỗng (không đọc được ảnh)
		return 0;
	}
	// Lấy kích thước ảnh gốc
	int cols = beforeImage.cols;
	int rows = beforeImage.rows;

	//Các thông số widthStep, nChannels để nội suy màu
	int widthStep = beforeImage.step[0];
	int nChannels = beforeImage.step[1];

	// Lấy ma trận affine cho trước và tạo ma trận nghịch đảo của nó
	Mat matrixTransform = transformer->getmatrixTransform();
	Mat Rinv = matrixTransform.inv();

	// Khởi tạo vector chứa [Px, Py, 1]
	float B[] =
	{
		0, 0, 1.0,
	};
	Mat Pos = Mat(3, 1, CV_32FC1, B);

	//Lấy con trỏ của ảnh gốc và các thông số widthStep, nChannels để nội suy màu
	uchar* pSrc = (uchar*)beforeImage.data;

	// Tạo ma trận kết quả dựa vào ảnh gốc và affine đã cho trước
	// Nếu affine cho trước là scale ta sẽ khởi tạo ảnh kết quả theo tỉ lệ cho trước
	if (matrixTransform.at<float>(0, 1) == 0 && matrixTransform.at<float>(1, 0) == 0)
		afterImage = Mat::zeros(beforeImage.rows * matrixTransform.at<float>(0, 0), beforeImage.cols * matrixTransform.at<float>(1, 1), CV_8UC3);
	else
	{
		afterImage = Mat::zeros(beforeImage.rows, beforeImage.cols, CV_8UC3);

		AffineTransform AT;
		//Đưa tâm ảnh về gốc tọa độ
		AT.Translate(-rows / 2, -cols / 2);
		// Xoay ảnh
		AT.setmatrixTransform(matrixTransform);
		// Đưa tâm ảnh về chính giữa
		AT.Translate(rows / 2, cols / 2);
		// Ta tạo lại ma trận nghịch đảo của nó
		Rinv = AT.getmatrixTransform().inv();
	}

	try
	{
		// Lấy kích thước của ma trận kết quả
		int new_rows = afterImage.rows;
		int new_cols = afterImage.cols;
		for (int i = 0; i < new_rows; i++)
		{
			uchar* pData = afterImage.ptr<uchar>(i);
			for (int j = 0; j < new_cols; j++)
			{
				//Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là gốc trên cùng bên trái
				Pos.at<float>(0, 0) = i * 1.0;
				Pos.at<float>(1, 0) = j * 1.0;
				//Tìm tọa độ thực trên ảnh gốc
				Mat tpos = Rinv * Pos;
				//tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
				float tx = tpos.at<float>(0, 0);
				float ty = tpos.at<float>(0, 1);
				//Chỉ xét tx, ty nằm trong ảnh gốc
				if (tx >= 0 and ty >= 0 and tx < rows and ty < cols)
				{
					for (int c = 0; c < nChannels; c++)
						pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);

				}
			}
		}
	}
	catch (Exception & e)
	{
		return 1;
	}

	//Giải phóng các ma trận trung g
	Rinv.release();
	Pos.release();
	// Trả về giá trị 0 nếu biến đổi thành công
	return 0;
}

int GeometricTransformer::RotateKeepImage(
	const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
	//Các thông số widthStep, nChannels để nội suy màu
	int rows = srcImage.rows;
	int cols = srcImage.cols;

	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];
	//Đổi góc từ độ sang radian
	//float theta = angle * _PI / 180;

	AffineTransform AT;
	//Đưa tâm ảnh về gốc tọa độ của ảnh gốc
	AT.Translate(-rows / 2, -cols / 2);
	//Xoay ảnh
	AT.Rotate(angle);

	float X[] = { 0, 0, rows, rows };
	float Y[] = { 0, cols, 0, cols };
	//Tiến hành xoay các biến đổi hình học các đỉnh của ảnh
	for (int i = 0; i < 4; i++) {
		AT.TransformPoint(X[i], Y[i]);
	}
	//Lấy 4 điểm xa nhất theo 4 hướng
	float minx = std::min({ X[0], X[1], X[2], X[3] });
	float maxx = std::max({ X[0], X[1], X[2], X[3] });
	float miny = std::min({ Y[0], Y[1], Y[2], Y[3] });
	float maxy = std::max({ Y[0], Y[1], Y[2], Y[3] });
	//Kích thước ảnh mới
	int newRows = (int)ceil(maxx - minx);
	int newCols = (int)ceil(maxy - miny);
	//Đưa tâm ảnh về chính giữa ảnh mới
	AT.Translate(newRows / 2, newCols / 2);

	//Lấy nghịch đảo*/
	Mat Rinv = AT.getmatrixTransform().inv();

	//Khởi tạo vector chứa [Px, Py, 1]
	float B[] =
	{
		0, 0, 1.0,
	};
	Mat Pos = Mat(3, 1, CV_32FC1, B);
	/*
	//Khởi tạo tọa độ 4 góc ảnh
	float p[4][3];
	float p1[] = { -rows / 2.0, -cols / 2.0, 1.0 };
	float p2[] = { -rows / 2.0, cols - cols / 2.0, 1.0 };
	float p3[] = {  rows - rows / 2.0, -cols / 2.0, 1.0 };
	float p4[] = {  rows - rows / 2.0, cols - cols / 2.0, 1.0 };
	//Tạo các vector tương ứng với 4 góc ảnh
	Mat q1 = Mat(3, 1, CV_32FC1, p1);
	Mat q2 = Mat(3, 1, CV_32FC1, p2);
	Mat q3 = Mat(3, 1, CV_32FC1, p3);
	Mat q4 = Mat(3, 1, CV_32FC1, p4);
	//Tiến hành lấy tọa độ sau khi xoay ảnh của 4 góc ảnh
	q1 = R * q1;
	q2 = R * q2;
	q3 = R * q3;
	q4 = R * q4;

	vector<float> x;
	x.push_back(q1.ptr<float>(0)[0]);
	x.push_back(q2.ptr<float>(0)[0]);
	x.push_back(q3.ptr<float>(0)[0]);
	x.push_back(q4.ptr<float>(0)[0]);

	vector<float> y;
	y.push_back(q1.ptr<float>(1)[0]);
	y.push_back(q2.ptr<float>(1)[0]);
	y.push_back(q3.ptr<float>(1)[0]);
	y.push_back(q4.ptr<float>(1)[0]);
	//Lấy 4 điểm xa nhất theo 4 hướng
	float minx = std::min({ x[0], x[1], x[2], x[3] });
	float maxx = std::max({ x[0], x[1], x[2], x[3] });
	float miny = std::min({ y[0], y[1], y[2], y[3] });
	float maxy = std::max({ y[0], y[1], y[2], y[3] });
	//Giải phóng ma trận trung gian
	q1.release();
	q2.release();
	q3.release();
	q4.release();
	//Tính toán số dòng, cột của khung ảnh mới
	int newRows = (int)ceil(maxx - minx);
	int newCols = (int)ceil(maxy - miny);
	*/
	//Tạo khung cho ảnh đích với cùng kích thước với ảnh gốc
	if (nChannels == 1)
		dstImage = Mat(newRows, newCols, CV_8UC1, Scalar(0));
	else if (nChannels == 3)
		dstImage = Mat(newRows, newCols, CV_8UC3, Scalar(0));
	else {
		return 1;
	}
	//Lấy con trỏ của ảnh gốc và các thông số widthStep, nChannels để nội suy màu
	uchar* pSrc = (uchar*)srcImage.data;

	for (int i = 0; i < dstImage.rows; i++) {
		uchar* pData = dstImage.ptr<uchar>(i);
		for (int j = 0; j < dstImage.cols; j++) {
			//Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là tâm ảnh mới
			Pos.ptr<float>(0)[0] = i * 1.0;
			Pos.ptr<float>(1)[0] = j * 1.0;
			//Tìm tọa độ thực trên ảnh gốc
			Mat tpos = Rinv * Pos;
			//tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
			float tx = tpos.ptr<float>(0)[0];
			float ty = tpos.ptr<float>(1)[0];
			//Chỉ xét tx, ty nằm trong ảnh gốc
			if (tx >= 0 and ty >= 0 and tx < rows - 1 and ty < cols - 1) {
				for (int c = 0; c < nChannels; c++)
					pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);
			}
		}
	}
	//Giải phóng các ma trận trung gian
	Rinv.release();
	Pos.release();
	return 0;
}
int GeometricTransformer::RotateUnkeepImage(
	const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
	//Các thông số widthStep, nChannels để nội suy màu
	int rows = srcImage.rows;
	int cols = srcImage.cols;

	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];
	//Đổi góc từ độ sang radian
	//float theta = angle * _PI / 180;

	AffineTransform AT;
	//Đưa tâm ảnh về gốc tọa độ
	AT.Translate(-rows / 2, -cols / 2);
	//Xoay ảnh
	AT.Rotate(angle);
	//Đưa tâm ảnh về chính giữa
	AT.Translate(rows / 2, cols / 2);

	//Lấy nghịch đảo*/
	Mat Rinv = AT.getmatrixTransform().inv();


	//Khởi tạo vector chứa [Px, Py, 1]
	float B[] =
	{
		0, 0, 1.0,
	};
	Mat Pos = Mat(3, 1, CV_32FC1, B);


	//Tạo khung cho ảnh đích với cùng kích thước với ảnh gốc
	if (nChannels == 1) {
		dstImage = Mat(rows, cols, CV_8UC1, Scalar(0));
	}
	else if (nChannels == 3) {
		dstImage = Mat(rows, cols, CV_8UC3, Scalar(0));
	}
	else {
		return 1;
	}
	//Lấy con trỏ của ảnh gốc
	uchar* pSrc = (uchar*)srcImage.data;


	for (int i = 0; i < rows; i++) {
		uchar* pData = dstImage.ptr<uchar>(i);
		for (int j = 0; j < cols; j++) {
			//Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là tâm ảnh
			Pos.ptr<float>(0)[0] = i * 1.0;
			Pos.ptr<float>(1)[0] = j * 1.0;
			//Tìm tọa độ thực trên ảnh gốc
			Mat tpos = Rinv * Pos;
			//tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
			float tx = tpos.ptr<float>(0)[0];
			float ty = tpos.ptr<float>(1)[0];
			//Chỉ xét tx, ty nằm trong ảnh gốc
			if (tx >= 0 and ty >= 0 and tx < rows - 1 and ty < cols - 1) {
				for (int c = 0; c < nChannels; c++)
					pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);
			}

		}
	}
	//Giải phóng các ma trận trung g
	Rinv.release();
	Pos.release();
	return 0;
}

int GeometricTransformer::Scale(
	const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.empty())
	{
		// Trả với giá trị 0 nếu source image rỗng (không đọc được ảnh)
		return 0;
	}
	// Lấy kích thước ảnh gốc
	int cols = srcImage.cols;
	int rows = srcImage.rows;

	//Các thông số widthStep, nChannels để nội suy màu
	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];

	// Khởi tạo ma trận affine scale
	AffineTransform AT;
	AT.Translate(-rows / 2, -cols / 2);
	AT.Scale(sx, sy);
	AT.Translate(rows / 2, cols / 2);

	// Tạo ma trận nghịch đảo của affine scale
	Mat Rinv = AT.getmatrixTransform().inv();

	//Lấy con trỏ của ảnh gốc và các thông số widthStep, nChannels để nội suy màu
	uchar* pSrc = (uchar*)srcImage.data;

	// Khởi tạo ảnh kết quả với kích thước tỉ lệ theo sx, sy cho trước
	dstImage = Mat::zeros(rows, cols, CV_8UC3);


	// Khởi tạo vector chứa [Px, Py, 1]
	float B[] =
	{
		0, 0, 1.0,
	};
	Mat Pos = Mat(3, 1, CV_32FC1, B);

	// Dùng try catch để bắt lỗi trong quá trình transform
	try
	{
		for (int i = 0; i < rows; i++)
		{
			uchar* pData = dstImage.ptr<uchar>(i);
			for (int j = 0; j < cols; j++)
			{
				//Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là gốc trên cùng bên trái
				Pos.at<float>(0, 0) = i * 1.0;
				Pos.at<float>(1, 0) = j * 1.0;
				//Tìm tọa độ thực trên ảnh gốc
				Mat tpos = Rinv * Pos;
				//tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
				float tx = tpos.at<float>(0, 0);
				float ty = tpos.at<float>(1, 0);
				if (tx >= 0 and ty >= 0 and tx < rows and ty < cols)
				{
					for (int c = 0; c < nChannels; c++)
						pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);

				}
			}
		}
	}
	catch (Exception & e)
	{
		// Trả về giá trị 1 nếu có bất kì lỗi nào xảy ra trong quá trình scale
		return 1;
	}
	//Giải phóng các ma trận trung g
	Rinv.release();
	Pos.release();

	// Trả về giá trị 0 sau khi scale thành công
	return 0;
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	if (srcImage.empty())
		return 0;

	int rows = srcImage.rows;
	int cols = srcImage.cols;

	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];

	//Tạo khung cho ảnh đích với kích thước cho trước
	if (nChannels == 1) {
		dstImage = Mat(newHeight, newWidth, CV_8UC1, Scalar(0));
	}
	else if (nChannels == 3) {
		dstImage = Mat(newHeight, newWidth, CV_8UC3, Scalar(0));
	}
	else {
		return 1;
	}

	//Lấy con trỏ của ảnh gốc

	uchar* pSrc = (uchar*)srcImage.data;
	for (int i = 0; i < newHeight; i++) {
		uchar* pData = dstImage.ptr<uchar>(i);
		for (int j = 0; j < newWidth; j++) {

			float tx = (float)i * rows / newHeight;
			float ty = (float)j * cols / newWidth;

			//Chỉ xét tx, ty nằm trong ảnh gốc
			if (tx >= 0 and ty >= 0 and tx < rows and ty < cols)
			{
				for (int c = 0; c < nChannels; c++)
					pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);

			}
		}
	}
	return 0;
}

int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction)
{
	int nl = srcImage.rows;
	int nc = srcImage.cols;
	int chanels = srcImage.channels();
	int wstep = nc * chanels;
	// Tạo khung ảnh mới có cùng kích thước với ảnh gốc
	dstImage.release();
	dstImage = srcImage.clone();
	const size_t elemSize = srcImage.elemSize();

	if (direction == 0)
	{
		// nếu đối xứng theo trục đứng thì ta đối xứng giá trị các điểm ảnh trên mỗi dòng
		for (int i = 0; i < nl; i++)
		{
			char* pLine = dstImage.ptr<char>(i);
			for (int j = 0; j < nc / 2; j++)
			{
				char* temp,
					* p = pLine + (nc - j) * chanels;
				temp = new char[elemSize];

				memcpy(temp, p, elemSize);
				memcpy(p, pLine + j * chanels, elemSize);
				memcpy(pLine + j * chanels, temp, elemSize);
				delete[]temp;
			}
		}
	}
	else
	{
		// Đối xứng theo trục ngang thì hoán đổi giá trị điểm ảnh giữa các dòng với nhau
		for (int i = 0; i < nl / 2; i++)
		{

			char* pLine = dstImage.ptr<char>(i);
			char* temp,
				* p = dstImage.ptr<char>(nl - i - 1);

			temp = new char[elemSize * nc];
			memcpy(temp, p, elemSize * nc);
			memcpy(p, pLine, elemSize * nc);
			memcpy(pLine, temp, elemSize * nc);
			delete[]temp;
		}
	}
	return 0;
};