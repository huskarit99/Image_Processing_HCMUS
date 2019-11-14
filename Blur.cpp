#pragma once
#include <math.h>
#define _USE_MATH_DEFINES
#include "Blur.h"
#include "Convolution.h"
#include <vector>
using namespace std;

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;
	int channels = sourceImage.step[1];
	int widthStep = sourceImage.step[0];

	if (channels != 1) return 1;

	switch (method)
	{
	case 0: {
		cout << "Mean Blurring method!";
		Convolution Conv;
		vector<float> kernel;

		float n = kWidth * kHeight;
		for (int i = 0; i < kWidth * kHeight; i++) {
			kernel.push_back(1.0 / n);
		}
		Conv.SetKernel(kernel, kWidth, kHeight);

		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));

		Conv.DoConvolution(sourceImage, destinationImage);

		for (int i = 0; i < destinationImage.rows; i++) {
			float* dataRow = destinationImage.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				dataRow[j] /= 255.0;
			}
		}
	}
		  break;
	case 1: {
		cout << "Median Blurring method!";
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));

		int kHalfWidth = kWidth >> 1;
		int kHalfHeight = kHeight >> 1;

		vector<int> offsets;
		for (int y = -kHalfHeight; y <= kHalfHeight; y++)
			for (int x = -kHalfWidth; x <= kHalfWidth; x++)
				offsets.push_back(y * widthStep + x);

		uchar* p = (uchar*)sourceImage.data;

		for (int i = 0; i < destinationImage.rows; i++) {
			float* dataRow = destinationImage.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				int i_source = i + (kWidth / 2), j_source = j + (kHeight / 2);
				uchar* pSource = p + i_source * widthStep + j_source * channels;
				//Lấy lân cận
				vector<uchar> value;
				for (int k = 0; k < offsets.size(); k++) {
					value.push_back(pSource[offsets[k]]);
				}
				//Sắp xếp và xét median
				sort(value.begin(), value.begin() + value.size());
				dataRow[j] = value[value.size() / 2 + 1] / 255.0;
			}
		}
	}
		  break;
	case 2: {
		Mat sourceClone = sourceImage.clone(); // Get a clone of sourceImage
		destinationImage = Mat(rows, cols, CV_32FC1, Scalar(0.0));
		vector <int> dx;
		vector <int> dy;
		//vector <float> h;
		for (int i = 0; i < kHeight; i++)
			for (int j = 0; j < kWidth; j++) {
				dx.push_back(i - (kHeight / 2));
				dy.push_back(j - (kWidth / 2));
			}
		cout << "Gaussian Blurring method!";
		float stddev;
		if ((kHeight == 3) && (kWidth == 3)) stddev = 3;
		else
			stddev = (1.0 * kHeight / 3) * (1.0 * kWidth / 3) * 3;
		vector <float> h (kHeight * kWidth);
		for (int k = 0; k < kHeight * kWidth; k++) { //calculate h(i,j) in gaussian distribution
			int i = dx[k], j = dy[k];
			float disValue = 1 / (sqrt(2 * M_PI) * stddev) * exp(-(i * i + j * j) / (2 * stddev * stddev));
			h[k] = disValue;
		}
		for (int i = 0; i < rows; i++) 
			for (int j = 0; j < cols; j++) {
				float sumDisValue = 0;
				for (int k = 0; k < kHeight * kWidth; k++) {
					int i_pos = i - dx[k];
					int j_pos = j - dy[k];
					float dataImage;
					if ((i_pos - (kHeight / 2) < 0) || (i_pos + (kHeight / 2) >= rows)
						|| (j_pos - (kWidth / 2) < 0) || (j_pos + (kWidth / 2) >= cols)) {
						dataImage = 1.0 * sourceClone.ptr<uchar>(i)[j];
					}
					else
						dataImage = 1.0 * sourceClone.ptr<uchar>(i_pos)[j_pos];
					sumDisValue += dataImage * h[k];
				}
				if (sumDisValue > 255.0) 
					destinationImage.ptr<float>(i)[j] = 255;
				else 
					destinationImage.ptr<float>(i)[j] = sumDisValue;
			}

		//MinMaxScaler
		float minValue = 255;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				if (destinationImage.ptr<float>(i)[j] < minValue) minValue = destinationImage.ptr<float>(i)[j];

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				destinationImage.ptr<float>(i)[j] = (destinationImage.ptr<float>(i)[j] - minValue) / (255 - minValue);
	}
		break;

	}

	return 0;
}

Blur::Blur()
{
}

Blur::~Blur()
{
}
