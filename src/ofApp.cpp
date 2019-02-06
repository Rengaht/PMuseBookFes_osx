#include "ofApp.h"
#include "SceneSleep.h"
#include "SceneDetect.h"
#include "ScenePoem.h"
#include "SceneFeedback.h"
#include "SceneFinish.h"


ofTrueTypeFont PEmotionTag::FontEmotionTitle;
ofTrueTypeFont PEmotionTag::FontEmotionNumber;
ofxTrueTypeFontUC PPoemText::FontPoem;
float PPoemText::TextPadding=20;

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetVerticalSync(true);
    
	
	setupCamera();
    
    loadScene();
    
	_status_pre=PEMPTY;
	_status=PSLEEP;
	_scene[_status]->init();

	ofAddListener(SceneBase::sceneInFinish,this,&ofApp::onSceneInFinish);
	ofAddListener(SceneBase::sceneOutFinish,this,&ofApp::onSceneOutFinish);

	ofAddListener(_http_utils.newResponseEvent,this,&ofApp::urlResponse);
	_http_utils.start();

	ofEnableSmoothing();
	ofSetCircleResolution(10);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    
}


void ofApp::setupCamera(){
	_camera.setup(CAM_WIDTH,CAM_HEIGHT);
	_finder.setup("haarcascade_frontalface_default.xml");
	_finder.setPreset(ObjectFinder::Fast);
    
    float scale_=max((float)ofGetWidth()/_camera.getWidth(),(float)ofGetHeight()/_camera.getHeight());
    _camera_scale=ofPoint(scale_,scale_);
}

//--------------------------------------------------------------
void ofApp::update(){
	int dt_=ofGetElapsedTimeMillis()-_millis_now;
	_millis_now+=dt_;

	_scene[_status]->update(dt_);
	if(_status_pre!=PEMPTY)_scene[_status_pre]->update(dt_);
    
    _emotion_tag.update(dt_);
    _poem.update(dt_);

	if(_status!=PPOEM && _status!=PFINISH){
		_camera.update();
		//_camera.getPixelsRef().mirror(false,true);
		if(_camera.isFrameNew()){
			_finder.update(_camera);
		}		
	}
	bool found_ = _finder.size()>0;
	switch(_status){
		case PSLEEP:
			if(found_ && ((SceneSleep*)_scene[_status])->hintFinished()) setStatus(PStatus::PDETECT);
			break;
        case PPOEM:
            if(_emotion_tag.fadeInFinished()){
                if(_poem._state!=PPoem::IN)_poem.init();
            }
            break;
	}
    
    

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    float w=ofGetWidth();
    
    ofPushMatrix();
    ofTranslate(w,0);
    ofScale(-_camera_scale.x,_camera_scale.y);

	_camera.draw(0,0);
    //drawFaceFrame();
    
    
    ofPopMatrix();


	if(_in_transition && _status_pre!=PStatus::PEMPTY){
		_scene[_status_pre]->draw();
	}
	_scene[_status]->draw();

#ifdef DRAW_DEBUG_INFO
	ofPushStyle();
	ofSetColor(255,0,0);
	ofDrawBitmapString("fps= "+ofToString(ofGetFrameRate()),ofGetWidth()-100,10);
	ofPopStyle();
#endif

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch(key){
		case 'a':
			sendFaceRequest();
			break;
		case 'q':
			setStatus(PSLEEP);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bool trigger_=_scene[_status]->handleMouse(x,y);
}

void ofApp::setStatus(PStatus set_){
	
	_status_pre=_status;
	_status=set_;
	
	if(_status_pre!=PEMPTY) _scene[_status_pre]->end();
	_scene[_status]->init();
	_in_transition=true;

	switch(set_){
		case PSLEEP:
			_rect_face.clear();
			_json_face.clear();
            _emotion_tag.reset();
            _poem.reset();
			break;
        case PPOEM:
            _emotion_tag.init();
            break;
        case PFEEDBACK:
            _emotion_tag.end();
            _poem.goOut();
            break;
        case PFINISH:
            _emotion_tag.init();
            _poem.goIn();
            break;
	}

}


void ofApp::loadScene(){
	_scene[0]=new SceneSleep(this);
	_scene[1]=new SceneDetect(this);
	_scene[2]=new ScenePoem(this);
	_scene[3]=new SceneFeed(this);
	_scene[4]=new SceneFinish(this);

    
    _img_frame.loadImage("img/frame.png");
    
    _img_number=new ofImage[5];
    for(int i=0;i<5;++i){
        _img_number[i]=ofImage("img/"+ofToString(i+1)+".png");
        //_img_number[i].loadImage("img/"+ofToString(i+1)+".png");
    }
    
    PEmotionTag::FontEmotionNumber.loadFont("font/HelveticaNeue-Medium.ttf",32);
    PEmotionTag::FontEmotionTitle.loadFont("font/Helvetica-Neue-Light-Italic_22502.ttf",32);
    
    PPoemText::FontPoem.loadFont("font/Pro_GB18030-DemiBold.ttf",56);
}

void ofApp::onSceneInFinish(int &e){
	ofLog()<<"scene "<<e<<" in finish!";	
	_in_transition=false;

}
void ofApp::onSceneOutFinish(int &e){
	ofLog()<<"scene "<<e<<" out finish!";

}

void ofApp::sendFaceRequest(){

    ofLog()<<"Send Face Requeset...";
    
    string url_="https://eastasia.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceAttributes=age,gender,emotion,smile&returnFaceLandmarks=true";	
	// save image
	ofSaveImage(_camera.getPixels(),"tmp.jpg");
	ofBuffer data_=ofBufferFromFile("tmp.jpg",true);

	//_http_utils.postDataWithAuthKey(url_,data_,"Ocp-Apim-Subscription-Key", "022b0021b7994b659b75a7d9022f6746","application/octet-stream");
    
    ofxHttpForm form_;
    form_.action=url_;
    form_.method=OFX_HTTP_POST;
    form_.name="Face Request "+ofToString(ofGetElapsedTimeMillis());
    form_.addHeaderField("Ocp-Apim-Subscription-Key", "022b0021b7994b659b75a7d9022f6746");
    form_.contentType="application/octet-stream";
    form_.data=data_;
    
    _http_utils.addForm(form_);

}
void ofApp::sendPoemRequest(float mood_){

    ofLog()<<"Send Poem Requeset...";
    
    
    string url_="http://muse.mmlab.com.tw:5000/generate/mood/"+ofToString(mood_);
	_http_utils.setTimeoutSeconds(20);
    _http_utils.addUrl(url_);

}
void ofApp::urlResponse(ofxHttpResponse & resp_){
	
	if(resp_.status != 200){
        ofLog()<<"Requeset error: "<<resp_.reasonForStatus;
        return;
    }
    if(resp_.url.find("muse.mmlab.com.tw")!=-1){
        
        //parsePoem(resp_.responseBody);
    
        if(_poem.parse(resp_.responseBody)) setStatus(PPOEM);
        else setStatus(PSLEEP);
        
    }else if(resp_.url.find("microsoft.com")!=-1){
		
        parseFaceData(resp_.responseBody);
		if(_status==PDETECT) sendPoemRequest(0.5);
        else if(_status==PFEEDBACK) setStatus(PFINISH);
	}
    
	

}

void ofApp::parseFaceData(string data_){
	
	//ofLog()<<"get face data: "<<data_;

	ofxJSONElement json_;
	if(json_.parse(data_)){
		int len=json_.size();
		for(int i=0;i<1;++i){
			ofRectangle rect_(json_[i]["faceRectangle"]["left"].asInt(),
				json_[i]["faceRectangle"]["top"].asInt(),
				json_[i]["faceRectangle"]["width"].asInt(),
				json_[i]["faceRectangle"]["height"].asInt());
			_rect_face.push_back(rect_);

			_json_face.push_back(json_[i]["faceAttributes"]);
            
            _emotion_tag.parseTag(json_[i]["faceAttributes"]["emotion"]);

		}
	}

}


void ofApp::drawFaceFrame(){
    
    ofPushStyle();
    ofSetColor(255,0,0);
    ofNoFill();
    
    int len=_finder.size();
    for(int i=0;i<len;++i){
        ofDrawRectangle(_finder.getObject(i));

        auto rec_=_finder.getObject(i);
        
        ofSetColor(255);
        ofPushMatrix();
        ofTranslate(rec_.getPosition());
        _img_frame.draw(0,0,rec_.getWidth(),rec_.getHeight());
        ofPopMatrix();
    }
    ofPopStyle();

//    ofPushStyle();
//    ofSetColor(0,255,0);
//    ofNoFill();
//
//    float dy_=120;
//
//    len=_json_face.size();
//    for(int i=0;i<len;++i){
//        ofDrawRectangle(_rect_face[i]);
//
//    }
//    ofPopStyle();

}
void ofApp::drawEmotionData(){

    if(_json_face.size()<1) return;
   
		
//    float dy_=120;
//
//    int len=_json_face.size();
//    for(int i=0;i<len;++i){
//        ofDrawBitmapString("gender = "+ofToString(_json_face[i]["gender"].asString()),10,dy_+=20);
//        ofDrawBitmapString("age = "+ofToString(_json_face[i]["age"].asFloat()),10,dy_+=20);
//        ofDrawBitmapString("smile = "+ofToString(_json_face[i]["smile"].asFloat()),10,dy_+=20);
//
//        auto attr_=_json_face[i]["emotion"].getMemberNames();
//        for(auto& a: attr_){
//            ofDrawBitmapString(a+" = "+ofToString(_json_face[i]["emotion"][a].asFloat()),10,dy_+=20);
//        }
//    }

    _emotion_tag.draw();
}

void ofApp::drawNumber(int i){
//    ofPushStyle();
//    ofSetColor(255);
    
    if(i<0 || i>4) return;
    
    ofPushMatrix();
    ofTranslate(888,440);
    _img_number[i].draw(0,0);
    ofPopMatrix();
    
//    ofPopStyle();
}

void ofApp::drawPoem(){
    _poem.draw();
    
}


//string ofApp::ws2utf8(std::wstring &input){
//
////    try{
////        static std::locale loc("");
////        auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
////        return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(input);
////    }catch(exception& e){
////        cout<<e.what();
////    }
//    return "";
//}
//
//wstring ofApp::utf82ws(std::string &input){
//    try{
//        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
//        return utf8conv.from_bytes(input);
//    }catch(exception& e){
//        cout<<e.what();
//    }
//    return wstring();
//}
