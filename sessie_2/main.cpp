#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define RED 2
#define GREEN 1
#define BLUE 0

void segment_bgr(Mat image);
void segment_hsv(Mat image);
static void trackbar_callback(int, void *);

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

    mask = ((R>100) & (G<60) & (B<60));

    namedWindow("Red channel", WINDOW_AUTOSIZE);
    imshow("Red channel", mask);

    waitKey(0);
    return;
}

void segment_hsv(Mat image){
    Mat img_hsv;

    int hue_low = 40;
    int hue_high = 180;
    int sat_low = 130;
    int sat_high = 255;
    int val_low = 80;
    int val_high = 250;

    cvtColor(image, img_hsv, CV_BGR2HSV);

    namedWindow("Mask", WINDOW_AUTOSIZE);
    namedWindow("Cleaned mask", WINDOW_AUTOSIZE);
    namedWindow("Contour", WINDOW_AUTOSIZE);
    namedWindow("HSV - Press q to quit");

    createTrackbar("hue_low", "HSV - Press q to quit", &hue_low, 180, trackbar_callback);
    createTrackbar("hue_high", "HSV - Press q to quit", &hue_high, 360, trackbar_callback);     //hue_high goes to 360 so its possible to include a wider range of red in the mask
    createTrackbar("sat_low", "HSV - Press q to quit", &sat_low, 255, trackbar_callback);
    createTrackbar("sat_high", "HSV - Press q to quit", &sat_high, 255, trackbar_callback);
    createTrackbar("val_low", "HSV - Press q to quit", &val_low, 250, trackbar_callback);
    createTrackbar("val_high", "HSV - Press q to quit", &val_high, 250, trackbar_callback);

    while(true){        //infinite loop to refresh windows
        Mat mask;
        Mat mask_temp;
        Mat mask_cleaned;
        Mat result;
        Mat combined;

        if(hue_high > 180){
            inRange(img_hsv, Scalar(hue_low, sat_low, val_low), Scalar(180, sat_high, val_high), mask);             //hue of red below 180°
            inRange(img_hsv, Scalar(0, sat_low, val_low), Scalar(hue_high-180, sat_high, val_high), mask_temp);     //hue of red above 180° (or 0°)
            mask |= mask_temp;      //or the two masks to combine them
        }
        else inRange(img_hsv, Scalar(hue_low, sat_low, val_low), Scalar(hue_high, sat_high, val_high), mask);

        imshow("Mask", mask);

                //clean up noise
        dilate(mask, mask_cleaned, Mat(), Point(-1, -1), 6);                            //dilate 6 times
        erode(mask_cleaned, mask_cleaned, Mat(), Point(-1, -1), 6);                     //erode 6 times
        imshow("Cleaned mask", mask_cleaned);

                //connected component analysis
        vector<vector<Point> > contour;
        findContours(mask_cleaned, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        vector<Point> biggest_blob;

        if(contour.size() > 0){                 //program used to crash when no contours were found
            biggest_blob = contour[0];

            for(int i = 0; i < contour.size(); i++) {
                if(contourArea(contour[i]) > contourArea(biggest_blob)){
                    biggest_blob = contour[i];
                }
            }

            vector<vector<Point>> temp;
            temp.push_back(biggest_blob);

            vector<vector<Point>>hull(contour.size());                          //a convex hull around objects
            for(int i = 0; i < temp.size(); i++){
                convexHull(Mat(temp[i]), hull[i], false);
            }

            drawContours(mask_cleaned, hull, 0, 255, -1, 8);                    //-1: fill contours to get blobs
        }

        imshow("Contour", mask_cleaned);

        image.copyTo(combined, mask_cleaned);
        imshow("HSV - Press q to quit", combined);

        char key = (char) waitKey(1);                   //waitKey(1): 1ms delay to prevent the program from freezing
        if (key == 'q') break;
    }

    return;
}

static void trackbar_callback(int, void *){

}
