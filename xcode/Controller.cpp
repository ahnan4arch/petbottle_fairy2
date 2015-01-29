 //
//  Controler.cpp
//  level7
//
//  Created by Yume OOHAMA on 2015/01/20.
//  Copyright (c) 2015年 Yume OOHAMA. All rights reserved.
//

#include "Controller.h"
#include "Sound.h"

std::vector<rge::rgeVector3> convertRGEfloatToCVPointf(std::vector<cv::Point3f> cvp){
    vector<rge::rgeVector3> result;
    for(auto &c : cvp){
        result.push_back(rge::rgeVector3(c.x,c.y,c.z));
    }
    return result;
}


void Controller::update(){

    if (state_==OBJECT_DETECT) {
        objDetector_->update();
        
        if(objDetector_->isFound()) {
           // motDetector_->setTexture(objDetector_->getTexture());
           // motDetector_->setMoment(objDetector_->getCenterX());
            state_ = MOTION_DETECT;
        }
       /* waitKey(1000);
        if(objDetector_->detectObject()){
            objDetector_->makeTexture();
            motDetector_->setTexture(objDetector_->getTexture());
            motDetector_->setMoment(objDetector_->getCenterX());
        }
        */
        //state_ = MOTION_DETECT;
    }else if (state_==MOTION_DETECT){
        
        if(motDetector_->MotionDetecting()){
            play_appear();
            fairy_->setTexture(objDetector_->getTexture());
            fairy_->setTrack(convertRGEfloatToCVPointf(motDetector_->getTrack()));
            state_ = ANIMATION;
        }
    }else{ //state_ == ANIMATION
        waitKey(100);
        if(!fairy_->isVisible()){
            play_disappear();
            motDetector_->init();
            state_= MOTION_DETECT;
        }
    }
    
}

void Controller::draw(){
    // appから呼ぶ
    return;
}

void Controller::init(ObjectDetector *object,MotionDetector *motion, Fairy *fairy) {
    objDetector_ = object;
    motDetector_ = motion;
    fairy_ = fairy;
    
    motDetector_->init();
    objDetector_->init();
    audio_init();
}