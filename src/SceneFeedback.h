#pragma once
#ifndef SCENE_FEEDBACK_H
#define SCENE_FEEDBACK_H


#include "SceneBase.h"

class SceneFeed:public SceneBase{

	FrameTimer _timer_hint,_timer_blink;
    FrameTimer _timer_poem;
    
    ofImage _img_text;
    int _index_blink;
    bool _request_sent;
    
    enum FeedState {HINT,DETECT,EMOTION,POEM,READ};
    FeedState _state;
    
public:
	SceneFeed(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=3;
		
        _timer_hint=FrameTimer(TIME_HINT);
         ofAddListener(_timer_hint.finish_event,this,&SceneFeed::onTimerHintFinish);
    
        _timer_blink=FrameTimer(1000);
        
        _timer_poem=FrameTimer(TIME_RECAP);
        ofAddListener(_timer_poem.finish_event,this,&SceneFeed::onTimerPoemFinish);
        ofAddListener(_ptr_app->_event_recieve_emotion,this,&SceneFeed::onReceiveEmotion);
        
        _img_text.loadImage("img/text-6.png");
        
		setup();
	}
	void drawLayer(int i){
        
        
        float a1=getLayerAlpha(1);
        
		switch(i){
			case 0:
                _ptr_app->drawEmotionData();
                _ptr_app->drawPoem();
                
                
                break;
			case 1:
                if(_state==HINT){
                    _img_text.draw(0,0);
                    
                    ofSetColor(255,255*_timer_hint.valEaseInOut()*a1);
                    _ptr_app->_img_frame.draw(688,293,543,543);
                }else if(_state==DETECT){
                    _ptr_app->_img_frame.draw(688,293,543,543);
                    ofSetColor(255,255*(1.0-_timer_blink.valEaseInOut())*a1);
                    _ptr_app->drawNumber(TIME_DETECT_COUNT-1-_index_blink);
                }
                break;
		}
	}
	void init(){
		 SceneBase::init();
        _timer_hint.restart();
        _timer_poem.reset();
        
        _index_blink=0;
        //_request_sent=false;
        _state=FeedState::HINT;
	}
	void update(float dt_){
		SceneBase::update(dt_);
		_timer_hint.update(dt_);
        _timer_poem.update(dt_);
        
        _timer_blink.update(dt_);
        
        switch(_state){
            case DETECT:
                if(_timer_blink.val()==1 && _index_blink<TIME_DETECT_COUNT){
                    if(_index_blink<TIME_DETECT_COUNT-1) _ptr_app->_sound_count.play();
                    
                    _timer_blink.restart();
                     _index_blink++;
                    
                    
                    if(_index_blink>=TIME_DETECT_COUNT && !_request_sent){
                        _ptr_app->sendFaceRequest();
                        _state=EMOTION;
                    }
                   
                }
                break;
            case EMOTION:
                if(_ptr_app->_emotion_tag.fadeInFinished()){
                    _ptr_app->_poem.goIn();
                    _state=POEM;
                }
                break;
            case POEM:
                if(_ptr_app->_poem.goFinished()){
                    _ptr_app->_sound_processing.stop();
                    _timer_poem.restart();
                    _state=READ;
                }
                break;
        }
	}
	void onTimerHintFinish(int &e){
        _timer_blink.restart();
        _ptr_app->_sound_count.play();
        _state=DETECT;
	}
    void onTimerPoemFinish(int &e){
       // _ptr_app->prepareStatus(ofApp::PFINISH);
        _ptr_app->saveImage();
    }
    void onReceiveEmotion(int &e){
        if(e==_order_scene){
            _ptr_app->_emotion_tag.init();
//            _request_receive=true;
        }
    }
};


#endif
