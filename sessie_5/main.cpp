#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace ml;

static void onMouse(int event, int x, int y, int, void*pointer)
{
    vector<Point>*p = (vector<Point>*)pointer;
    if(event == EVENT_LBUTTONDOWN)
    {
        p->push_back(Point(x, y));
        cout << "Point " << Point(x, y) << " added." << endl;
    }

    if(event == EVENT_RBUTTONDOWN)
    {
        p->pop_back();
        cout << "Last point removed." << endl;
    }

    if(event == EVENT_MBUTTONDOWN)
    {
        cout << "" << endl;
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

    Mat image = imread(imgPath);
    Mat img_hsv;

    vector<Point>strawberry;
    vector<Point>background;

    namedWindow("afbeelding", WINDOW_AUTOSIZE);
    imshow("afbeelding", image);

        //selection of foreground
    cout << "Select the strawberries in the picture." << endl;
    setMouseCallback("afbeelding", onMouse, &strawberry);
    waitKey(0);

        //selection of background
    cout << "Select parts of the picture which do not contain strawberries." << endl;
    setMouseCallback("afbeelding", onMouse, &background);
    waitKey(0);

/*
    Mat bgr[3];
    split(image, bgr);
    bgr[1] = Mat::zeros(image.rows, image.cols, CV_8UC1);
    merge(bgr, 3, image);
*/
    cvtColor(image, img_hsv, CV_BGR2HSV);
    GaussianBlur(img_hsv, img_hsv, Size(5, 5), 0);

        //make descriptors with the selected points
            //foreground
    Mat trainingDataForeground(strawberry.size(), 3, CV_32FC1);
    Mat labels_fg = Mat::ones(strawberry.size(), 1, CV_32FC1);

    for(int i = 0; i < strawberry.size(); i++)
    {
        Vec3b descriptor = img_hsv.at<Vec3b>(strawberry[i]);
        trainingDataForeground.at<float>(i, 0) = descriptor[0];
        trainingDataForeground.at<float>(i, 1) = descriptor[1];
        trainingDataForeground.at<float>(i, 2) = descriptor[2];
    }

            //background
    Mat trainingDataBackground(background.size(), 3, CV_32FC1);
    Mat labels_bg = Mat::zeros(background.size(), 1, CV_32FC1);

    for(int i = 0; i < background.size(); i++)
    {
        Vec3b descriptor = img_hsv.at<Vec3b>(background[i]);
        trainingDataBackground.at<float>(i, 0) = descriptor[0];
        trainingDataBackground.at<float>(i, 1) = descriptor[1];
        trainingDataBackground.at<float>(i, 2) = descriptor[2];
    }

        //group foreground and background
    Mat trainingData;
    Mat labels;

    vconcat(trainingDataForeground, trainingDataBackground, trainingData);
    vconcat(labels_fg, labels_bg, labels);

        //K-nearest
    Ptr<TrainData> trainingD;
    Ptr<KNearest> kclassifier = KNearest::create();

    trainingD = TrainData::create(trainingData, ROW_SAMPLE, labels);
    kclassifier->setIsClassifier(true);
    kclassifier->setAlgorithmType(KNearest::BRUTE_FORCE);
    kclassifier->setDefaultK(3);
    kclassifier->train(trainingD);

    cout << "K-nearest training complete!" << CV_VERSION << endl;

        //Normal bayes
    Ptr<NormalBayesClassifier> bayes = NormalBayesClassifier::create();
    bayes->train(trainingD, ROW_SAMPLE, labels);

    cout << "Normal Bayes training complete!" << endl;

        //SVM
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainingD, ROW_SAMPLE, labels);

    cout << "SVM training complete!" << endl;

    Mat labels_knn, labels_normalBayes, labels_svm;
    Mat result_knn = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Mat result_nb = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Mat result_svm = Mat::zeros(image.rows, image.cols, CV_8UC1);

        //loop over every pixel to find the strawberries
    for(int i=0; i < image.rows; i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            Vec3b pixel = img_hsv.at<Vec3b>(i, j);
            Mat data(1,3, CV_32FC1);
            data.at<float>(0,0) = pixel[0];
            data.at<float>(0,1) = pixel[1];
            data.at<float>(0,2) = pixel[2];

            kclassifier->findNearest(data, kclassifier->getDefaultK(), labels_knn);
            bayes->predict(data, labels_normalBayes);
            svm->predict(data, labels_svm);

            // Convert the matched labels to a binary mask
            result_knn.at<uchar>(i,j) = (uchar)labels_knn.at<float>(0,0);
            result_nb.at<uchar>(i,j) = (uchar)labels_normalBayes.at<int>(0,0);
            result_svm.at<uchar>(i,j) = (uchar)labels_svm.at<float>(0,0);
        }
    }

    imshow("result Knn", result_knn*255);
    imshow("result Normal Bayes", result_nb*255);
    imshow("result Svm", result_svm*255);

    waitKey(0);
    return 0;
}
