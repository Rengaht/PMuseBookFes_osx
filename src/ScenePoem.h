#ifndef SCENE_POEM_H
#define SCENE_POEM_H


#include "SceneBase.h"

class ScenePoem:public SceneBase{

	FrameTimer _timer_poem;
   
public:
	ScenePoem(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=2;

        _timer_poem=FrameTimer(TIME_POEM);
        ofAddListener(_timer_poem.finish_event,this,&ScenePoem::onTimerFinish);
        
        
		setup();
	}

	void drawLayer(int i){
		switch(i){
			case 0:
                ofPushStyle();
                ofSetColor(255,0,0);
                ofFill();
				    ofDrawBitmapString("2.Writing poem...",10,10);	                			
                ofPopStyle();
				break;
            case 1:
                _ptr_app->drawPoem();
				_ptr_app->drawEmotionData();
                break;
		}	
	}
    void init(){
        SceneBase::init();
        _timer_poem.restart();
        
    }
    void update(float dt_){
        SceneBase::update(dt_);
        _timer_poem.update(dt_);
    }
    void onTimerFinish(int &e){
        _ptr_app->setStatus(ofApp::PFEEDBACK);
    }

};




#endif
