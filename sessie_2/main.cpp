#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define RED 2
#define GREEN 1
#define BLUE 0

void segment_bgr(Mat image);
void segment_hsv(Mat image);

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image i || path to color image }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

    string imgPath = parser.get<string>("@image");

    if(imgPath.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        return -1;
    }

    Mat image = imread(imgPath, IMREAD_COLOR);
    segment_bgr(image);
    segment_hsv(image);

    return 0;
}

void segment_bgr(Mat image){
        //original image
    namedWindow("Input image", WINDOW_AUTOSIZE);
    imshow("Input image", image);

        //channels of image
    vector<Mat> channels;
    split(image, channels);         //split into BGR channels
    Mat mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Mat R = channels[RED];
    Mat G = channels[GREEN];
    Mat B = channels[BLUE];

    mask = ((R>120) & (G<100) & (B<100));

    floodFill(mask, cv::Point(0,0), Scalar(255));

    for(int row = 0; row < mask.rows; row++){
        for(int col = 0; col < mask.cols; col++){
            if(mask.at<uchar>(row, col) == 255) mask.at<uchar>(row, col) = 0;
            else mask.at<uchar>(row, col) = 255;
        }
    }

    namedWindow("Red channel", WINDOW_AUTOSIZE);
    imshow("Red channel", mask);

    waitKey(0);
    return;
}

void segment_hsv(Mat image){
    Mat img_hsv;
    Mat result;
    Mat combined;
    cvtColor(image, img_hsv, CV_BGR2HSV);
    Mat channelHSV[3];
    split(img_hsv, channelHSV);

    Mat H = channelHSV[0];
    Mat S = channelHSV[1];
    Mat V = channelHSV[2];

    Mat h_dest1, h_dest2, s_dest;
    inRange(H, 168, 180, h_dest1);      //red is around 180, but 180+1 = 0 here. so call twice
    inRange(H, 0, 10, h_dest2);
    inRange(S, 100, 255, s_dest);

    H = (h_dest1 | h_dest2) & s_dest;   //combine the results from above into a mask
    namedWindow("H threshold", WINDOW_AUTOSIZE);
    imshow("H threshold", H);

        //erode and dilate to clean up the image
    erode(H, result, Mat(), Point(-1, -1), 6);           //erode 2 times
    dilate(result, result, Mat(), Point(-1, -1), 6);       //dilate 6 times
    namedWindow("Cleaned", WINDOW_AUTOSIZE);
    imshow("Cleaned", result);

        //connected component analysis
    vector<vector<Point> > contour;
    findContours(result, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    vector<Point> biggest_blob = contour[0];
    for(int i = 0; i < contour.size(); i++) {
        if(contourArea(contour[i]) > contourArea(biggest_blob)){
            biggest_blob = contour[i];
        }
    }
    vector<vector<Point>> temp;
    temp.push_back(biggest_blob);

    vector<vector<Point>>hull(contour.size());              //a convex hull around objects
    for(int i = 0; i < temp.size(); i++){
        convexHull(Mat(temp[i]), hull[i], false);
    }

    for(int i = 0; i < contour.size(); i++){
        drawContours(result, hull, i, 255, -1, 8);          //-1: fill contours to get blobs
    }

    namedWindow("Contour", WINDOW_AUTOSIZE);
    imshow("Contour", result);

    image.copyTo(combined, result);
    namedWindow("Masked image", WINDOW_AUTOSIZE);
    imshow("Masked image", combined);

    waitKey(0);
    return;
}
