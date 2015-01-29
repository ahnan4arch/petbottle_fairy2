//
//  fairy.h
//  rge_sample
//
//  Created by 稲福也美 on 2015/01/28.
//  Copyright (c) 2015年 Yuhei Akamine. All rights reserved.
//

#ifndef rge_sample_fairy_h
#define rge_sample_fairy_h

//妖精の軌跡の表示
#include "Types.h"


class Fairy
{
    std::vector<rge::rgeVector3> track_;
    
    int step_;
    rge::rgeVector3 pos_;
    cv::Mat texture_;
    bool visible_;
    double direction_;
    
public:
    Fairy() {
        step_ = 0;
        visible_ = false;
        direction_ = 0;
    }
    
    rge::rgeVector3 getPosition() {
        return pos_;
    }
    
    double getDirection(){
        return direction_;
    }
    
    //モデルの表示確認
    bool isVisible(){
        return visible_;
    }
    
    void setTrack(const std::vector<rge::rgeVector3>& track) {
        track_ = track;
        step_ = 0;
        visible_ = true;
    }
    void setTexture(cv::Mat texture){
        texture_ = texture;
    }
    
    void setVisible(bool b) { visible_ = b; }
    
    void update() {
        if (visible_){
            rge::rgeVector3 pos = track_[step_];
            
            rge::rgeVector3 next_point = track_[step_+1];
            
            direction_ = atan2(next_point.y - pos.y, next_point.x - pos.x)/M_PI * 180;
            
            step_++;
            pos.z = pos.z + sin(M_PI/30 * step_)*50;
            //妖精の位置にゆらぎを加えるsin波
            pos_ = pos;
        }
        
        //移動し終えたか？
        if(step_+1 == track_.size() ) {
            
            //移動終了の処理
            //消える処理
            visible_ = false;
            return;
            
        }
    }
    
};

#endif
