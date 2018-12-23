#include <iostream>
#include <opencv2/opencv.hpp>

#define COINPATH "bin/Debug/coin_picked_up.png"
#define DEATHPATH "bin/Debug/death.png"
#define KILLPATH "bin/Debug/kill.png"

//576 × 360

using namespace std;
using namespace cv;

void detectCoin(Mat frame);
bool detectDeath(Mat frame);
void detectKill(Mat frame, bool deathDetected);

Mat coin;
Mat death;
Mat kill;

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
    bool deathDetected = false;
    coin = imread(COINPATH);
    death = imread(DEATHPATH);
    kill = imread(KILLPATH);

    cvtColor(coin, coin, COLOR_BGR2GRAY);       //convert coin template into grayscale
    cvtColor(death, death, COLOR_BGR2GRAY);
    cvtColor(kill, kill, COLOR_BGR2GRAY);

    if(!vid.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while(1)
    {        Mat frame;
        vid >> frame;
        if(frame.empty()) break;

        rectangle(frame, Point(0, 0), Point(110, 70), CV_RGB(50, 50, 50), CV_FILLED);
        detectCoin(frame);
        deathDetected = detectDeath(frame);
        detectKill(frame, deathDetected);

                    //exit when esc is pressed
        char c=(char)waitKey(20);       //25
        if(c==27) break;    }

    return 0;
}

void detectCoin(Mat frame)
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
            cout << "Coin: " << maxVal << endl;
            coins++;
        }

        prevDet = 3;
    }
    else if(prevDet > 0)
    {
        prevDet--;
    }

    putText(frame, "Coins: "+to_string(coins), Point(10, 20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    imshow("Mario", frame);
    return;
}

bool detectDeath(Mat frame)
{
    static int prevDet;
    static int deaths = 0;
    bool deathDetected = false;
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

        if(prevDet == 0)       //prevent multiple detections
        {
            cout << "Death: " << maxVal << endl;
            deaths++;
        }
        deathDetected = true;
        prevDet = 20;
    }
    else if(prevDet > 0)
    {
        prevDet--;
    }

    putText(frame, "Deaths: "+to_string(deaths), Point(10, 40), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    imshow("Mario", frame);
    return deathDetected;
}

void detectKill(Mat frame, bool deathDetected)
{
    static int prevDet;
    static int kills = 0;
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

    if(deathDetected) prevDet = 10;      //prevents a death from being detected as a kill
        // Create the result matrix
    int result_cols = frame_gray.cols - kill.cols + 1;
    int result_rows = frame_gray.rows - kill.rows + 1;
    Mat result;

    result.create(result_rows, result_cols, CV_32FC1);

        //Do the Matching
    matchTemplate(frame_gray, kill, result, TM_CCOEFF_NORMED);

        //Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    if(maxVal > 0.80)
    {
            //rectangle around the found object
        rectangle(frame, maxLoc, Point( maxLoc.x + kill.cols , maxLoc.y + kill.rows), Scalar(0, 255, 0), 2, 8, 0);
        Vec3b punt = result.at<Vec3b>(maxLoc);

        if(prevDet == 0)       //prevent multiple detections
        {
            cout << "Kill: " << maxVal << endl;
            kills++;
        }

        prevDet = 10;
    }
    else if(prevDet > 0)
    {
        prevDet--;
    }

    putText(frame, "Kills: "+to_string(kills), Point(10, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    imshow("Mario", frame);
    return;
}
