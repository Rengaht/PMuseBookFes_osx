#ifndef SCENE_DETECT_H
#define SCENE_DETECT_H


#include "SceneBase.h"

class SceneDetect:public SceneBase{

	 FrameTimer _timer_hint;
    
	 ofImage _img_text[2];
	 FrameTimer _timer_blink;
	 int _index_blink;
     bool _request_sent;
    bool _request_receive;
public:
	SceneDetect(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=1;

		_img_text[0].loadImage("img/text-4.png");
		_img_text[1].loadImage("img/text-5.png");

		_timer_blink=FrameTimer(1000);
        _timer_hint=FrameTimer(TIME_HINT);

        _request_sent=false;
        
        ofAddListener(_timer_hint.finish_event,this,&SceneDetect::onTimerFinish);
        ofAddListener(_ptr_app->_event_recieve_emotion,this,&SceneDetect::onReceiveEmotion);
        setup();
	}

	void drawLayer(int i){
        
        float a_=getLayerAlpha(0);
        
		switch(i){
			case 0:
                ofPushStyle();
                /*ofSetColor(255,0,0);
                ofFill();
				    ofDrawBitmapString("1.Show your face...",10,10);				
                    ofDrawBitmapString(ofToString(floor(TIME_DETECT*(1-_timer_detect.val())/1000)),10,20);*/
                if(!_request_sent){
                    ofSetColor(255,255*(1.0-_timer_blink.valEaseInOut())*a_);
                    if(_index_blink==0) _img_text[0].draw(0,0);
                    else{
                        _img_text[1].draw(0,0);
                      
                        _ptr_app->drawNumber(TIME_DETECT_COUNT-_index_blink);
                      
                        ofPushStyle();
                        ofSetColor(255,255*a_);
                            _ptr_app->_img_frame.draw(688,293,543,543);
                        ofPopStyle();
                    }

                }
                ofPopStyle();
				break;
            case 1:
                _ptr_app->drawEmotionData();
                break;
		}	
	}
    void init(){
        SceneBase::init();
        _timer_hint.restart();
		
        _index_blink=0;
		_request_sent=false;
        _request_receive=false;
    }
    void update(float dt_){
        SceneBase::update(dt_);
        _timer_hint.update(dt_);
		
		_timer_blink.update(dt_);
		if(_timer_blink.val()==1){
			_timer_blink.restart();
			_index_blink++;

            if(_index_blink>TIME_DETECT_COUNT && !_request_sent){
                _ptr_app->sendFaceRequest();
                _request_sent=true;
            }
		}
    }
    void onTimerFinish(int &e){
        _timer_blink.restart();
    }
    void onReceiveEmotion(int &e){
        if( e==_order_scene && !_request_receive){
            _ptr_app->_emotion_tag.init();
            _request_receive=true;
        }
    }
};




#endif
