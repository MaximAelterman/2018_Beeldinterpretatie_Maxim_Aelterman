#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @movie m || path to movie }"
    );

    string movPath = parser.get<string>("@movie");

    if(movPath.empty()){
        cerr << "Something went wrong with your arguments: --h for help" << endl;
        return -1;
    }

    VideoCapture mov(movPath);
    if(!mov.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while(1)
    {
        Mat frame;
        mov.read(frame);

        if(frame.empty()) break;
        imshow("Faces", frame);

            //exit when esc is pressed
        char c=(char)waitKey(25);
        if(c==27) break;
    }

}
