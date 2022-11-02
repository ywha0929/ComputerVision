#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <glm/glm.hpp>
using namespace cv;
using namespace glm;
std::vector<cv::Mat> GaussianPyramid(cv::Mat img, int depth=5)
{
    std::vector<cv::Mat> pyramid;
    cv::Mat src = img;
    

    for(int i = 0; i<depth; i++)
    {
        pyramid.push_back(src);
        cv::pyrDown(src,src);
    }
    return pyramid;
    
}

std::vector<cv::Mat> LaplacianPyramid(cv::Mat img, int depth=5)
{
    std::vector<cv::Mat> pyramid;
    cv::Mat src = img;
    

    for(int i = 0; i<depth-1; i++)
    {
        cv::Mat small, temp;
        cv::pyrDown(src,small);
        cv::pyrUp(small,temp, src.size());
        pyramid.push_back(src-temp);
        src = small;
    }
    pyramid.push_back(src);
    return pyramid;
    
}
cv::Size getSize(cv::Mat input)
{
    auto cols = input.cols;
    auto rows = input.rows;
    if(cols%2 != 0)
    {
        cols +=1;
    }
    if(rows%2 != 0)
    {
        rows +=1;
    }
    return Size(cols,rows);
}

void showPyramid( const std::vector<cv::Mat>& pyramid , int contrast=10, int brightness=0.5)
{
    for (auto i: pyramid)
    {
        cv::Mat bright_i = i*contrast + brightness;
        cv::imshow("Image",bright_i);
        cv::waitKey();
    }
}

cv::Mat reconstruct( const std::vector<cv::Mat>& pyramid ) //& similar to pointer
{
    cv::Mat ret = pyramid.back();
    for(int i = pyramid.size()-2 ; i>=0; i--)
    {
        // auto size = getSize(pyramid[i]);
        
        // auto size = Size(ret.cols *2,ret.rows*2);
        //cv::resize(ret,ret,size);
        // cv::Mat temp;
        // std::cout<<"ret size : "<<ret.cols<<", "<<ret.rows<<std::endl;
        // std::cout<<"pyramid size : "<<pyramid[i].cols<<", "<<pyramid[i].rows<<std::endl;
        // std::cout<<"size : "<<size.width<<", "<<size.height<<std::endl;
        // fprintf(stderr,"size : %d, %d",ret.cols,ret.rows);
        // fprintf(stderr,"pyramid size : %d, %d",pyramid[i].cols,pyramid[i].rows);
        cv::pyrUp(ret,ret);
        // std::cout<<"pyrUp"<<std::endl;
        // std::cout<<"ret size : "<<ret.cols<<", "<<ret.rows<<std::endl;
        // std::cout<<"pyramid size : "<<pyramid[i].size()<<std::endl;
        //temp = ret.clone();
        cv::resize(ret,ret,pyramid[i].size(),0,0,cv::INTER_LINEAR);
        // temp.release();
        std::cout<<"resize done"<<std::endl;
        //ret = ret;
        //cv::resize(pyramid[i],temp,size);
        ret += pyramid[i];
        ret  = ret /2;
    }
    return ret;
}

cv::Mat performGaussianFilter(cv::Mat image, int depth =5)
{
    std::vector<cv::Mat> pyr = GaussianPyramid(image,depth);
    return reconstruct(pyr)-0.5;
}

int main(int argc, char*argv[])
{
	std::vector<cv::Mat> coins;
    for(int i = 0; i< 6;i++)
    {
        char filename[20];
        sprintf(filename,"coins%d.jpg",i);
        cv::Mat image = imread(filename);
        image.convertTo(image,CV_8UC1);
        coins.push_back(image);
    }
    for(int i = 0; i<6; i++)
    {
        cv::Mat thisImage = coins[i];
        std::vector<cv::Vec3f> circles;
        cv::Mat GrayImage;
        
        cvtColor(thisImage,GrayImage,COLOR_BGR2GRAY);
        // Blur Image
        // imshow("before Gaussian",GrayImage);
        // waitKey();
        // GrayImage = performGaussianFilter(GrayImage,4);
        medianBlur(GrayImage, GrayImage, 5);
        //imshow("after Gaussian",GrayImage);
        // waitKey();

        
        // std::cout<<"Check : before HoughCircles"<<std::endl;
        // std::cout<<"Check : "<<GrayImage.type()<<" , "<<CV_8UC1<<std::endl;
        cv::HoughCircles(GrayImage,circles,HOUGH_GRADIENT,
            1, //dp
            25, //minDist
            115, //higher threshold
            52, //accumulator?
            30, //minRadius
            90 //maxRadius
        );
        // std::cout<<"Check : After HoughCircles"<<std::endl;
        std::cout<<"Number of Circles for coin"<<i<<" is : "<<circles.size()<<std::endl;
        if(argc != 1)
        {
            for( size_t i = 0; i < circles.size(); i++ )
            {
                Vec3i c = circles[i];
                Point center = Point(c[0], c[1]);
                // circle center
                circle( thisImage, center, 1, Scalar(0,100,100), 3, LINE_AA);
                // circle outline
                int radius = c[2];
                circle( thisImage, center, radius, Scalar(255,0,255), 3, LINE_AA);
            }
            imshow("detected circles", thisImage);
            waitKey();
        }
        
    }

}
