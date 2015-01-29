//
//  MotionDetector.h
//  pseye_
//
//  Created by Yuhei Akamine on 2014/12/28.
//  Copyright (c) 2014年 Yuhei Akamine. All rights reserved.
//

#ifndef pseye__MotionDetector_h
#define pseye__MotionDetector_h

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "PSMoveTracker.h"
#include "psmove.h"


class MotionDetect
{
    std::vector<cv::Point2f> motion_;
    
    float thre_ = 10;
    int i = 1;
    cv::Point2f prevPos_;
    
    double vmin_;
    double distX_, distY_, v_, vmax_;
    double alpha_ = 0.2;
    double duration1_, duration2_;
    
    std::chrono::system_clock::time_point start1_;
    std::chrono::system_clock::time_point start2_;
    
    double match2Cir_;  double match2Tri_;  double match2Squa_;
    bool drawJudge_;
    std::vector<Point2f> pos_;
    std::vector<Point2f> track_, trackDraw_;
    std::vector<Point3f> track1_;
    
    cv::Mat imgLines_;
    cv::Mat grayCircle_, grayTriangle_, graySquare_;
    
    /*
    cv::Mat imgCircle_;
    cv::Mat imgTriangle_;
    cv::Mat imgSquare_;
    */
    std::vector<cv::Mat> shapes_;
    
    cv::Mat point_, imgPoint_;
    
    float velo_;
public:
    MotionDetect() {
        vmin_ = 1.0;    v_ = 0.0;   vmax_ = 0.0;
        distX_ = 0.0;   distY_ = 0.0;
        
        start1_ = std::chrono::system_clock::now();
        duration1_ = 0.0;   duration2_ = 0.0;
        
        match2Cir_ = 0.0;   match2Tri_ = 0.0;   match2Squa_ = 0.0;
        
        drawJudge_ = false;
    }
    
    void init(int winWidth_, int winHeight_) {
        vmin_ = 10.0;    v_ = 0.0;   vmax_ = 0.0;
        distX_ = 0.0;   distY_ = 0.0;
        
        start1_ = std::chrono::system_clock::now();
        duration1_ = 0.0;   duration2_ = 0.0;
        
        match2Cir_ = 0.0;   match2Tri_ = 0.0;   match2Squa_ = 0.0;
        
        drawJudge_ = false;

        imgLines_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);
         /*
        imgCircle_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);
        imgTriangle_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);
        imgSquare_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);*/
        point_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);
        imgPoint_ = cv::Mat::zeros(winHeight_, winWidth_, CV_8UC3);
        
        makeTeacher(winWidth_, winHeight_);
    }
    
    void setThreshold(float t) { thre_ = t; }
    
    bool detect(MotionTracker& tracker)
    {
        Point2f pos = tracker.pos();
        
        if((velo_ = norm(pos-prevPos_)) > thre_) {
            motion_.push_back(pos);
        }else {
            //motion_.clear();
        }
        
        prevPos_ = pos;
        return true;
    }
    
    void clearMotion()
    {
        motion_.clear();
    }
    
    vector<Point2f> getMotion()
    {
        return motion_;
    }
    
    float getVelo()
    {
        return velo_;
    }
    
    void draw(cv::Mat img) {
        for(int i=0; i<(int)motion_.size()-1; ++i) {
            cv::line(img, motion_[i], motion_[i+1], Scalar::all(255));
        }
    }
    
    /*
    bool drawJudging() {
        if ((int)motion_.size() > 1) {
            distX_ = motion_[(int)motion_.size()-1].x - motion_[(int)motion_.size()-2].x;
            distY_ = motion_[(int)motion_.size()-1].y - motion_[(int)motion_.size()-2].y;
            v_ = sqrt(distX_*distX_ + distY_*distY_);
            //cout << "Speed = " << v << endl;
        }else{
            distX_ = 0.0;  distY_ = 0.0;  v_ = 0.0;
        }
        
        if (vmin_ > v_) {
            std::chrono::system_clock::time_point end1_ = chrono::system_clock::now();
            std::chrono::duration<double> sec1_ = end1_ - start1_;
            duration1_ = sec1_.count();
            cout << "Duration = " << duration1_ << endl;
        }else{
            start1_ = std::chrono::system_clock::now();
        }
        
        if (duration1_ > 2.0 && vmin_ < v_) {
            //rumble();
            cout << "DRAWING" << endl;
            drawJudge_ = true;
            track_ = motion_;
            start2_ = std::chrono::system_clock::now();
            
            if (vmax_ < v_) {
                vmax_ = v_;
                //cout << "Vmax = " << vmax << endl;
            }
        }
        
        if (vmax_*alpha_ > v_) {
            std::chrono::system_clock::time_point end2_ = std::chrono::system_clock::now();
            std::chrono::duration<double> sec2_ = end2_ - start2_;
            duration2_ = sec2_.count();
            cout << "Stop = " << duration2_ << endl;
        }else{
            start2_ = chrono::system_clock::now();
        }
        
        if (duration2_ > 1.0 && drawJudge_ == true) {
            cout << "FINISH" << endl;
            //rumble();
            drawJudge_ = false;
            duration2_ = 0.0;
        }
        
        return drawJudge_;
    }
     */
    
    int matching(cv::Mat drawImage_) {
        cvtColor(drawImage_, drawImage_, COLOR_RGB2GRAY);
        
        /*
        match2Cir_ = matchShapes(drawImage_, grayCircle_, 1, 0);
        match2Tri_ = matchShapes(drawImage_, grayTriangle_, 1, 0);
        match2Squa_ = matchShapes(drawImage_, graySquare_, 1, 0);
        
        if (match2Cir_ < match2Tri_ && match2Cir_ < match2Squa_) {
            return 1;
        }else if (match2Tri_ < match2Cir_ && match2Tri_ < match2Squa_) {
            return 2;
        }else if (match2Squa_ < match2Cir_ && match2Squa_ < match2Tri_) {
            return 3;
        }
         */
        
        double min_dist = DBL_MAX;
        int ishape=0;
        int i=0;
        for(auto& s : shapes_) {
            double dist = matchShapes(drawImage_, s, 1, 0);
            cout << dist <<  " ";
            if(dist < min_dist) {
                min_dist = dist;
                
                ishape = i+1;
            }
            i++;
        }
        
        return ishape;
    }
    
    void makeTeacher(int width_, int height_) {
        Mat im = Mat(Size(width_, height_), CV_8U, Scalar(0));
        circle(im, Point(width_/2,height_/2), 200, Scalar::all(255));
        shapes_.push_back(im);
        
        cv::Point pt[3];
        pt[0] = cv::Point(width_,height_);  pt[1] = cv::Point(100,380);  pt[2] = cv::Point(540,380);
        
        im = Mat(Size(width_, height_), CV_8U, Scalar(0));
        line(im, pt[0], pt[1], Scalar::all(255));
        line(im, pt[1], pt[2], Scalar::all(255));
        line(im, pt[2], pt[0], Scalar::all(255));
        shapes_.push_back(im);
        
        
        im = Mat(Size(width_, height_), CV_8U, Scalar(0));
        rectangle(im, Point(100,100), Point(540,380), Scalar::all(255));
        shapes_.push_back(im);
        
        /*
        for(auto& m : shapes_) {
            cvtColor(m,m,COLOR_RGB2GRAY);
        }
         */
    }
    
    /*
     void rumble() {
     PSMove* move = PSMoveTracker::returnPsm();
     psmove_set_rumble(PSMoveTracker::returnPsm(), 2);
     }
     */    
};

#endif
