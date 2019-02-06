#pragma once
#ifndef SCENE_FEEDBACK_H
#define SCENE_FEEDBACK_H


#include "SceneBase.h"

class SceneFeed:public SceneBase{

	FrameTimer _timer_hint,_timer_blink;
    ofImage _img_text;
    int _index_blink;
    bool _request_sent;
public:
	SceneFeed(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=3;
		
        _timer_hint=FrameTimer(TIME_HINT);
         ofAddListener(_timer_hint.finish_event,this,&SceneFeed::onTimerFinish);
        
        _timer_blink=FrameTimer(1000);
        
        _img_text.loadImage("img/text-6.png");
        
		setup();
	}
	void drawLayer(int i){
		switch(i){
			case 0:
                _ptr_app->drawPoem();
                _img_text.draw(0,0);
                _ptr_app->_img_frame.draw(688,293,543,543);
                break;
			case 1:
                ofPushStyle();
                
                ofSetColor(255,255*(1.0-_timer_blink.valEaseInOut()));
                _ptr_app->drawNumber(TIME_DETECT_COUNT-1-_index_blink);
                
               
                
                ofPopStyle();
				break;
		}
	}
	void init(){
		 SceneBase::init();
        _timer_hint.restart();
        
        _index_blink=0;
        _request_sent=false;
	}
	void update(float dt_){
		SceneBase::update(dt_);
		_timer_hint.update(dt_);
        
        
        _timer_blink.update(dt_);
        if(_timer_blink.val()==1 && _index_blink<TIME_DETECT_COUNT){
            _timer_blink.restart();
             _index_blink++;
            if(_index_blink>=TIME_DETECT_COUNT && !_request_sent){
                _ptr_app->sendFaceRequest();
                _request_sent=true;
            }
           
        }
	}
	void onTimerFinish(int &e){
        _timer_blink.restart();
	}
	

};


#endif
