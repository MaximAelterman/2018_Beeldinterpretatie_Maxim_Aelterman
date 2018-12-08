#include <iostream>
#include <opencv2/opencv.hpp>   //`pkg-config --libs opencv`

using namespace std;
using namespace cv;

void detectPerson(Mat frame);

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
        mov >> frame;

        if(frame.empty()) break;

        detectPerson(frame);

            //exit when esc is pressed
        char c=(char)waitKey(25);
        if(c==27) break;
    }

    mov.release();
    destroyAllWindows();
}

void detectPerson(Mat frame)
{
    HOGDescriptor hog;
    vector<Rect> person;
    vector<double> weight;
    static vector<Point> trackline;

    resize(frame, frame, Size(frame.cols*2, frame.rows*2));     //double frame size to increase detection performance

        //detect person
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());      //initialize descriptor as person detector
    hog.detectMultiScale(frame, person, weight);

        //draw rectangles around detections
    for(int i = 0; i < person.size(); i++)
    {
        if(weight[i] >= 0.5)        //threshold to reduce false detections
        {
            rectangle(frame, person[i], Scalar(0, 255, 0));
            putText(frame, to_string(weight[i]), Point(person[i].x, person[i].y), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);      //show the weight of current detection
            trackline.push_back(Point(person[i].x + (person[i].width/2), person[i].y + (person[i].height/2)));       //save the middle of the detection for tracking
        }
    }

        //visualize the path that the person took
    for(int i = 1; i < trackline.size(); i++)   //start at 1 so the trackline[i-1] isnt out of bounds
    {
        line(frame, trackline[i-1], trackline[i], Scalar(0, 0, 255), 3);
    }

    imshow("Person", frame);
    return;
}
