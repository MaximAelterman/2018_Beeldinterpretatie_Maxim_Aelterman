#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image i || path to color image }"
    "{ @template t || path to template }"
    );

    string imgPath = parser.get<string>("@image");
    string templatePath = parser.get<string>("@template");

        if(imgPath.empty() || templatePath.empty()){
        cerr << "Something went wrong with your arguments" << endl;
        return -1;
    }

    Mat img = imread(imgPath);
    Mat templ = imread(templatePath);
    Mat result;

    namedWindow("Input image", WINDOW_AUTOSIZE);
    imshow("Input image", img);

    namedWindow("Template", WINDOW_AUTOSIZE);
    imshow("Template", templ);

    Mat img_display;
    img.copyTo( img_display );

    //int match_method  = CV_TM_SQDIFF;
    int match_method  = TM_SQDIFF;

        // Create the result matrix
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );

        // Do the Matching and Normalize
    matchTemplate( img, templ, result, match_method );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

        // Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

        // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if(match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED){
        matchLoc = minLoc;
    }
    else{
        matchLoc = maxLoc;
    }

        //rectangle around the found object
    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );



        //multiple object matching
    Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC1);
    //int tempWidth = templ.cols;
    //int tempHeight = templ.rows;

    threshold(result, mask, 0.9, 1, THRESH_BINARY);


    vector<vector<Point> > contours;
    findContours(result, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    if(contours.size() > 0){
        for(int i = 0; i < contours.size(); i++){
            Rect region = boundingRect(contours[i]);
            Mat temp = result(region);
            Point maxLoc1;
            minMaxLoc(temp, NULL, NULL, &maxLoc1);
            rectangle(img_display, Point(region.x + maxLoc1.x, region.y + maxLoc1.y), Point(maxLoc1.x + region.x + templ.cols, region.y + maxLoc.y), Scalar::all(0), 2, 8, 0);
        }
    }
    //rectangle(mask, Point(0-(tempWidth/2), 0-(tempHeight/2)), Point(0+(tempWidth/2), 0+(tempHeight/2)), Scalar::all(255), CV_FILLED, 8, 0);
    //minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, mask);

    /*
        findcontours van de template matches om hier een bounding box rond te zetten
            OF
        sliding window maar moeilijker
    */

    //namedWindow("Mask", WINDOW_AUTOSIZE);
    //imshow("Mask", mask);

    namedWindow("Result", WINDOW_AUTOSIZE);
    imshow("Result", img_display);

    namedWindow("Match map", WINDOW_AUTOSIZE);
    imshow("Match map", result);

    waitKey(0);
    return 0;
}
