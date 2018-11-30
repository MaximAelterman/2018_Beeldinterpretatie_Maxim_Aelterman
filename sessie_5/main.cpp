#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point>mousePos;

void makeDescriptor(Mat img);

static void onMouse(int event, int x, int y, int, void*)
{
    if(event == EVENT_LBUTTONDOWN)
    {
        mousePos.push_back(Point(x, y));
    }

    if(event == EVENT_RBUTTONDOWN)
    {
        mousePos.pop_back();
    }

    if(event == EVENT_MBUTTONDOWN)
    {
        cout << mousePos << endl;
    }
}

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image i || path to image }"
    );

    string imgPath = parser.get<string>("@image");

    if(imgPath.empty()){
        cerr << "Something went wrong with your arguments: --h for help" << endl;
        return -1;
    }

    Mat image = imread(imgPath, IMREAD_COLOR);
    Mat img_hsv;

    namedWindow("afbeelding", WINDOW_AUTOSIZE);
    imshow("afbeelding", image);

    setMouseCallback("afbeelding", onMouse, 0);
    waitKey(0);

    GaussianBlur(image, image, Point(5, 5), 0, 0);
    cvtColor(image, img_hsv, CV_BGR2HSV);

    makeDescriptor(img_hsv);
    //Mat trainingDataForeground()


    //vconcat
    //termcriteria

    return 0;
}

void makeDescriptor(Mat img)
{
    for(int i = 0; i < mousePos.size(); i++)
    {
        cout << mousePos[i] << img.at<uchar>(mousePos[i]) << endl;
    }
}

