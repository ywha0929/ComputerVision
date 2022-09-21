#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
int main()
{
	cv::Mat image = cv::imread("burt_image.png");

    cv::imshow("Image",image);
    cv::waitKey();
}
