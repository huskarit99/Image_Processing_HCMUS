
#include"ColorTransformer.h"

float COMP_CORREL(Mat Hist1, Mat Hist2) {
	if (Hist1.rows != Hist2.rows)
		return 0;
	float Mean1 = 0, Mean2 = 0, Denominator1 = 0, Denominator2 = 0, Numerator = 0;
	for (int i = 0; i < Hist1.rows * Hist1.cols; i++)
	{
		Mean1 += Hist1.data[i];
		Mean2 += Hist2.data[i];
	}
	Mean1 /= Hist1.rows * Hist1.cols;
	Mean2 /= Hist2.rows * Hist2.cols;
	for (int i = 0; i < Hist1.rows * Hist1.cols; i++)
	{
		Numerator += (Hist1.data[i] - Mean1) * (Hist2.data[i] - Mean2);
		Denominator1 += pow((Hist1.data[i] - Mean1), 2);
		Denominator2 += pow((Hist2.data[i] - Mean2), 2);
	}
	return Numerator / sqrt(Denominator1 * Denominator2);
}

float COMP_CHISQR(Mat H1, Mat H2)
{
	if (H1.rows != H2.rows)
		return 0;
	float result = 0;
	for (int i = 0; i < H1.rows * H1.cols; i++)
	{
		if (H1.data[i] != 0)
			result += pow((H1.data[i] - H2.data[i]), 2) / H1.data[i];
	}
	return result;
}

float COMP_INTERSECT(Mat H1, Mat H2)
{
	if (H1.rows != H2.rows)
		return 0;
	float result = 0, Sum1 = 0, Sum2 = 0;
	for (int i = 0; i < H1.rows * H1.cols; i++)
	{
		Sum1 += H1.data[i];
		Sum2 += H2.data[i];
		result += MIN(H1.data[i], H2.data[i]);
	}
	return result / MAX(Sum1, Sum2);
}

float COMP_BHATTACHARYYA(Mat H1, Mat H2)
{
	if (H1.rows != H2.rows)
		return 0;
	float Mean1 = 0, Mean2 = 0, result = 0;
	int N = H1.rows * H1.cols;
	for (int i = 0; i < N; i++)
	{
		Mean1 += H1.data[i];
		Mean2 += H2.data[i];
	}

	Mean1 /= N;
	Mean2 /= N;
	float cons = 1 / sqrt(Mean1 * Mean2 * N * N);
	for (int i = 0; i < N; i++)
	{
		result += (cons * sqrt(H1.data[i] * H2.data[i]));
	}

	return sqrt(1 - result);
}
int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	try {
		/* Tính các giá trị cơ bản*/
		int height = sourceImage.rows;
		int width = sourceImage.cols;
		int widthSteps = sourceImage.step[0];
		int nChannels = sourceImage.step[1];

		if (nChannels == 1)
			destinationImage = Mat(height, width, CV_8UC1);
		else
			destinationImage = Mat(height, width, CV_8UC3);

		/*Truy xuất đến các phần tử bằng con trỏ*/
		uchar* pSourceImage = (uchar*)sourceImage.data;
		uchar* pDestinationImage = (uchar*)destinationImage.data;
		for (int x = 0; x < height; x++, pSourceImage += widthSteps, pDestinationImage += widthSteps) {
			uchar* pRowSourceImage = pSourceImage;
			uchar* pRowDestinationImage = pDestinationImage;
			for (int y = 0; y < width; y++, pRowSourceImage += nChannels, pRowDestinationImage += nChannels) {
				/*Truy xuất đến từng kênh, và cộng giá trị thay đổi độ sáng*/
				for (int channel = 0; channel < nChannels; channel++) {
					int value = pRowSourceImage[channel] + b;
					/*Đảm bảo giá trị phải nằm từ 0 đến 255
					Nếu < 0 thì sẽ bằng 0
					Nếu > 255 thì bằng 255*/
					pRowDestinationImage[channel] = uchar(value > UCHAR_MAX ? UCHAR_MAX : value > 0 ? value : 0);
				}
			}
		}
		/*imshow("Destination Image", destinationImage);*/
		//waitKey(0);
	}
	catch (exception & e)
	{
		return 1;
	}
	return 0;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
{
	try {
		/* Tính các giá trị cơ bản*/
		int height = sourceImage.rows;
		int width = sourceImage.cols;
		int widthSteps = sourceImage.step[0];
		int nChannels = sourceImage.step[1];

		if (nChannels == 1)
			destinationImage = Mat(height, width, CV_8UC1);
		else 
			destinationImage = Mat(height, width, CV_8UC3);

		/*Truy xuất đến các phần tử bằng con trỏ*/
		uchar* pSourceImage = (uchar*)sourceImage.data;
		uchar* pDestinationImage = (uchar*)destinationImage.data;
		for (int x = 0; x < height; x++, pSourceImage += widthSteps, pDestinationImage += widthSteps) {
			uchar* pRowSourceImage = pSourceImage;
			uchar* pRowDestinationImage = pDestinationImage;
			for (int y = 0; y < width; y++, pRowSourceImage += nChannels, pRowDestinationImage += nChannels) {
				/*Truy xuất đến từng kênh, thay đổi độ tương phản bằng cách nhân với một giá c*/
				for (int channel = 0; channel < nChannels; channel++) {
					/*Do c là số thực (float) nên ta phải chuyển về kiểu số nguyên bằng hàm round*/
					int value = round(pRowSourceImage[channel] * c);
					/*Đảm bảo giá trị phải nằm từ 0 đến 255
					Nếu < 0 thì sẽ bằng 0
					Nếu > 255 thì bằng 255*/
					pRowDestinationImage[channel] = uchar(value > UCHAR_MAX ? UCHAR_MAX : value > 0 ? value : 0);
				}
			}
		}
		//imshow("Destination Image", destinationImage);
		//waitKey(0);
	}
	catch (exception & e)
	{
		return 1;
	}
	return 0;
}


int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	try {
		// Tính các thông số cơ bản
		int L = 256;
		int height = sourceImage.rows;
		int width = sourceImage.cols;
		// Kích thước của ảnh
		double NM = 1.0 * height * width;
		int widthSteps = sourceImage.step[0];
		// Số lượng kênh của ảnh
		int nChannels = sourceImage.step[1];
		if (nChannels == 1)
			destinationImage = Mat(height, width, CV_8UC1, Scalar(0));
		else
			destinationImage = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
		// Tính histogram của ảnh
		Mat H = Mat();
		CalcHistogram(sourceImage, H);

		/* Cách 1: Tính riêng lẻ từng kênh
		// Duyệt từng kênh của ảnh, cân bằng histogram trên từng kênh của ảnh
		for (int channel = 0; channel < nChannels; channel++) {

			int E[256] = { 0 };

			//Tổng tích lũy
			// Cumulative Distribution Function
			double CDF[256] = { 0 };
			CDF[0] = 1.0 * H.at<int>(channel, 0);
			for (int level = 1; level < L; level++) {
				CDF[level] += 1.0 * (CDF[level - 1] + H.at<int>(channel, level));
			}

			// Chuẩn hóa về đoạn 0 - 255
			for (int level = 0; level < L; level++) {
				E[level] = round(((L - 1) / NM) * CDF[level]);
			}

			// Tạo ảnh kết quả bằng cách duyệt ảnh bằng con trỏ ( cái thiện tốc độ)
			uchar* pSourceImage = (uchar*)sourceImage.data;
			uchar* pDestinationImage = (uchar*)destinationImage.data;
			for (int x = 0; x < height; x++, pSourceImage += widthSteps, pDestinationImage += widthSteps) {
				uchar* pRowSourceImage = pSourceImage;
				uchar* pRowDestinationImage = pDestinationImage;
				for (int y = 0; y < width; y++, pRowSourceImage += nChannels, pRowDestinationImage += nChannels) {
					// Truy xuất đến từng kênh
					pRowDestinationImage[channel] = E[pRowSourceImage[channel]];
				}
			}
		}
		*/
		/* Cách 2: Tính trung bình tất cả các kênh*/
		//Tổng tích lũy
		// Cumulative Distribution Function
		int E[256] = { 0 };
		double CDF[256] = { 0 };
		double count = 0;
		for (int channel = 0; channel < nChannels; channel++)
			count += H.at<int>(channel, 0);

		CDF[0] = (1.0 * count) / nChannels;

		for (int level = 1; level < L; level++) {
			count = 0;
			for (int channel = 0; channel < nChannels; channel++) count += H.at<int>(channel, level);
			CDF[level] += 1.0 * (CDF[level - 1] + count / nChannels);
		}

		// Chuẩn hóa về đoạn 0 - 255
		for (int level = 0; level < L; level++) {
			E[level] = round(((L - 1) / NM) * CDF[level]);
		}
		// Tạo ảnh kết quả bằng cách duyệt ảnh bằng con trỏ ( cái thiện tốc độ)
		uchar* pSourceImage = (uchar*)sourceImage.data;
		uchar* pDestinationImage = (uchar*)destinationImage.data;
		for (int x = 0; x < height; x++, pSourceImage += widthSteps, pDestinationImage += widthSteps) {
			uchar* pRowSourceImage = pSourceImage;
			uchar* pRowDestinationImage = pDestinationImage;
			for (int y = 0; y < width; y++, pRowSourceImage += nChannels, pRowDestinationImage += nChannels) {
				// Truy xuất đến từng kênh
				for (int channel = 0; channel < nChannels; channel++) {
					pRowDestinationImage[channel] = E[pRowSourceImage[channel]];
				}
			}
		}
		//imshow("Destination Image", destinationImage);
		//waitKey(0);
	}
	catch (exception & e)
	{
		return 1;
	}
	return 0;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	try {
		// Tính các thông số cơ bản
		int height = 512, width = 768;
		// Chiều rộng cột
		int binW = round((double)width / 256);

		histImage = Mat(height, width, CV_8UC3, Scalar(255, 255, 255));
		int nChannels = histMatrix.rows;

		// Tìm intensity cao nhất trong các kênh để normalize theo maxIntensity
		int maxIntensity = 0;
		for (int channel = 0; channel < nChannels; channel++) {
			for (int level = 0; level < 256; level++) {
				maxIntensity = histMatrix.at<int>(channel, level) > maxIntensity ? histMatrix.at<int>(channel, level) : maxIntensity;
			}
		}
		// Vẽ histogram
		for (int channel = nChannels - 1; channel >= 0; channel--) {
			int histogramNormalize[256] = { 0 };
			for (int level = 0; level < 256; level++) {
				// Normalize lại histogram
				histogramNormalize[level] = histMatrix.at<int>(channel, level);
				double tmp = double(histogramNormalize[level]);
				tmp /= maxIntensity;
				histogramNormalize[level] = round(tmp * height);
				// Vẽ histogram
				if (nChannels == 1) {
					rectangle(histImage, Point(level * binW, height),
						Point((level + 1) * binW, height - histogramNormalize[level]),
						Scalar::all(0), cv::FILLED, 8, 0);
				}
				else {
					if (channel == 0)
						rectangle(histImage, Point(binW * level, height),
							Point(binW * (level + 1), height - histogramNormalize[level]),
							Scalar(255, 0, 0), cv::FILLED, 8, 0);
					if (channel == 1)
						rectangle(histImage, Point(binW * level, height),
							Point(binW * (level + 1), height - histogramNormalize[level]),
							Scalar(0, 255, 0), cv::FILLED, 8, 0);
					if (channel == 2)
						rectangle(histImage, Point(binW * level, height),
							Point(binW * (level + 1), height - histogramNormalize[level]),
							Scalar(0, 0, 255), cv::FILLED, 8, 0);
				}
			}
		}
		//imshow("Histogram Image", histImage);
		//waitKey(0);
	}
	catch (exception & e)
	{
		return 1;
	}
	return 0;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histImage)
{
	// Lấy hàng và cột của ảnh
	int cols = sourceImage.cols;
	int rows = sourceImage.rows;
	// kích thước max của lược đồ màu 0 -> 255
	int histSize = 256;
	// Lấy số kênh màu để biết ảnh xám hay ảnh màu
	int channels = sourceImage.channels();
	// Sử dụng try - catch trong C++ để bắt lỗi trong quá trình tính lược đồ màu
	try
	{
		// Ảnh xám
		if (channels == 1)
		{
			// CV_<bit_depth>(S|U|F)C<number_of_channels>
			Mat gray_hist(channels, histSize, CV_32S, Scalar(0));
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < cols; j++)
				{
					uint8_t gray;
					gray = sourceImage.data[i * cols + j];
					// Đếm tần suất xuất hiện của giá trị điểm ảnh xuất hiện trong ảnh
					gray_hist.at<int>(gray) += 1;
				}
			}
			histImage = gray_hist;
		}
		else
			// Ảnh màu
			if (channels == 3)
			{
				// CV_<bit_depth>(S|U|F)C<number_of_channels>
				Mat bgr_hist(channels, histSize, CV_32S, Scalar(0, 0, 0));
				{
					for (int i = 0; i < rows; i++)
					{
						for (int j = 0; j < cols; j++)
						{
							uint8_t blue, green, red;
							blue = sourceImage.data[i * cols * 3 + j * 3 + 0]; // bule
							green = sourceImage.data[i * cols * 3 + j * 3 + 1]; // green
							red = sourceImage.data[i * cols * 3 + j * 3 + 2]; // red

							// Đếm tần suất xuất hiện của giá trị điểm ảnh xuất hiện trong ảnh
							bgr_hist.at<int>(0, blue) += 1;
							bgr_hist.at<int>(1, green) += 1;
							bgr_hist.at<int>(2, red) += 1;
						}
					}
				}
				histImage = bgr_hist;
			}
	}
	catch (exception & e)
	{
		return 1;
	}
	return 0;
}

float ColorTransformer::CompareImage(const Mat& image1, Mat& image2, int type) {
	Mat Hist1, Hist2;
	if ((CalcHistogram(image1, Hist1) == 0) && (CalcHistogram(image2, Hist2) == 0))
	{

		switch (type)
		{
		case 0:
			return COMP_CORREL(Hist1, Hist2);
			break;

		case 1:
			return COMP_CHISQR(Hist1, Hist2);
			break;
		case 2:
			return COMP_INTERSECT(Hist1, Hist2);
			break;

		case 3:
			return COMP_BHATTACHARYYA(Hist1, Hist2);
			break;
		default:
			return COMP_CORREL(Hist1, Hist2);
			break;
		}
	}
	else
		return 0;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}