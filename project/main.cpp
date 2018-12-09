#include <iostream>
#include <opencv2/opencv.hpp>

#define COINPATH "bin/Debug/coin_picked_up.png"
#define DEATHPATH "bin/Debug/death.png"

using namespace std;
using namespace cv;

int detectCoin(Mat frame);
int detectDeath(Mat frame);

Mat coin;
Mat death;

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @video v || path to video }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

    string vidPath = parser.get<string>("@video");

    if(vidPath.empty()){
        cerr << "Something went wrong with your arguments: --h for help" << endl;
        return -1;
    }

    VideoCapture vid(vidPath);
    coin = imread(COINPATH);
    death = imread(DEATHPATH);

    cvtColor(coin, coin, COLOR_BGR2GRAY);       //convert coin template into grayscale
    cvtColor(death, death, COLOR_BGR2GRAY);

    if(!vid.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while(1)
    {        Mat frame;
        vid >> frame;
        if(frame.empty()) break;

        detectCoin(frame);
        detectDeath(frame);

                    //exit when esc is pressed
        char c=(char)waitKey(30);
        if(c==27) break;    }

    return 0;
}

int detectCoin(Mat frame)
{
    static int prevDet;
    static int coins = 0;
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // Create the result matrix
    int result_cols = frame_gray.cols - coin.cols + 1;
    int result_rows = frame_gray.rows - coin.rows + 1;
    Mat result;

    result.create(result_rows, result_cols, CV_32FC1);

        // Do the Matching
    matchTemplate(frame_gray, coin, result, TM_CCOEFF_NORMED);//TM_CCORR_NORMED);
    //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

            // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    if(maxVal > 0.65)
    {
            //rectangle around the found object
        rectangle(frame, maxLoc, Point( maxLoc.x + coin.cols , maxLoc.y + coin.rows), Scalar(0, 255, 0), 2, 8, 0);
        Vec3b punt = result.at<Vec3b>(maxLoc);

        if(prevDet == 0)       //prevent multiple detections of the same coin
        {
            cout << maxVal << endl;
            coins++;
        }

        prevDet = 3;
    }
    else if(prevDet > 0)
    {
        prevDet--;
    }

    putText(frame, "Coins: "+to_string(coins), Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    imshow("Mario", frame);
    return 0;
}

int detectDeath(Mat frame)
{
    static int prevDet;
    static int deaths = 0;
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // Create the result matrix
    int result_cols = frame_gray.cols - coin.cols + 1;
    int result_rows = frame_gray.rows - coin.rows + 1;
    Mat result;

    result.create(result_rows, result_cols, CV_32FC1);

        // Do the Matching
    matchTemplate(frame_gray, death, result, TM_CCOEFF_NORMED);

            // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    if(maxVal > 0.65)
    {
            //rectangle around the found object
        rectangle(frame, maxLoc, Point( maxLoc.x + death.cols , maxLoc.y + death.rows), Scalar(0, 255, 0), 2, 8, 0);
        Vec3b punt = result.at<Vec3b>(maxLoc);

        if(prevDet == 0)       //prevent multiple detections of the same coin
        {
            cout << maxVal << endl;
            deaths++;
        }

        prevDet = 3;
    }
    else if(prevDet > 0)
    {
        prevDet--;
    }

    putText(frame, "Deaths: "+to_string(deaths), Point(30, 50), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    imshow("Mario", frame);
    return 0;
}
