#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void readImage(string image){
//Colored
    Mat img_color = imread(image);
    namedWindow("Colored", WINDOW_AUTOSIZE);
    imshow("Colored", img_color);

//split in its red, green and blue components
    vector<Mat> channels;
    split(img_color, channels);

        //Blue
    namedWindow("Blue channel", WINDOW_AUTOSIZE);
    imshow("Blue channel", channels[0]);
        //Green
    namedWindow("Green channel", WINDOW_AUTOSIZE);
    imshow("Green channel", channels[1]);
        //Red
    namedWindow("Red channel", WINDOW_AUTOSIZE);
    imshow("Red channel", channels[2]);

//Grayscale
    Mat img_gray;
    cvtColor(img_color, img_gray, COLOR_BGR2GRAY);      //convert to image to grayscale
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", img_gray);

    int rows = img_gray.rows;
    int cols = img_gray.cols;

    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            cout << img_gray.at<u_int>(row, col) << " ";
        }
        cout << endl;
    }

    cout << "image read" << endl;
    return;
}

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image i || (required) path to image }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

//capture arguments
    string image_location(parser.get<string>("@image"));

    cout << "Image location: " << image_location << endl;

    Mat img = imread(image_location, IMREAD_COLOR);

//self-made drawing with opencv draw functions
    Mat drawing = Mat(177, 285, CV_8UC3, Scalar(255, 255, 255));        //White surface the size of the images
    rectangle(drawing, Point(40, 60), Point(250, 160), Scalar(255, 0, 0), 2, 8);
    line(drawing, Point(40, 60), Point(145, 10), Scalar(255, 0, 0), 2, 8);
    line(drawing, Point(145, 10), Point(250, 60), Scalar(255, 0, 0), 2, 8);
    circle(drawing, Point(20, 20), 30, Scalar(0, 255, 255), -1, 8);     //negative thickness: filled circle
    namedWindow("Drawing", WINDOW_AUTOSIZE);
    imshow("Drawing", drawing);

    if(img.empty()){
        cerr << "Image not found." << endl;
        parser.printMessage();
        //return -1;
    }

    if(image_location.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        parser.printMessage();
        //return -1;
    }

    if(parser.has("@image")){
        readImage(image_location);
        //return 0;
    }

    waitKey(0);         //wait after everything is drawn so we can see the results
    return 0;
}
