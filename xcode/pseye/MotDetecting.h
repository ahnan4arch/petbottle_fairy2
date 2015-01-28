//
//  MotDetecting.h
//  pseye_
//
//  Created by e125719 on 2015/01/28.
//  Copyright (c) 2015年 Yuhei Akamine. All rights reserved.
//

#ifndef pseye__MotDetecting_h
#define pseye__MotDetecting_h

#include <iostream>
#include <unistd.h>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "psmove_config.h"
#include "PSEyeCapture.h"
#include "PSMoveTracker.h"
#include "MotionDetector.h"

#include "Timer.h"


class MotionDetector {
    int camWidth_, camHeight_;
    int objectWid_ = 0;
    int objectHei_ = 0;
    float objectMom_;
    int dtcShape_;
    bool judgeDraw_;
    vector<Point3f> tracking_;
    Mat text_;
    
    PSEyeCapture eye;
    PSMoveTracker tracker;
    MotionDetect detect;
    Timer timer;
    float time = 0;
    
    Mat imgDraw_;
    enum {
        WAITING,
        START_DRAWING,
        DRAWING,
        FINISH
    } state = WAITING;
public:
    MotionDetector() {
#if DO_PAIRING
        PSMoveTracker::pair();
#endif
        
        eye.open(320*2,240*2);
        setupEye(eye);
        
        camWidth_ = eye.size().width;
        camHeight_ = eye.size().height;
        
        tracker.init();
        tracker.setThreshold(50);
        tracker.calibrate(eye);
        
        objectWid_ = 0; objectHei_ = 0;
    }
    
    void init()
    {
        detect.init(640, 480);
    }
    
    void setupEye(PSEyeCapture& eye)
    {
        eye.driver()->setGain(10);
        eye.driver()->setExposure(20);
        eye.driver()->setAutoWhiteBalance(0);
        eye.driver()->setBlueBalance(120);
        eye.driver()->setRedBalance(120);
    }
    
    bool MotionDetecting() {
        timer.duration_sec();
        while(1) {
            Mat img_;
            
            eye >> img_;
            flip(img_, img_, 1);
            
            float delta_time = timer.duration_sec();
            if (!img_.empty()) {
                tracker.track(img_);
                detect.detect(tracker);
                
                detect.draw(img_);
                imshow("preview", img_);
                
                const float START_POSE_VELO_THRE = 20.f;
                const float START_POSE_TIME = 2.f;
                if(state == WAITING) {
                    cout << "WAITING" <<endl;
                    float v = detect.getVelo();
                    cout << "v=" << v << endl;
                    cout << "t=" << time<<endl;
                    
                    if(v < START_POSE_VELO_THRE) {
                        time += delta_time;
                    }
                    
                    if(time > START_POSE_TIME) {
                        state = START_DRAWING;
                        time = 0;
                        detect.clearMotion();
                    }
                }else if(state == START_DRAWING) {
                    cout << "WAIT TO START DRAWing" <<endl;

                    float v = detect.getVelo();

                    if(v > START_POSE_VELO_THRE) {
                        state = DRAWING;
                    }
                    
                }else if(state == DRAWING) {
                    cout << "Drawing" <<endl;
                    const float DRAW_STOP_VELO_THRE = 20.f;
                    const float DRAW_STOP_TIME = 2.f;

                    float v = detect.getVelo();
                    cout << v << endl;
                    if(v < DRAW_STOP_VELO_THRE) {
                        time += delta_time;
                    }
                    if(time > DRAW_STOP_TIME) {
                        state = FINISH;
                        
                        imgDraw_ = Mat::zeros(camHeight_, camWidth_,  CV_8UC3);

                        detect.draw(imgDraw_);
                        detect.clearMotion();
                    }
                }else {
                    //judgeDraw_ = detect.drawJudging();
                    dtcShape_ = detect.matching(imgDraw_);
                    
                    if (dtcShape_ == 1) {
                        cout << "Circle" << endl;
                    }else if (dtcShape_ == 2) {
                        cout << "Triangle" << endl;
                    }else if (dtcShape_ == 3) {
                        cout << "Square" << endl;
                    }
                    state = WAITING;
                    time = 0;
                    imshow("mot", imgDraw_);
                    waitKey();
                }
                /*
                 if (judgeDraw_) {
                 detect.draw(imgDraw_);
                 }
                 */
                

                //imshow("draw", imgDraw_);
            }
            
            waitKey(1);
        }
        
        return true;
    }
    
    std::vector<Point3f> getTrack() {
        //cout << "track" << track_ << endl;
        
        std::vector<Point3f> track1_(tracking_.size());
        
        for (int i=0; i<tracking_.size(); i++) {
            if (tracking_[i].x > camWidth_/2 - objectWid_/2 && tracking_[i].x < camWidth_/2 + objectWid_/2) {
                if (tracking_[i].y < camHeight_/2) {
                    track1_[i].x = -100.0;
                    track1_[i].y = -100.0;
                    track1_[i].z = 0.0;
                }
            }else{
                track1_[i].x = tracking_[i].x;
                track1_[i].y = tracking_[i].y;
                track1_[i].z = (objectHei_ / tracking_.size()) * (i+1);
            }
        }
        
        /*
         for (int i=0; i<track1_.size(); i++) {
         cv::circle(imgPoint_, Point(track1_[i].x, track1_[i].z), 3, Scalar::all(255), -1);
         imshow("Point", imgPoint_);
         waitKey(1);
         }
         
         cout << "track1_ = " << track1_ << endl;
         cout << "track_ = " << track_ << endl;
         */
        
        return track1_;
    }
    
    void setTexture(Mat texture_) {
        text_ = texture_;
        
        objectWid_ = text_.cols;
        objectHei_ = text_.rows;
    }
    
    void setMoment(float cx_) {
        objectMom_ = cx_;
    }
};

#endif
