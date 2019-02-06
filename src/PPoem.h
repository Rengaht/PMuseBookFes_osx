//
//  PPoem.h
//  PMuseBookFes
//
//  Created by RengTsai on 2019/2/5.
//

#ifndef PPoem_h
#define PPoem_h


#include "ofxTrueTypeFontUC.h"

class PPoemLine{
    
    ofPoint _pos_src[4];
    ofPoint _pos_dest[4];
    int _mline;
    
    
    enum PPState {STOP,IN,OUT};
    PPState _state;
    
    FrameTimer _timer_in,_timer_out;
    FrameTimer _timer_go;
    FrameTimer _timer_flow;
public:
    PPoemLine(int mline_, ofPoint* src_,ofPoint* dest_){
        _mline=mline_;
        _pos_src[4]=src_;
        _pos_dest[4]=dest_;
    }
    void draw(){
        float alpha_=255*_timer_in.valEaseInOut()*(1-_timer_out.valEaseInOut());
        ofPushStyle();
        ofSetColor(0,alpha_);
        ofNoFill();
        ofSetLineWidth(2)l
        
        ofPoint src[4];
        for(int i=0;i<4;++i){
            if(_state==IN)
                src[i]=lerpPoint(_pos_src[i],_pos_dest[i],1.0-_timer_go.valEaseInOut());
            else if(_state==OUT)
                src[i]=lerpPoint(_pos_src[i],_pos_dest[i],_timer_go.valEaseInOut());
            else src[i]=_pos_src[i];
        }
        
        float dl=_timer_flow.valEaseInOut();
        float dt=1.0/(float)_mline;
        
        for(float i=0;i<1;i+=dt){
            ofPoint pt1=lerpPoint(src[0],src[1],i);
            ofPoint pt2=lerpPoint(src[2],src[3],i);
            ofLine(pt1,lerpPoint(pt1,pt2,dl));
        }
        
        ofPopStlye();
    }
    void update(float dt_){
        _timer_in.update(dt_);
        _timer_out.update(dt_);
        _timer_go.update(dt_);
        _timer_flow.update(dt_);
    }
    void init(){
        _timer_in.restart();
        _timer_go.reset();
        
        _timer_flow.restart();
        _state=STOP;
    }
    void end(){
        _timer_out.restart();
    }
    void goOut(){
        _timer_go.restart();
        _state=OUT;
    }
    void goIn(){
        _timer_go.restart();
        _state=IN;
    }
    ofPoint lerpPoint(ofPoint pt1_,ofPoint pt2_,float p){
        return ofPoint(lerp(pt1_.x,pt2_.x,p),lerp(pt1_.y,pt2_.y,p));
    }
}

class PPoemText{
    
    //string _str;
    FrameTimer _timer_in,_timer_out,_timer_go;
    ofRectangle _bound;
    //ofPoint _pos_text;
    
    enum PPState {STOP,IN,OUT};
    PPState _state;
    float _scale;
    
    int _mline;
    list<ofRectangle> _rect;
    list<string> _str;
    list<ofPoint> _pos_text;
    
public:
    
    ofVec2f _pos_src,_pos_dest;
    float _ang_src,_ang_dest;
    
    static ofxTrueTypeFontUC FontPoem;
    static float TextPadding;
    
    PPoemText(ofPoint pos_,string str_,float scale_=1,bool mline=true){
        reset(pos_,str_,scale_,mline);
    }
    void reset(){
        _timer_in.reset();
        _timer_out.reset();
    }
    void reset(ofPoint pos_,string str_,float scale,bool mline){
        
        _pos_src=pos_;
        _pos_dest=pos_;
        _scale=scale;
        
        _ang_src=_ang_dest=0;
        
        //_str=str_;
        
        ofLog()<<str_;
        //_rect=FontPoem.getStringBoundingBox(_str,0,0);
        
        _timer_in=FrameTimer(1000);
        _timer_out=FrameTimer(500);
        _timer_go=FrameTimer(1000);
        
        // parse string
        if(mline){
           // auto text=ofSplitString(str_,"/");
            int mstr=str_.size();
            int per_line=4;//max((int)ceil((mstr/3)/3),3);
            for(int i=0;i<str_.size();i+=per_line*3){
                string t="";
                for(int j=i;j<min((int)mstr,i+9);++j) t+=str_[j];
                _str.push_back(t);
            }
            _mline=_str.size();
        }else{
            _mline=1;
            _str.push_back(str_);
        }
        
    }
    void draw(){
        ofPushStyle();
        float alpha_=255*_timer_in.valEaseInOut()*(1-_timer_out.valEaseInOut());
        
        ofPushMatrix();
        //ofTranslate(_pos);
        if(_state==OUT){
            ofTranslate(ofLerp(_pos_src.x,_pos_dest.x,_timer_go.valEaseInOut()),ofLerp(_pos_src.y,_pos_dest.y,_timer_go.valEaseInOut()));
            ofRotate(ofLerp(_ang_src,_ang_dest,_timer_go.valEaseInOut()));
        }else if(_state==STOP){
            ofTranslate(_pos_src);
            ofRotate(_ang_src);
            
        }else{
            ofTranslate(ofLerp(_pos_dest.x,_pos_src.x,_timer_go.valEaseInOut()),ofLerp(_pos_dest.y,_pos_src.y,_timer_go.valEaseInOut()));
            ofRotate(ofLerp(_ang_dest,_ang_src,_timer_go.valEaseInOut()));
        }
        
        if(_rect.size()<_mline) setupBound();
        
        auto itr=_rect.begin();
        auto its=_str.begin();
        auto itp=_pos_text.begin();
        float dy=0;
        for(int i=0;i<_mline;++i, ++itr,++its,++itp){
            ofSetColor(0,alpha_);
            ofDrawRectangle(*itr);
            
            ofPushMatrix();
            ofTranslate(TextPadding,TextPadding);
            ofScale(_scale,_scale);
            ofTranslate(itp->x,itp->y);
            
            ofSetColor(255,104,62,alpha_);
            FontPoem.drawString(*its,0,0);
            
            ofPopMatrix();
            
            ofTranslate(0,itr->height);
        }
        
        ofPopMatrix();
        
        ofPopStyle();
    }
    
    void update(float dt_){
        _timer_in.update(dt_);
        _timer_out.update(dt_);
        _timer_go.update(dt_);
    }
    void init(){
        _timer_in.restart();
        _timer_go.reset();
        _state=STOP;
    }
    void end(){
        _timer_out.restart();
    }
    
    void setPos(ofVec2f src_,float sang_,ofVec2f dest_,float dang_){
        _pos_src=src_;
        _pos_dest=dest_;
        
        _ang_src=sang_;
        _ang_dest=dang_;
    }
    void goOut(){
        _timer_go.restart();
        _state=OUT;
    }
    void goIn(){
        _timer_go.restart();
        _state=IN;
    }
    ofRectangle getRect(){
        if(_bound.width==0) setupBound();
        return _bound;
        
    }
    void setupBound(){
        
        _rect.clear();
        _pos_text.clear();
        
        if(_mline==1){
            auto r=FontPoem.getStringBoundingBox(*_str.begin(),0,0);
            _bound=ofRectangle(0,0,r.width*_scale+TextPadding*2,r.height*_scale+TextPadding*2);
            
            _rect.push_back(_bound);
            _pos_text.push_back(ofPoint(-r.x,-r.y));
            
        }else{
            float y=0;
            float w,h;
            for(auto& t:_str){
                auto r=FontPoem.getStringBoundingBox(t,0,0);
                auto bound=ofRectangle(0,0,r.width*_scale+TextPadding*2,r.height*_scale+TextPadding*2);
                _rect.push_back(bound);
                _pos_text.push_back(ofPoint(-r.x,-r.y));
                
                w=max(w,bound.width);
                h+=bound.height;
                y+=bound.height;
            }
            _bound=ofRectangle(0,0,w,h);
        }
        return _bound;
    }
};

class PPoem{

    vector<PPoemText> _poem;
    vector<PPoemLine> _line;
public:
    
    enum PoemState {IN,OUT,END,EMPTY};
    PoemState _state;
    
    PPoem(){
        _state=PoemState::EMPTY;
    }
    void draw(){
        
        for(auto& s:_poem) s.draw();
        for(auto& s:_line) s.draw();
        
    }
    
    void update(float dt_){
        for(auto& s:_poem) s.update(dt_);
        for(auto& s:_line) s.update(dt_);
    }
    void reset(){
        for(auto& s:_poem) s.reset();
        for(auto& s:_line) s.reset();
        _state=PoemState::EMPTY;
    }
    void init(){
        resetPos();
        
        for(auto& s:_poem) s.init();
        _state=PoemState::IN;
    }
    void end(){
        for(auto& s:_poem) s.end();
        _state=PoemState::END;
    }
    void goOut(){
        ofLog()<<"Poem go out!";
        for(auto&p:_poem) p.goOut();
        _state=PoemState::OUT;
    }
    void goIn(){
        ofLog()<<"Poem go in!";
        for(auto&p:_poem) p.goIn();
        _state=PoemState::IN;
    }
    
    void resetPos(){
       
        
        int m=_poem.size();
        int i=0;
        
        
//        // find max length
        float max_=0;
        int index_max_=0;
//        for(int i=1;i<m;++i)
//            if(_poem[i].getRect().width>max_){
//                index_max_=i;
//                max_=_poem[i].getRect().width;
//            }
//        float cent_wid=max_;

        index_max_=ceil((_poem.size())/2);
        float cent_wid=_poem[index_max_].getRect().width;
        
        int src_y=230;
        int left_x=ofGetWidth()/2-cent_wid/2;
        float right_x=ofGetWidth()/2+cent_wid/2;
        
        _poem[i].setPos(ofPoint(ofGetWidth()/2-_poem[i].getRect().width/2,src_y),0,ofPoint(0,0),0);
        src_y+=_poem[i].getRect().height;
        i++;
        
        float y_=ofRandom(_poem[0].getRect().height+_poem[1].getRect().width,ofGetHeight()-100);
        _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                        ofPoint(0,y_),-90);
        src_y+=_poem[i].getRect().height;
        i++;
        
        if(m>5){
            _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                            ofPoint(_poem[1].getRect().height,y_),-90);
            src_y+=_poem[i].getRect().height;
            i++;
        }
        _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                        ofPoint(0,ofGetHeight()-_poem[i].getRect().height),0);
        src_y+=_poem[i].getRect().height;
        i++;
        
        
        _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                        ofPoint(ofGetWidth()-_poem[i].getRect().width,0),0);
        src_y+=_poem[i].getRect().height;
        i++;
        
        if(m>6){
            _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                            ofPoint(ofGetWidth()-_poem[i].getRect().width,ofGetHeight()-_poem[i].getRect().height),0);
            src_y+=_poem[i].getRect().height;
            i++;
        }
        _poem[i].setPos(ofPoint((i<index_max_?left_x:right_x-_poem[i].getRect().width),src_y),0,
                        ofPoint(ofGetWidth(),ofGetHeight()-_poem[i].getRect().width),90);
        
    }
    
    bool parse(string data_){
        
        _poem.clear();
        
        if(data_.find("|")!=string::npos){
            auto text=ofSplitString(data_,"|");
            int m=text.size();
            for(int i=0;i<m;++i){
                //auto ws=utf82ws(s);
                //s=ws2utf8(ws);
                _poem.push_back(PPoemText(ofPoint(0,0),text[i],(i==0?1:0.7),i==0));
            }
            
            ofLog()<<"#line= "<<ofToString(_poem.size());
        }
        return _poem.size()>0;
    }
    
};



#endif /* PPoem_h */
