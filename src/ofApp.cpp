#include "ofApp.h"
#include "SceneSleep.h"
#include "SceneDetect.h"
#include "ScenePoem.h"
#include "SceneFeedback.h"
#include "SceneFinish.h"


ofTrueTypeFont PEmotionTag::FontEmotionTitle;
ofTrueTypeFont PEmotionTag::FontEmotionNumber;
//ofxTrueTypeFontUC PPoemText::FontPoem;
ofSoundPlayer PPoemText::SoundGlitch[7];
float PPoemText::TextPadding=20;

ofxTrueTypeFontUL2 PPoemText::FontPoem2;


//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetVerticalSync(true);
    
    
    
////    if(ofIsGLProgrammableRenderer()){
////        _shader_glitch.load("shader/shadersGL3/shader");
////    }else{
    _shader_glitch.load("shadersGL3/glitch");
    _timer_shader_in=FrameTimer(300);
    _timer_shader_out=FrameTimer(300);
    
    _shader_blur.load("shadersGL3/shaderBlurX");
    _fbo_glitch.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
    _fbo_save.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
    
    
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
    
    _camera_paused=false;
}
void ofApp::setCameraPause(bool set_){
    _camera_paused=set_;
    if(set_){
        _timer_shader_in.restart();
        _timer_shader_out.reset();
        
        _sound_processing.play();
        
    }else{
        _timer_shader_in.reset();
        _timer_shader_out.restart();
    }
}
bool ofApp::faceFound(){
    return _finder.size()>0;
}
//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetBackgroundColor(0);
    
	int dt_=ofGetElapsedTimeMillis()-_millis_now;
	_millis_now+=dt_;

	_scene[_status]->update(dt_);
    //if(_status_pre!=PEMPTY)_scene[_status_pre]->update(dt_);
    
    _emotion_tag.update(dt_);
    _poem.update(dt_);

	if(!_camera_paused){
		_camera.update();
		//_camera.getPixelsRef().mirror(false,true);
        if(_status==PSLEEP && _camera.isFrameNew()){
			_finder.update(_camera);
		}		
	}
    /* for shader */
    _timer_shader_out.update(dt_);
    _timer_shader_in.update(dt_);
    float dd_=(_timer_shader_in.valEaseInOut()*(1.0-_timer_shader_out.valEaseInOut()));
    
    
	switch(_status){
	    case PDETECT:
            if(dd_>0 && dd_<1){
                if(ofRandom(10)<1) _shader_density=dd_+ofRandom(-.2,.2);
            }else if(dd_==1){
                if(ofRandom(30)<1) _shader_density=dd_+ofRandom(-.1,.1);
            }else _shader_density=dd_;
            break;
        case PFEEDBACK:
            if(dd_>0 && dd_<1){
                if(ofRandom(10)<1) _shader_density=dd_+ofRandom(-.2,.2);
            }else if(dd_==1){
                if(_poem.goFinished() && ofRandom(30)<1) _shader_density=dd_+ofRandom(-.1,.1);
            }else _shader_density=dd_;
            break;
        case PPOEM:
            if(_emotion_tag.fadeInFinished()){
                if(_poem._state!=PPoem::IN){
                    _poem.reset();
                    _poem.init();
                }
            }
            if(ofRandom(30)<1) _shader_density=dd_+ofRandom(-.1,.1);
            break;
	}
    
    

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    float w=ofGetWidth();
    
   
	//_camera.draw(0,0);
    drawShaderImage();
    //drawFaceFrame();
    


//    if(_in_transition && _status_pre!=PStatus::PEMPTY){
//        _scene[_status_pre]->draw();
//    }
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
			prepareStatus(PSLEEP);
			break;
        case 's':
            saveImage();
            break;
        case 't':
            sendPoemRequest();
            break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//bool trigger_=_scene[_status]->handleMouse(x,y);
    if(x<200 && y<200){
        prepareStatus(PSLEEP);
    }
}

void ofApp::prepareStatus(PStatus set_){
	
    if(set_==_status_next) return;
    
    ofLog()<<"prepare scene "<<set_;
	//_status_pre=_status;
	_status_next=set_;
	
	if(_status!=PEMPTY) _scene[_status]->end();
    _in_transition=true;
}
void ofApp::setStatus(PStatus set_){
	
    
    _scene[set_]->init();
	
	switch(set_){
		case PSLEEP:
			_rect_face.clear();
            
			//_json_face.clear();
            _emotion_tag.reset();
            _poem.clear();
        
            setCameraPause(false);
            _shader_density=0;
            
        
            break;
        case PDETECT:
            createUserID();
            break;
        case PPOEM:
            //_emotion_tag.init();
            _sound_processing.stop();
            break;
        case PFEEDBACK:
            _emotion_tag.end();
            _poem.goOut();
        
            setCameraPause(false);
            break;
        case PFINISH:
           // _emotion_tag.init();
           // _poem.goIn();
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
    
    PEmotionTag::FontEmotionNumber.loadFont("font/HelveticaNeue-Medium.ttf",28);
    PEmotionTag::FontEmotionTitle.loadFont("font/Helvetica-Neue-Light-Italic_22502.ttf",28);
    
    //PPoemText::FontPoem.loadFont("font/Pro_GB18030-DemiBold.ttf",48);
    PPoemText::FontPoem2.loadFont("font/Pro_GB18030-DemiBold.ttf",48);
    
    
    _sound_back.load("sound/back.wav");
    _sound_back.setLoop(true);
    _sound_back.play();
    
    _sound_processing.load("sound/processing.wav");
    _sound_processing.setLoop(true);
    
    _sound_finish.load("sound/finish.wav");
    _sound_count.load("sound/count-beep.wav");
    
    for(int i=0;i<7;++i)
        PPoemText::SoundGlitch[i].load("sound/glitch-"+ofToString(i+1)+".wav");
    
}

void ofApp::onSceneInFinish(int &e){
	ofLog()<<"scene "<<e<<" in finish!";	
	_in_transition=false;
    
    if(_status==PFINISH){
        saveRawData();
    }
}
void ofApp::onSceneOutFinish(int &e){
    
    
	ofLog()<<"scene "<<e<<" out finish!";
    setStatus(_status_next);
    _status=_status_next;
}

void ofApp::sendFaceRequest(){

    ofLog()<<">>>>>> Send Face Requeset...";
    
    setCameraPause(true);
    _emotion_tag.reset();
    
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
void ofApp::sendPoemRequest(){

    ofLog()<<">>>>>> Send Poem Requeset...";
    
    string emotion_=_emotion_tag.getData();
    string url_="http://muse.mmlab.com.tw:5000/generate/mood";
//    _http_utils.setTimeoutSeconds(20);
//    _http_utils.addUrl(url_);
    ofxHttpForm form_;
    form_.action=url_;
    form_.method=OFX_HTTP_POST;
    form_.name="Poem Request "+ofToString(ofGetElapsedTimeMillis());
    form_.contentType="application/json";
    form_.data=emotion_;
    
    _http_utils.addForm(form_);
}
void ofApp::urlResponse(ofxHttpResponse & resp_){
	
	if(resp_.status != 200){
        ofLog()<<"Requeset error: "<<resp_.reasonForStatus;
        prepareStatus(PSLEEP);
        return;
    }
    if(resp_.url.find("muse.mmlab.com.tw")!=-1){
        
        //parsePoem(resp_.responseBody);
        ofxJSONElement json_;
        json_.parse(resp_.responseBody);
        setUseSample(json_["sample"].asBool());
        
        if(_poem.parse(json_["text"].asString())){
            prepareStatus(PPOEM);
            _user_data["poem"].append(resp_.responseBody.getText());
            
        }else prepareStatus(PSLEEP);
        
    }else if(resp_.url.find("microsoft.com")!=-1){
        //ofLog()<<"receive: "<<resp_.responseBody;
        parseFaceData(resp_.responseBody);
        
        if(_status==PDETECT){
            sendPoemRequest();
        }
        int event=_status;
        ofNotifyEvent(_event_recieve_emotion,event);
        
        //else if(_status==PFEEDBACK) prepareStatus(PFINISH);
        
        
    }else if(resp_.url.find("mmlab.com.tw")!=-1){
        //ofLog()<<"receive: "<<resp_.responseBody;
        ofxJSONElement json_;
        json_.parse(resp_.responseBody);
        if(json_["result"]=="success"){
            _user_page=json_["page"].asInt();
            ofLog()<<"add to page: "<<_user_page;
            
            prepareStatus(PFINISH);
        }
        
        
    }
    
	

}

void ofApp::parseFaceData(string data_){
	
	//ofLog()<<"get face data: "<<data_;
    
    //_json_face.clear();
    
	ofxJSONElement json_;
	if(json_.parse(data_)){
		int len=json_.size();
		for(int i=0;i<1;++i){
			ofRectangle rect_(json_[i]["faceRectangle"]["left"].asInt(),
				json_[i]["faceRectangle"]["top"].asInt(),
				json_[i]["faceRectangle"]["width"].asInt(),
				json_[i]["faceRectangle"]["height"].asInt());
			_rect_face.push_back(rect_);

			//_json_face.push_back(json_[i]["faceAttributes"]);
            
            _emotion_tag.parseTag(json_[i]["faceAttributes"]["emotion"]);

		}
        _user_data["face"]=json_;
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
void ofApp::drawEmotionData(float a_){

   // if(_json_face.size()<1) return;

    _emotion_tag.draw(a_);
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

void ofApp::drawPoem(float a_){
    _poem.draw(a_);
    
}

void ofApp::drawShaderImage(){

    _fbo_glitch.begin();
    ofClear(0);
    _shader_glitch.begin();
   
    //_shader_glitch.setUniform1f("blurAmnt", ofMap(mouseX,0,ofGetWidth(),0,1));
    
    _shader_glitch.setUniform1f("blurAmnt", _shader_density);
    
    ofPushMatrix();
    ofTranslate(_fbo_glitch.getWidth(),0);
    ofScale(-_camera_scale.x,_camera_scale.y);
        _camera.draw(0,0);
    ofPopMatrix();
    
    _shader_glitch.end();
    _fbo_glitch.end();
    
    _fbo_save.begin();
    ofClear(0);
    _shader_blur.begin();
    _shader_blur.setUniform1f("blurAmnt",_shader_density*2.0);
    
    _fbo_glitch.draw(0,0);
    
    _shader_blur.end();
    _fbo_save.end();

    _fbo_save.draw(0,0);
}

void ofApp::saveImage(){
    
    string filename_="output/"+_user_id+".jpg";
    
    _fbo_save.begin();
//    ofSetBackgroundColor(0);
//    _shader_blur.begin();
//    _shader_blur.setUniform1f("blurAmnt",_shader_density*2.0);
//
//    _fbo_glitch.draw(0,0,_fbo_save.getWidth(),_fbo_save.getHeight());
//
//    _shader_blur.end();
    
    drawEmotionData();
    drawPoem();
    
    _fbo_save.end();
    
    
    ofPixels pixels;
    _fbo_save.readToPixels(pixels);
    ofImage tmp_;
    tmp_.setFromPixels(pixels.getPixels(),pixels.getWidth(),pixels.getHeight(),OF_IMAGE_COLOR);
    tmp_.save(filename_);
    
    uploadImage();
}
void ofApp::uploadImage(){
    string filename_="output/"+_user_id+".jpg";
    
    ofxHttpForm form_;
    form_.action="https://mmlab.com.tw/project/MuseBookFes/upload.php";
    form_.method=OFX_HTTP_POST;
    form_.name="Poem Upload "+ofToString(ofGetElapsedTimeMillis());
    form_.addFile("file",ofToDataPath(filename_));
    form_.addFormField("guid", _user_id);
    form_.addFormField("action","upload");
    
    _http_utils.addForm(form_);
    
}
void ofApp::createUserID(){
    _user_id="mm"+ofGetTimestampString();
    _user_data.clear();
}

void ofApp::saveRawData(){
    
    ofImage tmp_;
    tmp_.setFromPixels(_camera.getPixels().getData(),_camera.getWidth(),_camera.getHeight(),OF_IMAGE_COLOR);
    tmp_.mirror(false, true);
    tmp_.save("raw/"+_user_id+".jpg");
    
    // save poem & emotion tag
    _user_data["id"]=_user_id;
    _user_data["page"]=_user_page;
    
    _user_data.save("data/"+_user_id+".json",true);

  
}

int ofApp::getUserPage(){
    return _user_page;
}
bool ofApp::isSamplePoem(){
    return _use_sample;
}
void ofApp::setUseSample(bool set_){
    _use_sample=set_;
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


