// OpenCV3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


#include "ColorTransformer.h"
#include "Converter.h"
#include "GeometricTransformer.h"
#include "Blur.h"
#include "EdgeDetector.h"

char command[255];
char inputPath[255];

int main(int argc, char** argv)
{
	if (argc < 3) {
		cout << "Passing wrong arguments";
		return 0;
	}

	char* command = argv[1];
	char* inputPath = argv[2];
	Mat SourceImage = imread(inputPath, IMREAD_COLOR);
	if (SourceImage.empty()) {
		cout << "Could not find the image";
		return 1;
	}
	Mat DestinationImage;
	/*============================================================================
	Lab 1
	============================================================================*/
	Converter ImageConverter;
	ColorTransformer ImageColorTransformer;
	imshow("Source Image", SourceImage);
	if (strcmp(command, "--rgb2gray") == 0) {
		if ((ImageConverter.Convert(SourceImage, DestinationImage, 0) == 0)) {
			imshow("RGB to Gray", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--gray2rgb") == 0) {
		if ((ImageConverter.Convert(SourceImage, DestinationImage, 1) == 0)) {
			imshow("Gray to RGB", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--rgb2hsv") == 0) {
		if ((ImageConverter.Convert(SourceImage, DestinationImage, 2) == 0)) {
			imshow("RGB to HSV", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--hsv2rgb") == 0) {
		if ((ImageConverter.Convert(SourceImage, DestinationImage, 3) == 0)) {
			imshow("HSV to RGB", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--bright") == 0) {
		if (argc < 4) {
			cout << "Too few arguments for change brightness!";
			return 1;
		}
		int b = atoi(argv[3]);
		if ((ImageColorTransformer.ChangeBrighness(SourceImage, DestinationImage, b) == 0)) {
			imshow("Change brightness", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--contrast") == 0) {
		if (argc < 4) {
			cout << "Too few arguments for change contrast!";
			return 1;
		}
		float c = atof(argv[3]);
		if ((ImageColorTransformer.ChangeContrast(SourceImage, DestinationImage, c) == 0)) {
			imshow("Change contrast", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--hist") == 0) {
		Mat histMatrix;
		if ((ImageColorTransformer.CalcHistogram(SourceImage, histMatrix) == 0)) {
			cout << "Histogram of Source Image:";
			cout << histMatrix;
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--equalhist") == 0) {
		Mat histMatrix;
		if ((ImageColorTransformer.HistogramEqualization(SourceImage, DestinationImage) == 0)) {
			imshow("Histogram Equalization", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--drawhist") == 0) {
		Mat histMatrix;
		if (ImageColorTransformer.CalcHistogram(SourceImage, histMatrix) == 1)
			cout << "There are erros when calculating histogram";

		if ((ImageColorTransformer.HistogramEqualization(SourceImage, DestinationImage) == 0)) {
			Mat histImage;
			ImageColorTransformer.DrawHistogram(histMatrix, histImage);
			imshow("Histogram Image of Source Image", histImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--compare") == 0) {
		Mat SecondImage = imread(argv[3]);
		if (SecondImage.empty()) {
			cout << "Could not find the image";
			return 1;
		}
		imshow("Second Image", SecondImage);
		cout << "Correlation between 2 images: " << ImageColorTransformer.CompareImage(SourceImage, SecondImage, 0);
		waitKey(0);
		return 0;
	}
	/*============================================================================
	Lab 2
	============================================================================*/
	GeometricTransformer GeoTransformer;
	PixelInterpolate *interpolate = new NearestNeighborInterpolate();
	if (strcmp(command, "--zoom") == 0) {
		if (argc < 4) {
			cout << "Too few arguments to zoom image!";
			return 1;
		}
		float s = atof(argv[3]);
		if (GeoTransformer.Scale(SourceImage, DestinationImage, s, s, interpolate) == 0) {
			imshow("Zoom Image", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	if (strcmp(command, "--resize") == 0) {
		if (argc < 5) {
			cout << "Too few arguments to resize!";
			return 1;
		}
		int newX = atoi(argv[3]);
		int newY = atoi(argv[4]);
		if (GeoTransformer.Resize(SourceImage, DestinationImage, newX, newY, interpolate) == 0) {
			imshow("Resize Image", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--rotate") == 0) {
		if (argc < 4) {
			cout << "Too few arguments to rotate!";
			return 1;
		}
		float angle = atof(argv[3]);
		if (GeoTransformer.RotateKeepImage(SourceImage, DestinationImage, angle, interpolate) == 0) {
			imshow("Rotate Keep Image", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--rotateN") == 0) {
		if (argc < 4) {
			cout << "Too few arguments to rotate!";
			return 1;
		}
		float angle = atof(argv[3]);
		if (GeoTransformer.RotateUnkeepImage(SourceImage, DestinationImage, angle, interpolate) == 0) {
			imshow("Rotate Unkeep Image", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	if (strcmp(command, "--flip") == 0) {
		if (argc < 4) {
			cout << "Too few arguments to flip!";
			return 1;
		}
		bool direction = strcmp(argv[3], "Ox") == 0 ? 1 : 0;
		if (GeoTransformer.Flip(SourceImage, DestinationImage, direction) == 0) {
			imshow("Flip Image", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	/*============================================================================
	Lab 3
	============================================================================*/
	Blur BlurMethod;
	Mat graySourceImage;
	cvtColor(SourceImage, graySourceImage, CV_BGR2GRAY);
	if (strcmp(command, "--mean") == 0) {
		if (argc < 5) {
			cout << "Too few arguments to blur image!";
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, DestinationImage, kWidth, kHeight, 0) == 0) {
			imshow("Mean Blurring Method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	if (strcmp(command, "--median") == 0) {
		if (argc < 5) {
			cout << "Too few arguments to blur image!";
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, DestinationImage, kWidth, kHeight, 1) == 0) {
			imshow("Median Blurring Method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	if (strcmp(command, "--gauss") == 0) {
		if (argc < 5) {
			cout << "Too few arguments to blur image!";
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, DestinationImage, kWidth, kHeight, 2) == 0) {
			imshow("Gaussian Blurring Method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	EdgeDetector ED;
	if (strcmp(command, "--sobel") == 0) {
		if (ED.DetectEdge(graySourceImage, DestinationImage, 3, 3, 1) == 0) {
			imshow("Edge Detection with Sobel method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	if (strcmp(command, "--prewitt") == 0) {
		if (ED.DetectEdge(graySourceImage, DestinationImage, 3, 3, 2) == 0) {
			imshow("Edge Detection with Prewitt method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}
	if (strcmp(command, "--laplace") == 0) {
		if (ED.DetectEdge(graySourceImage, DestinationImage, 3, 3, 3) == 0) {
			imshow("Edge Detection with Laplace method", DestinationImage);
		}
		else cout << "Errors occur!";
		waitKey(0);
		return 0;
	}

	cout << "Not found any commands!";

	waitKey(0);
	return 0;
}
