#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <stdio.h>

#include "pseye/Labeling.h"

using namespace std;
using namespace cv;
/*
class ObjDetect{
    
    cv::Mat backImg;
    cv::Mat frame;
    cv::Mat grayFrame;
    cv::Mat diff;
    cv::Mat texture;
    float area1 = 0;
    float area = 0;
    cv::Rect brect;
    int roiCnt = 0;
    int i = 0;
    
    float cx;
    float cy;
    
public:
    
    ObjDetect(void) {
        // Constructer
    }
    
    int getBackImg(){
        
        cout << "please press key" << endl;
        
       // getchar();
        
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "camera cannot open" << endl;
            return -1;
        }
        cap >> backImg;
        
        cv::cvtColor(backImg, backImg, CV_BGR2GRAY);
        
        cv::Mat smallframe;
        resize(backImg, smallframe, Size(640,480));
        backImg = smallframe;
        
        return 0;
        
    }
    
    
    bool detectObject(){
        cv::VideoCapture cap1(0);
        if (!cap1.isOpened())
        {
            cerr << "camera cannot open" << endl;
            return -1;
        }
        cap1 >> frame;
        cv::Mat smallframe;
        resize(frame, smallframe, Size(640,480));
        frame = smallframe;
        
        
        
        cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
        cv::absdiff(backImg,grayFrame,diff);
        cv::threshold(diff,diff,20,255,CV_THRESH_BINARY);
        
        cv::erode(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::erode(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        
        vector< vector< cv::Point > > contours;
        cv::findContours(diff, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            vector< cv::Point > approx;
            cv::approxPolyDP(cv::Mat(*contour), approx, 0.01 * cv::arcLength(*contour, true), true);
            area1 = cv::contourArea(approx);
        }
        
        if(area1 > 1000)
            return true;
        else
            return false;
        
    }
    
    
    void makeTexture(){
        vector< vector< cv::Point > > contours;
        vector< cv::Point > approx;
        cv::findContours(diff, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        
        cv::Moments mom = cv::moments(contours[i]);
        cx = mom.m10/mom.m00;
        cy = mom.m01/mom.m00;
        
        cout << "重心座標（"<< cx << "," << cy << "）" << endl;
        
        float maxarea = 0;
        
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            cv::convexHull(*contour, approx);
            area = cv::contourArea(approx);
            
            if (area > maxarea) {
                maxarea = area;
                brect = cv::boundingRect(cv::Mat(approx).reshape(2));
                texture = cv::Mat(frame, brect);
            }
            
            i++;
        }
       
        //w = texture.cols;
        //h = texture.rows;

    }
    
    
    cv::Mat getTexture(){
        
        return texture;
        
    }
    
    float getCenterX(){
        
        return cx;
        
    }
    
    
    
};
*/



class ObjectDetector
{
    VideoCapture* video_;
    
    Mat bgAccum;
    Mat backgournd;
    Mat foreground;
    Mat objImage;
    int count = 0;
    
    cv::Rect objRect_;
    
    bool found = false;
    enum {
        GET_BG,
        FIND_OBJ,
        OBJ_FOUND
    } state = GET_BG;
public:
    ObjectDetector() {}
    
    void setCapture(cv::VideoCapture* vc)
    {
        video_ = vc;
    }
    
    void init()
    {
        bgAccum = Mat(Size(640, 480), CV_32FC3, Scalar::all(0));
        count = 0;
    }
    
    void update()
    {
        if(state == GET_BG) {
            calcBackground();
        }else {
            found = findObject();
        }
    }
    
    bool isFound() { return found; }
    
    cv::Rect getObjectRect() { return objRect_; }
    
    cv::Mat getTexture()
    {
        if(!objImage.empty())
            return objImage(objRect_).clone();
        else
            return Mat();
    }
    
    cv::Mat getBackground() { return backgournd; }
    cv::Mat getForeground() { return foreground; } 
private:
    
    void calcBackground()
    {
        cout << "Capturing background..." << endl;
        Mat bgr;
        *video_ >> bgr;
        
        Mat fbgr;
        bgr.convertTo(fbgr, CV_32FC3);
        
        bgAccum += fbgr;
        count ++;
        
        const int BG_COUNT = 10;
        if(count > BG_COUNT) {
            bgAccum *= 1.f/count;
            
            bgAccum.convertTo(backgournd, CV_8UC3);
            
            imshow("bg", backgournd);
            state = FIND_OBJ;
            
            cout << "Finding object." << endl;
            
            count = 0;
        }
    }
    
    bool findObject()
    {
        Mat bgr;
        *video_ >> bgr;
        
        Mat diff;
        cv::absdiff(backgournd, bgr, diff);
        
        cvtColor(diff, diff, COLOR_BGR2GRAY);
        
        erode(diff, diff, Mat());
        erode(diff, diff, Mat());
        erode(diff, diff, Mat());
        dilate(diff, diff, Mat());
        dilate(diff, diff, Mat());
        dilate(diff, diff, Mat());
        
        Mat obj;
        cv::threshold(diff, obj, 50, 255, THRESH_BINARY);
        
        typedef Labeling<uchar, uchar> Labeling8U;
        Labeling8U labeling;
        
        cv::Mat labeled(obj.size(), CV_8U);
        labeling.Exec(obj.ptr(), labeled.ptr(), obj.size().width, obj.size().height, true, 10);
        
        if(labeling.GetNumOfResultRegions() > 0)
        {
            Labeling8U::RegionInfo* ri = labeling.GetResultRegionInfo(0);
            
            //ri->GetCenter(pos_.x, pos_.y);
            
            Rect r;
            int mx,my,sx,sy;
            ri->GetMin(mx, my);
            ri->GetSize(sx, sy);
            
            r.x = mx;
            r.y = my;
            r.width = sx;
            r.height = sy;
            
            objRect_ = r;
            
            const int OBJ_FIND_COUNT = 50;
            if(ri->GetNumOfPixels() > 200) {
                
                count ++;
                if(count > OBJ_FIND_COUNT) {
                
                    objImage = bgr.clone();
                    
                    Mat fore(backgournd.size(), CV_8UC4, Scalar::all(0));
                    
                    for(int i=0; i<fore.size().area(); ++i) {
                        Vec3b c = objImage.at<Vec3b>(i);
                        uchar a = obj.at<uchar>(i) < 255 ? 0 : 255;
                        
                        fore.at<Vec4b>(i) = Vec4b(c[2],c[1],c[0],a);
                    }
                    foreground = fore;
                    
                    cvtColor(bgr, backgournd, COLOR_BGR2RGB);
//                    backgournd = bgr.clone();
                    
                    state = OBJ_FOUND;
                    
                    /*CV_EXPORTS_W void rectangle(InputOutputArray img, Point pt1, Point pt2,
                     const Scalar& color, int thickness = 1,
                     int lineType = LINE_8, int shift = 0);*/
                    
                    rectangle(bgr, Point(mx,my), Point(mx+sx,my+sy), Scalar(0,0,255));
                    
                    imshow("obj rect", bgr);
                    
                    cout << "Found" << endl;
                    
                    init();
                    return true;
                }
            }
        }
        imshow("obj", obj);

        return false;
        
        
    }
    
};
