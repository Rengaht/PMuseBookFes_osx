#ifndef SCENE_SLEEP_H
#define SCENE_SLEEP_H


#include "SceneBase.h"
#include "PMuseFlow.h"

#define FACE_DETECT_FRAME 4000

class SceneSleep:public SceneBase{
private:
    PMuseFlow _flow;
    ofImage _img_back;
    ofImage _img_text[3];
    
    FrameTimer _timer_text;
    int _index_text;
    float _index_face_start;
    
public:
	SceneSleep(ofApp* set_):SceneBase(set_){
		_mlayer=2;
        _order_scene=0;
		_img_back.loadImage("img/tint-1.png");
		for(int i=0;i<3;++i) _img_text[i].loadImage("img/text-"+ofToString(i+1)+".png");

		_timer_text=FrameTimer(TIME_HINT);

		setup();
	}

	void drawLayer(int i){
		switch(i){
			case 0:
                if(_status==SceneBase::Init){
                    ofPushStyle();
                    ofSetColor(255);
                        _img_back.draw(0,0,ofGetWidth(),ofGetHeight());
                    ofPopStyle();
                }else{
                    _img_back.draw(0,0,ofGetWidth(),ofGetHeight());
                }
                _flow.draw(getLayerAlpha(0));
				break;
			case 1:
				ofPushStyle();
				ofSetColor(255,255*_timer_text.val()*getLayerAlpha(1));
				_img_text[_index_text%3].draw(0,0);
				ofPopStyle();
				break;
		}	
	}
	void update(float dt_){
		SceneBase::update(dt_);
        
        _flow.update(dt_);
        
		_timer_text.update(dt_);
		if(_timer_text.val()==1){
			_timer_text.restart();
			++_index_text;
		}
        
        if(_ptr_app->faceFound()){
            if(_index_face_start<0){
                _index_face_start=0;
                ofLog()<<"---------- detect face ---------- ";
            }else{
                if(_index_face_start>=FACE_DETECT_FRAME) _ptr_app->prepareStatus(ofApp::PDETECT);
                _index_face_start+=dt_;
            }
        }else{
            if(_index_face_start>0)
                ofLog()<<"----------  no face ---------- "<<_index_face_start;
            _index_face_start=-1;
            
        }
	}
	void init(){
		SceneBase::init();
		_timer_text.restart();
		_index_text=0;
        _index_face_start=-1;
        
        _flow.restart();
	}

    
};




#endif
