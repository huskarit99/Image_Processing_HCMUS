#include "EdgeDetector.h"
#include "Convolution.h"

int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;
	int channels = sourceImage.channels();
	
	if (channels != 1) return 1;
	float eps = 1e-6;

	switch (method) {
	case 1: {
		float threshold = 25;

		Mat sourceClone = sourceImage.clone(); // Get a clone of sourceImage
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));
		cout << "Sobel Method \n";
		vector <float> Wx = { -1 , 0, 1, -2, 0, 2, -1, 0, 1 };
		vector <float> Wy = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
		for (int i = 0; i < Wx.size(); i++) {
			Wx[i] *= 1.0 / 4;
			Wy[i] *= 1.0 / 4;
		}
		Mat Gx;
		Mat Gy;
		Convolution ConvoX, ConvoY;
		ConvoX.SetKernel(Wx, 3, 3);
		ConvoY.SetKernel(Wy, 3, 3);
		ConvoX.DoConvolution(sourceClone, Gx);
		ConvoY.DoConvolution(sourceClone, Gy);
		//imshow("Show Image", Gx);
		//imshow("Show Image", Gy);
		for (int i = 0; i < destinationImage.rows; i++)
			for (int j = 0; j < destinationImage.cols; j++) {
				float fx = Gx.ptr<float>(i)[j];
				float fy = Gy.ptr<float>(i)[j];
				float e = sqrt(fx * fx + fy * fy);
				if (e - threshold >= eps) {
					destinationImage.ptr<float>(i)[j] = 1;
				}
			}
	}
		break;
	case 2: {
		cout << "Prewitt Method \n";
		float threshold = 50;

		Convolution ConvOx, ConvOy;
		Mat fx;
		Mat fy;

		vector <float> Wx = { 1.0, 0, -1.0, 1.0, 0, -1.0, 1.0, 0, -1.0 };
		vector <float> Wy = { -1.0, -1.0, -1.0, 0, 0, 0, 1.0, 1.0, 1.0 };
		for (int i = 0; i < Wx.size(); i++) {
			Wx[i] *= 1.0 / 3;
			Wy[i] *= 1.0 / 3;
		}
		ConvOx.SetKernel(Wx, kWidth, kHeight);
		ConvOx.DoConvolution(sourceImage, fx);

		ConvOy.SetKernel(Wy, kWidth, kHeight);
		ConvOy.DoConvolution(sourceImage, fy);

		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));
		for (int i = 0; i < destinationImage.rows; i++) {
			float* DestRow = destinationImage.ptr<float>(i);
			float* fxRow = fx.ptr<float>(i);
			float* fyRow = fy.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				float e = sqrt(fxRow[j] * fxRow[j] + fyRow[j] * fyRow[j]);
				if (e - threshold >= eps) {
					DestRow[j] = 1;
				}
			}
		}
		fx.release();
		fy.release();
	}
		break;
	case 3: {
		try {
			// Tạo mask
			vector<float> laplace = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
			Mat destinationImageCopied = Mat(rows, cols, CV_32FC1);

			// Tính tích chập
			Convolution Laplace;
			Laplace.SetKernel(laplace, 3, 3);
			// Nếu không tính được tích chập thì fail
			if (Laplace.DoConvolution(sourceImage, destinationImageCopied) == 1) return 1;

			// Tính threshold
			float threshold = -1.0 * INT_MAX;
			destinationImage = Mat::zeros(rows, cols, CV_8UC1);
			for (int x = 0; x < destinationImageCopied.rows; x++) {
				for (int y = 0; y < destinationImageCopied.cols; y++) {
					float value = destinationImageCopied.at<float>(x, y);
					threshold = value > threshold ? value : threshold;
				}
			}
			// Threshold sẽ bằng 1/4 giá trị max
			threshold = threshold > 255 ? 255 : threshold;
			threshold = threshold * 25 / 100.0;

			/* Tính zero crossing
			   Xét 4 trường hợp đối nhau: trên-dưới, trái-phải và 2 đường chéo
			   Nếu dấu khác nhau và độ chênh lệch giữa 2 giá trị > threshold thì chọn
			   Nếu có >= 2 chọn thì đó là điểm zero crossing
			*/
			int dx[] = { -1, 1, 0, -1 };
			int dy[] = { -1, -1, 1, 0 };
			for (int x = 1; x < destinationImageCopied.rows - 1; x++) {
				for (int y = 1; y < destinationImageCopied.cols - 1; y++) {
					int count = 0;
					for (int k = 0; k < 4; k++) {
						float value1 = destinationImageCopied.at<float>(x + dx[k], y + dy[k]);
						float value2 = destinationImageCopied.at<float>(x - dx[k], y - dy[k]);
						int sign1 = value1 < 0 ? -1 : 1;
						int sign2 = value2 < 0 ? -1 : 1;
						if (sign1 != sign2 && abs(value1 - value2) - eps > threshold) {
							count++;
						}
					}
					// Đánh dấu là điểm zero crossing
					if (count >= 2) destinationImage.at<uchar>(x, y) = 255;
				}
			}
			//imshow("Destination Image with Laplace", destinationImage);
			//waitKey(0);

		}
		catch (exception & e) {
			return 1;
		}
		return 0;
	}
		break;
	}
	

	return 0;
}

EdgeDetector::EdgeDetector() {
}

EdgeDetector::~EdgeDetector() {
}
