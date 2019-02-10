#ifndef SCENE_POEM_H
#define SCENE_POEM_H


#include "SceneBase.h"

class ScenePoem:public SceneBase{

	FrameTimer _timer_poem;
    ofImage _img_text;
    FrameTimer _timer_hint;
public:
	ScenePoem(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=2;

        _timer_poem=FrameTimer(TIME_POEM);
        ofAddListener(_timer_poem.finish_event,this,&ScenePoem::onTimerFinish);
        
        _img_text.load("img/text-8.png");
        _timer_hint=FrameTimer(TIME_HINT);
        
		setup();
	}

	void drawLayer(int i){
		switch(i){
			case 0:
//                ofPushStyle();
//                ofSetColor(255,0,0);
//                ofFill();
//                    ofDrawBitmapString("2.Writing poem...",10,10);                                
//                ofPopStyle();
				break;
            case 1:
                _ptr_app->drawEmotionData();
                _ptr_app->drawPoem();
                
                if(_ptr_app->isSamplePoem()){
                    ofPushStyle();
                    ofSetColor(255,255*(_timer_hint.valEaseInOut()));
                        _img_text.draw(0,0);
                    ofPopStyle();
                }
                break;
		}	
	}
    void init(){
        SceneBase::init();
        _timer_poem.restart();
        _timer_hint.restart();
    }
    void update(float dt_){
        SceneBase::update(dt_);
        _timer_poem.update(dt_);
        _timer_hint.update(dt_);
    }
    void onTimerFinish(int &e){
        if(_ptr_app->isSamplePoem()) _ptr_app->saveImage();
        else _ptr_app->prepareStatus(ofApp::PFEEDBACK);
    }

};




#endif
