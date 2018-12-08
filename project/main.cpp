#include <iostream>
#include <opencv2/opencv.hpp>

#define COINPATH "bin/Debug/coin_picked_up_small.png"

using namespace std;
using namespace cv;

int detectCoin(Mat frame);

Mat coin;

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

    cvtColor(coin, coin, COLOR_BGR2GRAY);       //convert coin template into grayscale

    if(!vid.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while(1)
    {        Mat frame;
        vid >> frame;
        if(frame.empty()) break;

        detectCoin(frame);

                    //exit when esc is pressed
        char c=(char)waitKey(30);
        if(c==27) break;    }

    return 0;
}

int detectCoin(Mat frame)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // Create the result matrix
    int result_cols = frame_gray.cols - coin.cols + 1;
    int result_rows = frame_gray.rows - coin.rows + 1;
    Mat result;

    result.create(result_rows, result_cols, CV_32FC1);

        // Do the Matching and Normalize
    matchTemplate(frame_gray, coin, result, TM_CCOEFF_NORMED);//TM_CCORR_NORMED);
    //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

            // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    if(maxVal > 0.725)//0.835)
    {
            //rectangle around the found object
        rectangle(frame, maxLoc, Point( maxLoc.x + coin.cols , maxLoc.y + coin.rows ), Scalar(0, 255, 0), 2, 8, 0);
        Vec3b punt = result.at<Vec3b>(maxLoc);
        cout << maxVal << endl;
    }

    imshow("Mario", frame);
    imshow("result", result);
    return 0;
}
