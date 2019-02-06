#ifndef SCENE_AUTH_H
#define SCENE_AUTH_H


#include "SceneBase.h"

class SceneAuth:public SceneBase{
	
public:
	SceneAuth(ofApp* set_):SceneBase(set_){
		
		_mlayer=1;
        _order_scene=3;

		_button.push_back(ofRectangle(10,20,50,50));
		_button.push_back(ofRectangle(60,20,50,50));

		setup();
	}

	void drawLayer(int i){
		switch(i){
			case 0:
				ofDrawBitmapString("3.Agree???",10,10);				
				break;
		}	

	}
    void buttonEvent(int index_){
        switch(index_){
            case 0:
                _ptr_app->setStatus(ofApp::PFINISH);
                break;
            case 1:
                _ptr_app->setStatus(ofApp::PSLEEP);
                break;
        }
    }

};




#endif