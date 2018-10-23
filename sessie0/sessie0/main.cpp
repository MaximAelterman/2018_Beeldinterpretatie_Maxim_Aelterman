#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void readImage(string image){
//Grayscale
    Mat img_gray = imread(image, IMREAD_GRAYSCALE);
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", img_gray);

//Colored
    Mat img_color = imread(image, 1);
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
    cout << "image read" << endl;
    waitKey(0);
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

//argumenten inlezen
    string image_location(parser.get<string>("@image"));

    cout << "Image location: " << image_location << endl;

    Mat img = imread(image_location, IMREAD_COLOR);

    Mat drawing = Mat::zeros(400, 400, CV_8UC3);
    line(drawing, Point(15, 20), Point(70, 50), Scalar(0, 0, 255), 2, 8);
    namedWindow("Drawing", WINDOW_AUTOSIZE);
    imshow("Drawing", drawing);

    if(img.empty()){
        cerr << "Image not found." << endl;
        parser.printMessage();
        return -1;
    }

    if(image_location.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        parser.printMessage();
        return -1;
    }

    if(parser.has("@image")){
        readImage(image_location);
        return 0;
    }

    return 0;
}
