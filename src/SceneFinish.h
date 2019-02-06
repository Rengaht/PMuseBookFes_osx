#ifndef SCENE_FINISH_H
#define SCENE_FINISH_H


#include "SceneBase.h"

class SceneFinish:public SceneBase{
	
    FrameTimer _timer_finish;
    ofImage _img_tint,_img_text;
    
public:
	SceneFinish(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=4;
        
        _timer_finish=FrameTimer(TIME_FINISH);
        ofAddListener(_timer_finish.finish_event,this,&SceneFinish::onTimerFinish);

        _img_tint.loadImage("img/tint-2.png");
        _img_text.loadImage("img/text-7.png");
        
		setup(); 
	}

	void drawLayer(int i){
		switch(i){
			case 0:
				//ofDrawBitmapString("4.Poem archived at page xxx!",10,10);
                _ptr_app->drawEmotionData();
                _ptr_app->drawPoem();
                break;
			case 1:
                _img_tint.draw(0,0);
                _img_text.draw(0,0);
                //ofDrawBitmapString("Go to muse.com.tw",10,20);
				break;
		}	
	}
     void init(){
        SceneBase::init();
        _timer_finish.restart();
    }
    void update(float dt_){
        SceneBase::update(dt_);
        _timer_finish.update(dt_);
    }
    void onTimerFinish(int &e){
        _ptr_app->prepareStatus(ofApp::PSLEEP);
    }
};




#endif
