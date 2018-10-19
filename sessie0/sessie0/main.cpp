#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void readImageGray(string image){
    //vector<Mat> channels;
    //split(image, channels);
    //imshow("blue channel", channels[0])
    //imshow("blue channel", channels[1])
    //imshow("blue channel", channels[2])
    cout << "image read" << endl;
    return;
}

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ image_gray ig || (required) path to image }"
    "{ image_color ig || (required) path to image }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

//argumenten inlezen
    string image_gray_location(parser.get<string>("image_gray"));
    string image_color_location(parser.get<string>("image_gray"));

    if(image_gray_location.empty() || image_color_location.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        parser.printMessage();
        return -1;
    }

    if(parser.has("image_gray")){
        readImageGray(image_gray_location);
        return 0;
    }

    return 0;
}
