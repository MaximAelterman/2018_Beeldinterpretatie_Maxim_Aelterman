#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image || path to color image }"
    "{ @template || path to template }"
    );

    if(parser.has("help")){
        parser.printMessage();
        return 0;
    }

    string imgPath = parser.get<string>("@image");
    string templatePath = parser.get<string>("@template");

        if(imgPath.empty() || templatePath.empty()){
        cerr << "Something went wrong with your arguments: --h for help" << endl;
        return -1;
    }

    cout << "opening " << imgPath << " as image and " << templatePath << " as template..." << endl;

    Mat img = imread(imgPath);
    Mat templ = imread(templatePath);
    Mat result;

    namedWindow("Input image", WINDOW_AUTOSIZE);
    imshow("Input image", img);

    namedWindow("Template", WINDOW_AUTOSIZE);
    imshow("Template", templ);

        //convert to grayscale
    cvtColor(img, img, COLOR_BGR2GRAY);
    cvtColor(templ, templ, COLOR_BGR2GRAY);

    Mat img_display;
    img.copyTo(img_display);

    int match_method  = TM_CCOEFF_NORMED;    //gives the best results

        // Create the result matrix
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

        // Do the Matching and Normalize
    matchTemplate(img, templ, result, match_method);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

        // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

        // CV_TM_SQDIFF and TM_CCOEFF have their best matches in the lowest values the others in the highest values
    if(match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED || match_method == TM_CCOEFF_NORMED){
        matchLoc = minLoc;
    }
    else{
        matchLoc = maxLoc;
    }

        //rectangle around the found object
    rectangle(img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0);
    rectangle(result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0);

        //multiple object matching
    Mat mask;

    threshold(result, mask, 0.9, 1, THRESH_BINARY);
    mask.convertTo(mask, CV_8UC1);
    mask *= 255;                    //the mask has values between 0 and 1 which wont be visible for us


    vector<vector<Point> > contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    if(contours.size() > 0){
        for(int i = 0; i < contours.size(); i++){
            Rect region = boundingRect(contours[i]);
            Mat temp = result(region);
            Point maxLoc1;
            minMaxLoc(temp, NULL, NULL, &maxLoc1);
            rectangle(img_display, Point(region.x + maxLoc1.x, region.y + maxLoc1.y), Point(maxLoc1.x + region.x + templ.cols, region.y + maxLoc1.y + templ.rows), Scalar::all(0), 2, 8, 0);
        }
    }

    namedWindow("Mask", WINDOW_AUTOSIZE);
    imshow("Mask", mask);

    namedWindow("Result", WINDOW_AUTOSIZE);
    imshow("Result", img_display);

    namedWindow("Match map", WINDOW_AUTOSIZE);
    imshow("Match map", result);

    waitKey(0);
    return 0;
}
