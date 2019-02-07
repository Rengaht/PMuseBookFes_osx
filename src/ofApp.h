#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxHttpUtils.h"
#include "ofxJSON.h"

#include "FrameTimer.h"
#include "SceneBase.h"
#include "PEmotionTag.h"
#include "PPoem.h"

#define CAM_WIDTH 1920
#define CAM_HEIGHT 1080

#define DRAW_DEBUG_INFO

#define TIME_DETECT_COUNT 3
#define TIME_POEM 5000
#define TIME_RECAP 2000
#define TIME_FINISH 8000

#define TIME_HINT 3000


using namespace ofxCv;
using namespace cv;


class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseReleased(int x, int y, int button);
		
		enum PStatus {PSLEEP,PDETECT,PPOEM,PFEEDBACK,PFINISH,PEMPTY};
		PStatus _status,_status_pre,_status_next;
		void prepareStatus(PStatus set_);

		SceneBase* _scene[5];

		void loadScene();
		bool _in_transition;
		void onSceneInFinish(int &e);
		void onSceneOutFinish(int &e);



		bool faceFound();
	
		void setupCamera();
        void setCameraPause(bool set_);
        bool _camera_paused;
    
        void sendFaceRequest();
        void sendPoemRequest(float mood_);
        
        ofxHttpUtils _http_utils;
		void urlResponse(ofxHttpResponse & response);

		
        void drawFaceFrame();
		void drawEmotionData(float a_=1.0);
        void drawPoem(float a_=1.0);
        void drawNumber(int i);
    
    
        ofImage _img_frame;
        ofImage *_img_number;
    
    
        PEmotionTagGroup _emotion_tag;
        PPoem _poem;
    
        ofEvent<int> _event_recieve_emotion;
    
    
        ofSoundPlayer _sound_back,_sound_processing;
        ofSoundPlayer _sound_finish,_sound_count;
        //ofxTrueTypeFontUC _font_poem;
    private:        
		int _millis_now;
        
        int _rid_face,_rid_poem;
        void parseFaceData(string data_);
    
        ofPoint _camera_scale;
        ofVideoGrabber _camera;
        ofxCv::ObjectFinder _finder;
    
        vector<ofRectangle> _rect_face;
        vector<ofxJSONElement> _json_face;
    
    
        void setStatus(PStatus set_);
    
        void drawShaderImage();
        FrameTimer _timer_shader_in,_timer_shader_out;
        float _shader_density;
    
        void saveImage();
    
        ofShader _shader_glitch;
        ofShader _shader_blur;
        ofFbo _fbo_glitch;
        ofFbo _fbo_save;
    
    
        //string ws2utf8(wstring& s);
        //wstring utf82ws(string& s);
    
    
};
