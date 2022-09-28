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

std::vector<cv::Mat> blendImages(const std::vector<cv::Mat>& pyrImage1, const std::vector<cv::Mat>& pyrImage2, const std::vector<cv::Mat>& pyrMask) //blend two image pyramid and create result pyramid
{
    std::vector<cv::Mat> pyrResult;
    if( !(pyrImage1.size() == pyrImage2.size() && pyrImage2.size() == pyrMask.size()) )
    {
        std::cout<<"Sizes(depths) of three pyramids are not identical : "<<pyrImage1.size()<<", "<<pyrImage2.size()<<", "<<pyrMask.size()<<std::endl;
        exit(-1);
    }
    for(int i = 0; i< pyrImage1.size(); i++)
    {
        cv::Mat layerResult;
        cv::Mat blendedImage1;
        cv::Mat blendedImage2;

        cv::multiply(pyrMask[i],pyrImage1[i],blendedImage1);
        cv::multiply(Scalar(1,1,1)-pyrMask[i],pyrImage2[i],blendedImage2);

        layerResult = blendedImage1 + blendedImage2;
        pyrResult.push_back(layerResult);
    }
    return pyrResult;
}

int main()
{
	cv::Mat imageApple = cv::imread("./apple.jpg");
    cv::Mat imageOrange = cv::imread("./orange.jpg");
    cv::Mat imageMask = cv::imread("./mask.png");
    imageApple.convertTo(imageApple,CV_32F,1/255.f);
    imageOrange.convertTo(imageOrange,CV_32F,1/255.f);
    imageMask.convertTo(imageMask,CV_32F,1/255.f);
    int depth = 40;
    auto pyrApple = LaplacianPyramid(imageApple,depth);
    auto pyrOrange = LaplacianPyramid(imageOrange,depth);
    auto pyrMask = GaussianPyramid(imageMask,depth);

    // showPyramid(pyrMask);
    // imshow("mask",reconstruct(pyrMask));
    // waitKey();

    auto pyrResult = blendImages(pyrApple,pyrOrange,pyrMask);
    auto imgResult = reconstruct(pyrResult);
    imshow("reconstructed",imgResult);
    waitKey();
    // auto pyrBlended = GaussianPyramid(imgResult);
    // auto imgBlended = reconstruct(pyrBlended);
    // imgBlended = imgBlended/10;
    // imshow("reconstructed",imgBlended);
    // waitKey();
}
