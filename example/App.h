//
//  App.h
//  rge_sample
//
//  Created by Yuhei Akamine on 2014/10/28.
//  Copyright (c) 2014年 Yuhei Akamine. All rights reserved.
//

#ifndef __rge_sample__App__
#define __rge_sample__App__

#include <vector>
#include <SDL2/SDL.h>
#include "rge/RGE.h"
#include "Frame.h"
#include "OpenGLTexture.h"
#include "Controller.h"
#include "fairy.h"


class App
{
    std::vector<rge::TextureRef> textures_;
    rge::TextureRef liveTexture_;
    int selectedTexture_ = 0;
    float poseRatio_ = 0;
public:
    App();
    void init();
    void applyWindowSize(int w, int h);
    void update();
    void display();
    
    void mouseLeftPressed(int x, int y);
    void mouseLeftReleased(int x, int y);
    void mouseLeftDragged(int x, int y);
    void dummyData(float x, float y ,float rot);

    
private:
    void readTextures();
    rge::FrameRef robotbase;
    Fairy fairy_;
    Controller controller_;
    MotionDetector motDetector_;
    ObjDetect objDetector_;
    //void readTextures();
    
    //PSEyeCapture capture_;
    OpenGLTexture texture_;

};


#endif /* defined(__rge_sample__App__) */
