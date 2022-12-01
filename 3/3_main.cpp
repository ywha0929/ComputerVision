#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>
// #include <opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv4/opencv2/xfeatures2d/nonfree.hpp>

using namespace cv;
using namespace glm;
using namespace cv::xfeatures2d;

std::vector<DMatch> filterMatches(std::vector<DMatch> matches, double range = 0.5)
{
    std::vector<DMatch> goodMatch;
    double averageDistance;
    for(int i= 0; i< matches.size(); i++)
    {
        averageDistance += matches[i].distance;
    }
    averageDistance = averageDistance / matches.size();
    for(int i = 0; i< matches.size(); i++)
    {
        if(matches[i].distance < averageDistance + range && matches[i].distance > averageDistance - range)
        {
            goodMatch.push_back(matches[i]);
        }
    }
    
    return goodMatch;
}
int main(int argc, char*argv[])
{
    char nameImage1[] = "./left2.jpg";
    char nameImage2[] = "./right2.jpg";
    Mat image1 = imread(nameImage1);
    pyrDown(image1,image1);
    pyrDown(image1,image1);
    Mat image2 = imread(nameImage2);
    pyrDown(image2,image2);
    pyrDown(image2,image2);
    Mat gray_image1;
    Mat gray_image2;
    cvtColor(image1, gray_image1, cv::COLOR_RGB2GRAY);
    cvtColor(image2, gray_image2, cv::COLOR_RGB2GRAY);
    // imshow("",gray_image1);
    // waitKey();
    double minHessian = 1000 ;
    Ptr<SURF> detector = SURF::create(minHessian);
    std::vector<KeyPoint> keypointsImage1;
    std::vector<KeyPoint> keypointsImage2;
    Mat img_keypoints;
    detector->detect(gray_image1, keypointsImage1);
    detector->detect(gray_image2, keypointsImage2);

    Mat descriptorImage1, descriptorImage2;
    detector->compute(gray_image1, keypointsImage1, descriptorImage1);
    detector->compute(gray_image2, keypointsImage2, descriptorImage2);

    BFMatcher bruteforceMatcher = BFMatcher(cv::NORM_L2 , true);

    std::vector<DMatch> matches;
    bruteforceMatcher.match(descriptorImage1, descriptorImage2, matches);
    
    Mat imageMatch;
    // drawMatches(image1,keypointsImage1,image2,keypointsImage2,matches,imageMatch);
    // imshow("before",imageMatch);
    // waitKey();
    std::vector<DMatch> goodMatch = filterMatches(matches, 1);
    // drawMatches(image1,keypointsImage1,image2,keypointsImage2,goodMatch,imageMatch);
    // imshow("after",imageMatch);
    // waitKey();

    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for(int i = 0; i< goodMatch.size(); i++)
    {
        scene.push_back(keypointsImage1[goodMatch[i].queryIdx].pt);
        obj.push_back(keypointsImage2[goodMatch[i].trainIdx].pt);
    }

    Mat Homography = findHomography(obj,scene, RANSAC);

    int row = image1.rows;
    int col = image2.cols;

    cv::Mat targetImage;
    warpPerspective(image2,targetImage,Homography,cv::Size(image1.cols+image2.cols,image1.rows+image2.rows));
    cv::Mat half(targetImage,cv::Rect(0,0,image2.cols,image2.rows));
    image1.copyTo(half);
    imshow( "Result", targetImage );
    
    waitKey(0);

    // Rect targetRect = Rect(0,0,col*2,row*2);
    // Mat targetImage = image2(targetRect);
    // warpPerspective(image1,targetImage,Homography,Size(col*2,row*2));
    // imshow("target",targetImage);
    waitKey();
    return 0;
}   