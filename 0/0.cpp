#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
using namespace cv;
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
cv::Mat reconstruct( const std::vector<cv::Mat>& pyramid) //& similar to pointer
{
    cv::Mat ret = pyramid.back();
    for(int i = pyramid.size()-2 ; i>=0; i--)
    {
        // auto size = getSize(pyramid[i]);
        
        // auto size = Size(ret.cols *2,ret.rows*2);
        //cv::resize(ret,ret,size);
        cv::Mat temp;
        std::cout<<"ret size : "<<ret.cols<<", "<<ret.rows<<std::endl;
        std::cout<<"pyramid size : "<<pyramid[i].cols<<", "<<pyramid[i].rows<<std::endl;
        // std::cout<<"size : "<<size.width<<", "<<size.height<<std::endl;
        // fprintf(stderr,"size : %d, %d",ret.cols,ret.rows);
        // fprintf(stderr,"pyramid size : %d, %d",pyramid[i].cols,pyramid[i].rows);
        cv::pyrUp(ret,ret);
        std::cout<<"pyrUp"<<std::endl;
        std::cout<<"ret size : "<<ret.cols<<", "<<ret.rows<<std::endl;
        std::cout<<"pyramid size : "<<pyramid[i].size()<<std::endl;
        //temp = ret.clone();
        cv::resize(ret,ret,pyramid[i].size(),0,0,cv::INTER_LINEAR);
        // temp.release();
        std::cout<<"resize done"<<std::endl;
        //ret = ret;
        //cv::resize(pyramid[i],temp,size);
        ret += pyramid[i];
    }
    return ret;
}

int main()
{
	cv::Mat image = cv::imread("./archon.jpg");
    image.convertTo(image,CV_32F,1/255.f);

    auto pyr = GaussianPyramid(image,100);
    // for (auto i: pyr)
    // {
    //     cv::Mat bright_i = i;
    //     cv::imshow("Image",bright_i);
    //     cv::waitKey();
    // }
    auto res = reconstruct(pyr);
    res = res * .01f-0.5;

    imshow("reconstructed",res);
    waitKey();
    imshow("image",image);
    waitKey();

    
    std::cout<<"OpenCV version : "<<CV_VERSION<<std::endl;
}
