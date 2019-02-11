#ifndef SCENE_FINISH_H
#define SCENE_FINISH_H


#include "SceneBase.h"

class SceneFinish:public SceneBase{
	
    FrameTimer _timer_finish,_timer_sound;
    ofImage _img_tint,_img_text,_img_logo;
    ofImage _img_text_2;
public:
	SceneFinish(ofApp* set_):SceneBase(set_){
		_mlayer=4;
        _order_scene=4;
        
        _timer_finish=FrameTimer(TIME_FINISH);
        ofAddListener(_timer_finish.finish_event,this,&SceneFinish::onTimerFinish);

        _img_tint.load("img/tint-2.png");
        _img_text.load("img/text-7.png");
        _img_logo.load("img/mmlab.png");
        
        _img_text_2.load("img/text-9.png");
        
		setup();
        
       // _timer_in[1].setDue(EASE_DUE*2);
       // _timer_in[3]=FrameTimer(EASE_DUE,EASE_OFFSET*3);
        
        _timer_sound=FrameTimer(EASE_OFFSET);
        ofAddListener(_timer_sound.finish_event,this,&SceneFinish::onSceneInFinish);
        
	}

	void drawLayer(int i){
        
        float a_=(_status==End)?getLayerAlpha(0):1;
        
		switch(i){
			case 0:
				//ofDrawBitmapString("4.Poem archived at page xxx!",10,10);
                _ptr_app->drawEmotionData(a_);
                _ptr_app->drawPoem(a_);
                break;
			case 1:
                
                if(_status==SceneBase::End){
                    ofPushStyle();
                    ofSetColor(255);
                        _img_tint.draw(0,0);
                    ofPopStyle();
                }else{
                    _img_tint.draw(0,0);
                }
               
                //ofDrawBitmapString("Go to muse.com.tw",10,20);
				break;
            case 2:
                if(_ptr_app->isSamplePoem()) _img_text_2.draw(0,0);
                else{
                    _img_text.draw(0,0);
                    if(_ptr_app->getUserPage()>0){
                        ofPushStyle();
                        ofSetColor(255,104,62,255*getLayerAlpha(2));
                        
                        //string str_=ofToString(_ptr_app->getUserPage(),3,'0');
                        
                        ofPushMatrix();
                        ofTranslate(143,840);
                        uint32_t t(_ptr_app->getUserPage());
                        PPoemText::FontPoem2.drawString(ofToString(t,3,'0'),0,0);
                        ofPopMatrix();
                        
                        ofPopStyle();
                    }
                }
                break;
            case 3:
                _img_logo.draw(0,0);
                break;
		}
	}
     void init(){
        SceneBase::init();
        _timer_finish.restart();
         _timer_sound.restart();
    }
    void update(float dt_){
        SceneBase::update(dt_);
        _timer_finish.update(dt_);
        _timer_sound.update(dt_);
    }
    void onTimerFinish(int &e){
        _ptr_app->prepareStatus(ofApp::PSLEEP);
    }
    void onSceneInFinish(int &e){
        _ptr_app->_sound_finish.play();
    }
};




#endif
