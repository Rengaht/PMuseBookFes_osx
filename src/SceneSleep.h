#ifndef SCENE_SLEEP_H
#define SCENE_SLEEP_H


#include "SceneBase.h"
#include "PMuseFlow.h"

class SceneSleep:public SceneBase{
	
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
				_img_back.draw(0,0,ofGetWidth(),ofGetHeight());
                /*ofPushStyle();
                ofSetColor(255,0,0);
                ofFill();
    				ofDrawBitmapString("0.Look at the camera...",10,10);				
                ofPopStyle();*/				
				_flow.draw();
				break;
			case 1:
				ofPushStyle();
				ofSetColor(255,255*_timer_text.val());
				_img_text[_index_text%3].draw(0,0);
				ofPopStyle();
				break;
		}	
	}
	void update(float dt_){
		SceneBase::update(dt_);
		_timer_text.update(dt_);
		if(_timer_text.val()==1){
			_timer_text.restart();
			++_index_text;
		}
	}
	void init(){
		SceneBase::init();
		_timer_text.restart();
		_index_text=0;
	}

    bool hintFinished(){
        return _index_text>=3;
    }
    
private:
	PMuseFlow _flow;
	ofImage _img_back;
	ofImage _img_text[3];

	FrameTimer _timer_text;
	int _index_text;
};




#endif
