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
    if(depth < 3)
    {
        depth = 3;
    }

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
        cv::imshow("Show",bright_i);
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

std::vector<cv::Mat> sliceImage(const cv::Mat& Image, int num=10, int axis=0) //0 for vertical 1 for horizontal
{
        //axis 1 not implemented
    std::vector<cv::Mat> slicedImage;
    int imgSize;
    int imgOtherSize;
    int sliceSize;
    int sliceNum;
    int lastSliceSize;
    if(axis == 0)
    {
        imgSize = Image.cols;
        imgOtherSize = Image.rows;
    }

    sliceSize = imgSize/num;
    if(imgSize != sliceSize*num)
    {
        sliceNum = num+1;
        lastSliceSize  = imgSize%num;
    }
    else
    {
        sliceNum = num;
        lastSliceSize = 0;
    }

    std::cout<<"slice : "<<sliceNum<<" times..."<<std::endl;
    for(int k = 0; k<sliceNum; k++)
    {

        std::cout<<"slicing : "<<k+1<<" times..."<<std::endl;
        if(k == sliceNum-1 && lastSliceSize != 0)
        {
            cv::Mat slice = cv::Mat(Image.rows,lastSliceSize,CV_32F);
            slice = Image.colRange(k*sliceSize,k*sliceSize+lastSliceSize-1);
            // for(int i = 0; i< Image.rows; i++)
            // {
            //     for(int j = 0; j<lastSliceSize; j++)
            //     {
            //         slice[i][j] = Image[i][k*sliceSize+j];
            //     }
            // }
            slicedImage.push_back(slice);
        }
        else
        {
            cv::Mat slice = cv::Mat(Image.rows,sliceSize,CV_32F);
            slice = Image.colRange(k*sliceSize,k*sliceSize+sliceSize-1);
            // for(int i = 0; i< Image.rows; i++)
            // {
            //     for(int j = 0; j<sliceSize; j++)
            //     {
            //         slice[i][j] = Image[i][k*sliceSize+j];
            //     }
            // }
            slicedImage.push_back(slice);
        }
        std::cout<<"slicing : "<<k+1<<" times... done"<<std::endl;
    }
    return slicedImage;
}

cv::Mat appendSlicedImages(const std::vector<cv::Mat>& slicedImage, int axis=0)
{
    //axis 1 not implemented
    int fullImageSize = 0;
    int sliceNum = slicedImage.size();
    cv::Mat fullImage = slicedImage[0];
    std::cout<<"slice : "<<sliceNum<<" times..."<<std::endl;
    for (int k = 1; k<slicedImage.size(); k++)
    {
        std::cout<<"appending : "<<k+1<<" times... done"<<std::endl;
        cv::hconcat(fullImage,slicedImage[k],fullImage);
        // for(int i = 0; i<fullImage.rows; i++)
        // {
        //     for(int j = 0; j<slicedImage[k].cols;j++)
        //     {
        //         if(k = slicedImage.size()-1)
        //         {
        //             fullImage[i][fullImage.cols -slicedImage[k].cols +j] = slicedImage[i][j];
        //         }
        //         else
        //         {
        //             fullImage[i][k*sliceSize + j] = slicedImage[i][j];
        //         }
                
        //     }
        // }
        std::cout<<"appending : "<<k+1<<" times... done"<<std::endl;
    }
    return fullImage;
}

int main()
{
	cv::Mat imageApple = cv::imread("./apple.jpg");
    cv::Mat imageOrange = cv::imread("./orange.jpg");
    cv::Mat imageMask = cv::imread("./mask.png");
    imageApple.convertTo(imageApple,CV_32F,1/255.f);
    imageOrange.convertTo(imageOrange,CV_32F,1/255.f);
    imageMask.convertTo(imageMask,CV_32F,1/255.f);

    std::cout<<"Image Size : "<<imageApple.size()<<std::endl;
    std::cout<<"Image Size : "<<imageOrange.size()<<std::endl;
    std::cout<<"Image Size : "<<imageMask.size()<<std::endl;

    auto pyrApple = LaplacianPyramid(imageApple);
    auto pyrOrange = LaplacianPyramid(imageOrange);
    auto pyrMask = GaussianPyramid(imageMask);

    // showPyramid(pyrMask);
    // imshow("mask",reconstruct(pyrMask));
    // waitKey();

    auto pyrResult = blendImages(pyrApple,pyrOrange,pyrMask);
    auto imgResult = reconstruct(pyrResult);
    imshow("reconstructed",imgResult);
    // waitKey();
    auto slicedImage = sliceImage(imgResult,5);
    // showPyramid(slicedImage);

    std::vector<std::vector<cv::Mat>> pyrSlicedImage;
    std::cout<<"downscale : "<<slicedImage.size()<<" times"<<std::endl;
    int mid = slicedImage.size()/2;
    for(int i = 0; i< slicedImage.size(); i++)
    {
        std::cout<<"downscaling : "<<i+1<<" times..."<<std::endl;
        if(i<mid)
        {
            pyrSlicedImage.push_back(GaussianPyramid(slicedImage[i],i));
        }
        else
        {
            pyrSlicedImage.push_back( GaussianPyramid( slicedImage[i], (slicedImage.size()-i) ) );
        }
        std::cout<<"downscaling : "<<i+1<<" times... done"<<std::endl;
    }
    std::vector<cv::Mat> reconstructedSlicedImage;
    std::cout<<"reconstruct : "<<pyrSlicedImage.size()<<" times"<<std::endl;
    for(int i = 0; i< pyrSlicedImage.size(); i++)
    {
        std::cout<<"reconstructing : "<<i+1<<" times..."<<std::endl;
        reconstructedSlicedImage.push_back( reconstruct(pyrSlicedImage[i]) );
        std::cout<<"reconstructing : "<<i+1<<" times... done"<<std::endl;
    }

    auto appendedImage = appendSlicedImages(reconstructedSlicedImage);
    imshow("appended",appendedImage/3);
    waitKey();
    for(int i = 0; i< 10; i++)
    {
        appendedImage = reconstruct( GaussianPyramid(appendedImage) );
    }
    imshow("appended",appendedImage/3);
    waitKey();
}
