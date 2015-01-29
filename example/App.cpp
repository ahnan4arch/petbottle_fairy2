//
//  App.cpp
//  rge_sample
//
//  Created by Yuhei Akamine on 2014/10/28.
//  Copyright (c) 2014年 Yuhei Akamine. All rights reserved.
//

#include "App.h"

#include <fstream>
using namespace std;

#include <rge/RGE.h>
#include <math.h>
#include "trackball.h"
#include "rge/RgeReader.h"


using namespace rge;

//#include "OpenGLTexture.h"

#include <opencv2/opencv.hpp>
using namespace cv;

static VideoCapture video;


App::App()
{
}

void App::readTextures()
{
    vector<string> tex_names = {
        "sampleModels/sabi.jpg",
        "sampleModels/tex2.jpg",
        "sampleModels/rh1.jpg"
    };
    
    for(auto& name : tex_names) {
        TextureRef tex = TextureRef(new ImageTexture);
        
        tex->createFromFile(name);
        
        textures_.push_back(tex);
    }
}

void App::init()
{
    //背景クリア色
    glClearColor(0.0, 0.0, 1.0, 1.0);
			 
    video.open(1);
    video.set(CAP_PROP_FRAME_WIDTH, 640);
    video.set(CAP_PROP_FRAME_HEIGHT, 480);

   // dummyData(0, 0, 0);
    
    objDetector_.setCapture(&video);
    
    controller_.init(&objDetector_,&motDetector_,&fairy_);
    RGE::getInstance()->init();
    
    RgeReader reader;
    reader.read("rge/sampleModels/youseimodel4.rge");
    //reader.read("rge/sampleModels/youseimodel4.rge");
    
    if(RGE::getInstance()->findFrame("Armature"))
        RGE::getInstance()->findFrame("Armature")->setBonesVisibility(false);
    
    ofstream os("/tmp/ipo.csv");
    os << RGE::getInstance()->rootFrame()->getActionsDump() << endl;
    
    
    RGE::getInstance()->rootFrame()->printChildren();
    
    RGE::getInstance()->setBackgroundColor(color3(0,0,1));
    //RGE::getInstance()->rootFrame()->stopAnimation();
    RGE::getInstance()->rootFrame()->anim()->setLooping(true, true);
    
    //LAYER_2に属するオブジェクトを描画しない
    RGE::getInstance()->setLayerVisibility(LAYER_2, false);
    
    readTextures();

    //妖精描画
    robotbase = RGE::getInstance()->findFrame("body");
    
    
    //テクスチャ
    liveTexture_ = TextureRef(new ImageTexture);
    liveTexture_->createFromFile("rge/sampleModels/pet.jpg");
    RGE::getInstance()->findMaterial("dress1")->setTexture(liveTexture_);
        
    
//    dummyData(0,0,0);
    
//    MaterialRef mat = MaterialRef(new GlslMaterial);
//    mat->create();
//    mat->setId("glsl");
//
//    RGE::getInstance()->registerMaterial(mat);
//    RGE::getInstance()->updateMaterial();
}

void App::applyWindowSize(int w, int h)
{
    /* トラックボールする範囲 */
    trackballRegion(w, h);
    
    /* ウィンドウ全体をビューポートにする */
    //	glViewport(0, 0, w, h);(
    RGE::getInstance()->setViewport(0, 0, w, h);
    
    //ウインドウのアスペクト比を設定
    RGE::getInstance()->setAspectRatio((double)w/h);
}


void App::update()
{
    RGE::getInstance()->update();
    
   // imshow("video", img);
   // waitKey(1);
    
    //RGE::getInstance()->findMesh("Cylinder.000")->clearCache();
    
    controller_.update();
    fairy_.update();
    

}

int sign(float f)
{
    return (f<0)?-1:1;
}

void App::display()
{

    
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    //Mat img;
    //video >> img;

    Mat tex = objDetector_.getTexture();
    if(!tex.empty()) {
        //tex = imread("rge/sampleModels/pet.jpg");
        liveTexture_->updateImage(tex.size().width, tex.size().height, GL_BGR, tex.ptr());
        imshow("tex", tex);
    }

    
    Mat img = objDetector_.getBackground();
    
    if(!img.empty()) {
        texture_.update(img, GL_RGB);//GL_BGR
        texture_.drawBackground();
    }

    /* モデルビュー変換行列の初期化 */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /* トラックボール処理による回転(デバッグ用) */
    trackballRotation("Camera");
    
    //RGE(グラフィックエンジン）による描画
    glDisable(GL_BLEND);
    
    if(RGE::getInstance()->findLight("Spot"))
        SpotLight::upcast(RGE::getInstance()->findLight("Spot"))->setCastShadow(true);
    
    
    rgeVector3 position;
    if(fairy_.isVisible()){
         position = fairy_.getPosition();
        double direction = fairy_.getDirection();
        //    robotbase->setTranslation((position)-vector3<float>(0,0,0)); //要調節
        robotbase->setTranslation((position/50)-vector3<float>(0,0,0)); //要調節
        robotbase->setRotation(0,0,direction+90);
        
        rgeVector2 scr_pos = robotbase->getScreenPos();
//        cout << pos.x << "," << pos.y << endl;
        
        Rect rect = objDetector_.getObjectRect();
        
        if(prevZ_ != 0 && sign(prevZ_) != sign(position.y) &&
           rect.contains(Point2f(scr_pos.x,scr_pos.y))) {
         
            fairy_.setVisible(false);
        }
        
        prevZ_ = position.y;
    }
    else {
        robotbase->setTranslation(rgeVector3(1000,1000,1000));
        prevZ_ = 0;
    }

    RGE::getInstance()->render();

    
    img = objDetector_.getForeground();
    
    if(!img.empty() && position.y > 0) {
        texture_.updateRGBA(img);//GL_BGR
        texture_.drawBackgroundWithAlpha();
    }


}

void App::mouseLeftPressed(int x, int y)
{
    trackballStart(x, y);
}

void App::mouseLeftReleased(int x, int y)
{
    trackballStop(x,y);
}

void App::mouseLeftDragged(int x, int y)
{
    trackballMotion(x, y);
}


//妖精を回転させる
void App::dummyData(float x, float y ,float rot)
{
    double radian;
    int r;
    float z;
    
    r = 300;     //円の半径
    std::vector<rge::rgeVector3> dummy_track;
    
    for(int i=0; i<1800;i++){
        radian = M_PI/180 * i;
        x = cos(radian) * r;
        y = sin(radian) * r;
        z = 0;
        //      z = sin(radian * 4);
        
        dummy_track.push_back(rge::rgeVector3(x,y,z));
    }
    
    fairy_.setTrack(dummy_track);
//    //妖精を動かす
//        robotbase->setTranslation(x,y,z);
//        robotbase->rotate(0,0,0);
    
    
}


