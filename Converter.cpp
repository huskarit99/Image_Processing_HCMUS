#include"Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {

	int cols = sourceImage.cols;
	int rows = sourceImage.rows;
	Mat image(rows, cols, CV_8UC1);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			uint8_t red, green, blue;
			// Truy cập vào giá trị từng kênh màu bằng con trỏ
			// Vị trí đầu tiên của 3 kênh màu sẽ là màu blue -> green -> red
			blue = sourceImage.data[i * cols * 3 + j * 3 + 0]; // B
			green = sourceImage.data[i * cols * 3 + j * 3 + 1]; // G
			red = sourceImage.data[i * cols * 3 + j * 3 + 2]; // R
// Có rất nhiều cách để chuyển ảnh màu sang ảnh xám, ta chọn 1 tỉ lệ để chuyển như sau:
			image.data[i * cols + j] = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
		}
	}
	destinationImage = image;
	return 1;
};

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage) {
	int cols = sourceImage.cols;
	int rows = sourceImage.rows;
	Mat image(rows, cols, CV_8UC3);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			uint8_t gray;
			gray = sourceImage.data[i * cols + j];
			// Khó có thể chuyển từ màu xám sang RGB chính xác, nên ở đây ta sẽ tạm chấp nhận việc  gia trị 3 kênh màu RGB sẽ bằng giá trị của kênh màu xám
			image.data[i * cols * 3 + j * 3 + 0] = gray; // B
			image.data[i * cols * 3 + j * 3 + 1] = gray; // G
			image.data[i * cols * 3 + j * 3 + 2] = gray; // R
		}
	}
	destinationImage = image;
	return 1;
};

int Converter::RGB2HSV(const Mat& image, Mat& destinationImage) {
	if (image.empty())
		return 0;
	int nl = image.rows; // number of lines
	int nc = image.cols;

	destinationImage = image.clone();
	for (int i = 0; i < nl; i++) {
		// lấy địa chỉ của dòng thứ i

		const uchar* data = image.ptr<uchar>(i);
		uchar* data2 = destinationImage.ptr<uchar>(i);


		for (int j = 0; j < nc; j++) {


			float r, g, b, H, S, V;
			r = data[j * 3 + 2] / (float)255;
			g = data[j * 3 + 1] / (float)255;
			b = data[j * 3 + 0] / (float)255;

			V = std::max({ r, g, b });
			if (V != 0)
			{
				S = (V - std::min({ r,g, b })) / V;
			}
			else
				S = 0;

			if (V == r)
				H = 60 * (g - b) / (V - std::min({ r,g, b }));
			else if (V == g)
				H = 120 + 60 * (b - r) / (V - std::min({ r,g, b }));
			else
				H = 240 + 60 * (r - g) / (V - std::min({ r,g, b }));
			if (H < 0)
				H += 360;
			data2[j * 3 + 0] = H / 2;
			data2[j * 3 + 1] = S * 255;
			data2[j * 3 + 2] = V * 255;

		}
	}
	return 1;
}

int Converter::HSV2RGB(const Mat& image, Mat& destinationImage) {
	if (image.empty())
		return 0;
	int nl = image.rows; // number of lines
	int nc = image.cols;

	destinationImage = image.clone();
	for (int i = 0; i < nl; i++) {
		// lấy địa chỉ của dòng thứ i

		const uchar* data = image.ptr<uchar>(i);
		uchar* data2 = destinationImage.ptr<uchar>(i);


		for (int j = 0; j < nc; j++) {


			float R, G, B, H, S, V, C, X, m;
			H = data[j * 3 + 0] * 2;
			S = data[j * 3 + 1] / (float)255;
			V = data[j * 3 + 2] / (float)255;
			C = V * S;
			X = C * (1 - abs(((int)H - 60) % 2 - 1));
			m = V - C;
			if (H < 60)
			{
				R = C; G = X; B = 0;
			}
			else if (H < 120)
			{
				R = X; G = C; B = 0;
			}
			else if (H < 180)
			{
				R = 0; G = C; B = X;
			}
			else if (H < 240)
			{
				R = 0; G = X; B = C;
			}
			else if (H < 300)
			{
				R = C; G = 0; B = C;
			}
			else
			{
				R = C; G = 0; B = X;
			}
			data2[j * 3 + 2] = (R + m) * 255;
			data2[j * 3 + 1] = (G + m) * 255;
			data2[j * 3 + 0] = (B + m) * 255;



		}
	}
	return 1;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	switch (type)
	{
	case 0:
		return RGB2GrayScale(sourceImage, destinationImage);
	case 1:
		return GrayScale2RGB(sourceImage, destinationImage);
	case 2:
		return RGB2HSV(sourceImage, destinationImage);
	case 3:
		return HSV2RGB(sourceImage, destinationImage);
	default:
		return 0;
		break;
	}
};

Converter::Converter() {};
Converter::~Converter() {};