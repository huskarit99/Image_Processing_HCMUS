#include "Convolution.h"

vector<float> Convolution::GetKernel() {
	return this->_kernel;
}

void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight) {
	this->_kernel = kernel;
	this->_kernelHeight = kHeight;
	this->_kernelWidth = kWidth;
}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage) {
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;
	int channels = sourceImage.channels();

	Mat sourceClone = sourceImage.clone(); // Get a clone of sourceImage
	vector <int> dx;
	vector <int> dy;
	for (int i = 0; i < this->_kernelHeight; i++)
		for (int j = 0; j < this->_kernelWidth; j++) {
			dx.push_back(i - (this->_kernelHeight / 2));
			dy.push_back(j - (this->_kernelWidth / 2));
		}

	destinationImage = Mat(rows - this->_kernelHeight + 1, cols - this->_kernelWidth + 1, CV_32FC1, Scalar(0.0));

	switch (channels) {
	case 1:
		for (int i = 0; i < destinationImage.rows; i++) {
			//uchar* dataRow = destinationImage.ptr<uchar>(i);
			float* dataRow = destinationImage.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				int i_source = i + (this->_kernelHeight / 2) , j_source = j + (this->_kernelWidth / 2);
				float convolution = 0.0;
				for (int k = 0; k < this->_kernelHeight * this->_kernelWidth; k++) {
					float dataImage = 1.0 * sourceClone.ptr<uchar>(i_source - dx[k])[j_source - dy[k]];
					float dataKernel = 1.0 * this->_kernel[(dx[k] + (this->_kernelHeight / 2)) * this->_kernelHeight + dy[k] + (this->_kernelWidth / 2)];
					convolution += dataImage * dataKernel;
				}
				dataRow[j] = convolution;
			}
		}
		break;
	case 3:
		return 1;
		break;
	}
	return 0;
}

Convolution::Convolution() {
	this->_kernel = vector<float>(0, 0);
	this->_kernelHeight = 0;
	this->_kernelWidth = 0;
}

Convolution::~Convolution() {
}
