//
//  PEmotionTag.h
//  PMuseBookFes
//
//  Created by RengTsai on 2019/2/5.
//

#ifndef PEmotionTag_h
#define PEmotionTag_h

#define MEMOTION_TAG 8

#include "ofMain.h"

class PEmotionTag{
    
    ofPoint _pos;
    string _title;
    int _number;
    
    float _pos_num;
    
    FrameTimer _timer_in,_timer_out;
    
public:
    
    static ofTrueTypeFont FontEmotionTitle,FontEmotionNumber;
    
    PEmotionTag(){}
    PEmotionTag(float x,float y,string title,int num){
        reset(x,y,title,num);
    }
    void draw(float a_){
        
        ofPushStyle();
        ofSetColor(0,255*_timer_in.valEaseInOut()*(1-_timer_out.valEaseInOut())*a_);
        
        ofPushMatrix();
        ofTranslate(_pos);
        
        FontEmotionTitle.drawString(_title,0,0);
        FontEmotionNumber.drawString(ofToString(_number)+"%",_pos_num,0);
        
        ofPopMatrix();
        ofPopStyle();
        
    }
    void reset(float x,float y,string title,int num){
        _pos=ofPoint(x,y);
        _title=ofToUpper(title);
        _number=num;
        
        _pos_num=-1;//FontEmotionTitle.getStringBoundingBox(title,0,0).width+64;
        
        _timer_in=FrameTimer(500,ofRandom(200));
        _timer_out=FrameTimer(300);
        
    }
    void reset(){
        _timer_in.reset();
        _timer_out.reset();
    }
    void update(float dt_){
        _timer_in.update(dt_);
        _timer_out.update(dt_);
        
        if(_pos_num<0) _pos_num=FontEmotionTitle.getStringBoundingBox(_title,0,0).width+64;
        
    }
    void init(){
        _timer_in.restart();
    }
    void end(){
        _timer_out.restart();
    }
    bool finished(){
        return _timer_in.val()>=1;
    }
    float val(){
        return _number;
    }
};

class PEmotionTagGroup{
    
    PEmotionTag _tag[MEMOTION_TAG];
    
public:
    PEmotionTagGroup(){}
    void parseTag(ofxJSONElement emotion_){
        
        
        auto attr_=emotion_.getMemberNames();
        int num=attr_.size();
        
        float x=0;
        float y=ofRandom(100,200);
        
        for(int j=0;j<num;++j){
            string title_=attr_[j];
            int number_=round(emotion_[title_].asFloat()*100);
            ofLog()<<title_<<" "<<number_;
            
           _tag[j].reset(x,y,title_,number_);
            
            x=j<3?(ofRandom(30,150)):(ofGetWidth()/4.0*3.0+ofRandom(-50,100));
            y=min(y+ofRandom(150,250),ofGetHeight()-100.0f);
            if(j==3){
                y=ofRandom(100,200);
            }
        }
        
        
    }
    
    void draw(float a_){
        for(int i=0;i<MEMOTION_TAG;++i){
            _tag[i].draw(a_);
        }
    }
    
    void update(float dt_){
        for(int i=0;i<MEMOTION_TAG;++i){
            _tag[i].update(dt_);
        }
    }
    void reset(){
     for(int i=0;i<MEMOTION_TAG;++i) _tag[i].reset();
    }
    void init(){
        ofLogVerbose()<<"emotion tag init!!";
        for(int i=0;i<MEMOTION_TAG;++i) _tag[i].init();
    }
    void end(){
        for(int i=0;i<MEMOTION_TAG;++i) _tag[i].end();
    }
    bool fadeInFinished(){
        for(int i=0;i<MEMOTION_TAG;++i)
            if(!_tag[i].finished()) return false;
        return true;
    }
    ofxJSONElement getData(){
        ofxJSONElement json_;
        
        float arr_[8];
        for(int i=0;i<MEMOTION_TAG;++i){
            json_["tag"].append(_tag[i].val()/100.0);
        }
        return json_;
    }
    string getDataString(){
        
        return getData().getRawString();
    }
};


#endif /* PEmotionTag_h */
