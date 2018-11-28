#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void orb_keypoints(Mat img, Mat obj);
void brisk_keypoints(Mat img, Mat obj);
void akaze_keypoints(Mat img, Mat obj);

int main(int argc, const char**argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h || show this message }"
    "{ @image i || path to image }"
    "{ @object o || path to object }"
    );

    string imgPath = parser.get<string>("@image");
    string objectPath = parser.get<string>("@object");

    if(imgPath.empty() || objectPath.empty()){
        cerr << "Something went wrong with your arguments: --h for help" << endl;
        return -1;
    }

    Mat image = imread(imgPath, IMREAD_GRAYSCALE);
    Mat object = imread(objectPath, IMREAD_GRAYSCALE);

    namedWindow("afbeelding", WINDOW_AUTOSIZE);
    imshow("afbeelding", image);

    namedWindow("object", WINDOW_AUTOSIZE);
    imshow("object", object);

    orb_keypoints(image, object);
    brisk_keypoints(image, object);
    akaze_keypoints(image, object);

    //waitKey(0);
    return 0;
}

void orb_keypoints(Mat img, Mat obj)
{
    Mat orb_img, orb_obj;
    Ptr<ORB> detector = ORB::create();
    vector<KeyPoint> keypoints_img, keypoints_obj;
    Mat descriptors_img, descriptors_obj;

        //detect the keypoints
    detector->detectAndCompute(obj, Mat(), keypoints_obj, descriptors_obj);
    detector->detectAndCompute(img, Mat(), keypoints_img, descriptors_img);

        //bruteforce matching
    BFMatcher matcher(NORM_L2);
    std::vector<DMatch> matches;

    matcher.match(descriptors_obj, descriptors_img, matches);

        //draw all matches
    Mat img_matches;
    drawMatches(obj, keypoints_obj, img, keypoints_img, matches, img_matches);

        //mark keypoints on images
    drawKeypoints(img, keypoints_img, orb_img, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
    drawKeypoints(obj, keypoints_obj, orb_obj, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);

    namedWindow("ORB image", WINDOW_AUTOSIZE);
    imshow("ORB image", orb_img);

    namedWindow("ORB object", WINDOW_AUTOSIZE);
    imshow("ORB object", orb_obj);

    namedWindow("ORB matches (bruteforce)", WINDOW_AUTOSIZE);
    imshow("ORB matches (bruteforce)", img_matches);

        //homography through RANSAC
    Mat result;
    double max_dist = 0; double min_dist = 100;
    for(int i = 0; i < descriptors_obj.rows; i++){
        double dist = matches[i].distance;
        if(dist < min_dist) min_dist = dist;
        if(dist > max_dist) max_dist = dist;
    }

        //find the good matches
    vector<DMatch> good_matches;
    for(int i = 0; i < descriptors_obj.rows; i++){
        if(matches[i].distance <= 3*min_dist){
            good_matches.push_back(matches[i]);
        }
    }
        //only draw the good matches
    drawMatches(obj, keypoints_obj, img, keypoints_img, good_matches, result, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        //find object in scene
    vector<Point2f> objct, scene;
    for(size_t i = 0; i < good_matches.size(); i++){
        objct.push_back(keypoints_obj[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints_img[good_matches[i].trainIdx].pt);
    }

    Mat H = findHomography(objct, scene, RANSAC);

        //Draw rectangle around object
    vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(obj.cols, 0);
    obj_corners[2] = cvPoint(obj.cols, obj.rows); obj_corners[3] = cvPoint(0, obj.rows);

    vector<Point2f> scene_corners(4);
    perspectiveTransform(obj_corners, scene_corners, H);

    line(result, scene_corners[0] + Point2f(obj.cols, 0), scene_corners[1] + Point2f(obj.cols, 0), Scalar(0, 255, 0), 4);
    line(result, scene_corners[1] + Point2f(obj.cols, 0), scene_corners[2] + Point2f(obj.cols, 0), Scalar(0, 255, 0), 4);
    line(result, scene_corners[2] + Point2f(obj.cols, 0), scene_corners[3] + Point2f(obj.cols, 0), Scalar(0, 255, 0), 4);
    line(result, scene_corners[3] + Point2f(obj.cols, 0), scene_corners[0] + Point2f(obj.cols, 0), Scalar(0, 255, 0), 4);

    namedWindow("ORB matches (RANSAC)", WINDOW_AUTOSIZE);
    imshow("ORB matches (RANSAC)", result);

    waitKey(0);
    destroyAllWindows();
    return;
}

void brisk_keypoints(Mat img, Mat obj)
{
    Mat brisk_img, brisk_obj;
    Ptr<BRISK> detector = BRISK::create();
    vector<KeyPoint> keypoints_img, keypoints_obj;
    Mat descriptors_img, descriptors_obj;

        //detect the keypoints
    detector->detectAndCompute(obj, Mat(), keypoints_obj, descriptors_obj);
    detector->detectAndCompute(img, Mat(), keypoints_img, descriptors_img);

        //bruteforce matching
    BFMatcher matcher(NORM_L2);
    std::vector<DMatch> matches;
    matcher.match(descriptors_obj, descriptors_img, matches);

    Mat img_matches;
    drawMatches(obj, keypoints_obj, img, keypoints_img, matches, img_matches);

        //mark keypoints on images
    drawKeypoints(obj, keypoints_obj, brisk_obj, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
    drawKeypoints(img, keypoints_img, brisk_img, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);

    namedWindow("BRISK image", WINDOW_AUTOSIZE);
    imshow("BRISK image", brisk_img);

    namedWindow("BRISK object", WINDOW_AUTOSIZE);
    imshow("BRISK object", brisk_obj);

    namedWindow("ORB matches (bruteforce)", WINDOW_AUTOSIZE);
    imshow("ORB matches (bruteforce)", img_matches);

    waitKey(0);
    destroyAllWindows();
    return;
}

void akaze_keypoints(Mat img, Mat obj)
{
    Mat akaze_img, akaze_obj;
    Ptr<AKAZE> detector = AKAZE::create();
    vector<KeyPoint> keypoints_img, keypoints_obj;
    Mat descriptors_img, descriptors_obj;

        //detect the keypoints
    detector->detectAndCompute(obj, Mat(), keypoints_obj, descriptors_obj);
    detector->detectAndCompute(img, Mat(), keypoints_img, descriptors_img);

        //bruteforce matching
    BFMatcher matcher(NORM_L2);
    std::vector<DMatch> matches;
    matcher.match(descriptors_obj, descriptors_img, matches);

    Mat img_matches;
    drawMatches(obj, keypoints_obj, img, keypoints_img, matches, img_matches);

        //mark keypoints on images
    drawKeypoints(obj, keypoints_obj, akaze_obj, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
    drawKeypoints(img, keypoints_img, akaze_img, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);

    namedWindow("AKAZE image", WINDOW_AUTOSIZE);
    imshow("AKAZE image", akaze_img);

    namedWindow("AKAZE object", WINDOW_AUTOSIZE);
    imshow("AKAZE object", akaze_obj);

    namedWindow("ORB matches (bruteforce)", WINDOW_AUTOSIZE);
    imshow("ORB matches (bruteforce)", img_matches);

    waitKey(0);
    destroyAllWindows();
    return;
}
