#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define RED 2
#define GREEN 1
#define BLUE 0

Mat skinDet(string imgColor);
void textSegment(string imgBimodal);
void erode_dilate(string imgPath, Mat mask);

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @color_image ci || path to color image }"
    "{ @bimodal_image bi || path to image }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

    string imgPath_color = parser.get<string>("@color_image");
    string imgPath_bimodal = parser.get<string>("@bimodal_image");

    if(imgPath_color.empty() || imgPath_bimodal.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        return -1;
    }

    cout << "color image path: " << imgPath_color << endl;
    cout << "bimodal image path: " << imgPath_bimodal << endl;

    //skindetection
    Mat skinMask = skinDet(imgPath_color);

    //textSegmentation
    textSegment(imgPath_bimodal);

    //ruisonderdrukking
    erode_dilate(imgPath_color, skinMask);

    return 0;
}

Mat skinDet(string imgColor){
    Mat image = imread(imgColor, IMREAD_COLOR);

    namedWindow("Color image", WINDOW_AUTOSIZE);
    imshow("Color image", image);

    vector<Mat> channels;
    split(image, channels);

    Mat mask_1 = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Mat R = channels[RED];
    Mat G = channels[GREEN];
    Mat B = channels[BLUE];

    for(int row = 0; row < mask_1.rows; row++){
        for(int col = 0; col < mask_1.cols; col++){
            if((R.at<uchar>(row, col)>95) && (G.at<uchar>(row, col)>40) && (B.at<uchar>(row, col)>20) &&
            ((max(R.at<uchar>(row, col),max(G.at<uchar>(row, col),B.at<uchar>(row, col))) - min(R.at<uchar>(row, col),min(G.at<uchar>(row, col),B.at<uchar>(row, col))))>15) &&
            (abs(R.at<uchar>(row, col)-G.at<uchar>(row, col))>15) && (R.at<uchar>(row, col)>G.at<uchar>(row, col)) && (R.at<uchar>(row, col)>B.at<uchar>(row, col))){
                mask_1.at<uchar>(row, col) = 255;
            }
        }
    }

    namedWindow("Skin", WINDOW_AUTOSIZE);
    imshow("Skin", mask_1);
    waitKey(0);
    cvDestroyAllWindows();      //closes all open windows
    return mask_1;
}

void textSegment(string imgBimodal){
    //OTSU
    Mat mask;
    Mat img_color = imread(imgBimodal);
    Mat img_gray;
    cvtColor(img_color, img_gray, COLOR_BGR2GRAY);      //convert to image to grayscale

    namedWindow("Grayscale image", WINDOW_AUTOSIZE);
    imshow("Grayscale image", img_gray);

    threshold(img_gray, mask, 0, 255, THRESH_OTSU | THRESH_BINARY); //apply OTSU thresholding
    namedWindow("OTSU", WINDOW_AUTOSIZE);
    imshow("OTSU", mask);

    Mat mask_equalized;
    equalizeHist(img_gray, mask_equalized);

    namedWindow("Histogram equalized", WINDOW_AUTOSIZE);
    imshow("Histogram equalized", mask_equalized);

    //CLAHE
    Mat mask_clahe;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15, 15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(img_gray.clone(), mask_clahe);
    namedWindow("CLAHE", WINDOW_AUTOSIZE);
    imshow("CLAHE", mask_clahe);

    //CLAHE threshold
    Mat mask_clahe_thresh;
    threshold(mask_clahe, mask_clahe_thresh, 0, 255, THRESH_OTSU | THRESH_BINARY); //apply OTSU thresholding
    namedWindow("CLAHE thresh", WINDOW_AUTOSIZE);
    imshow("CLAHE thresh", mask_clahe_thresh);

    waitKey(0);
    cvDestroyAllWindows();
    return;
}

void erode_dilate(string imgPath, Mat mask){
    Mat image = imread(imgPath, IMREAD_COLOR);
    Mat combined;
    Mat result;

    erode(mask, result, Mat(), Point(-1, -1), 2);           //erode 2 times
    namedWindow("Erode", WINDOW_AUTOSIZE);
    imshow("Erode", result);

    dilate(result, result, Mat(), Point(-1, -1), 6);       //dilate 6 times
    namedWindow("Erode-dilate", WINDOW_AUTOSIZE);
    imshow("Erode-dilate", result);

    vector<vector<Point> > contour;
    findContours(result, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    Mat drawing = Mat::zeros( result.size(), CV_8UC3 );

    vector<vector<Point>>hull(contour.size());              //a convex hull around objects
    for(int i = 0; i < contour.size(); i++){
        convexHull(Mat(contour[i]), hull[i], false);
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
    cvDestroyAllWindows();
    return;
}
