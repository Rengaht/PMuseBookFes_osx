//
//  PPoem.h
//  PMuseBookFes
//
//  Created by RengTsai on 2019/2/5.
//

#ifndef PPoem_h
#define PPoem_h
#define LINE_MARGIN 10
#define LINE_WIDTH 5
#define MAX_POEM_LENGTH 800
#define POEM_TRANS_TIME 500

#define TIME_POEM_IN 1000
#define TIME_POEM_OUT 500

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
    PPoemLine(int mline_, ofPoint src_[],ofPoint dest_[]){
        _mline=mline_;
        for(int i=0;i<4;++i){
            _pos_src[i]=src_[i];
            _pos_dest[i]=dest_[i];
        }
        _timer_in=FrameTimer(TIME_POEM_IN,ofRandom(.1,.5)*TIME_POEM_IN);
        _timer_out=FrameTimer(TIME_POEM_OUT,ofRandom(.1,.5)*TIME_POEM_OUT);
        _timer_go=FrameTimer(POEM_TRANS_TIME,ofRandom(.1,.5)*POEM_TRANS_TIME);
        
        _timer_flow=FrameTimer(ofRandom(500,1000),ofRandom(500));
    }
    void draw(float a_){
        float alpha_=255*_timer_in.valEaseInOut()*(1-_timer_out.valEaseInOut())*a_;
        ofPushStyle();
        ofSetColor(0,alpha_);
        //ofNoFill();
        ofSetLineWidth(LINE_WIDTH);
        
        ofPoint src[4];
        for(int i=0;i<4;++i){
            if(_state==IN)
                src[i]=lerpPoint(_pos_src[i],_pos_dest[i],1.0-_timer_go.valEaseInOut());
            else if(_state==OUT)
                src[i]=lerpPoint(_pos_src[i],_pos_dest[i],_timer_go.valEaseInOut());
            else src[i]=_pos_src[i];
        }
        
        float dl=1;//_timer_flow.valEaseInOut();
        float dt=1.0/(float)_mline;
        
        for(float i=0;i<1;i+=dt){
            ofPoint pt1=lerpPoint(src[0],src[1],i);
            ofPoint pt2=lerpPoint(src[2],src[3],i);
            //ofLog()<<pt1<<"-"<<pt2;
            ofDrawLine(pt1,lerpPoint(pt1,pt2,dl));
        }
        
        ofPopStyle();
    }
    void update(float dt_){
        _timer_in.update(dt_);
        _timer_out.update(dt_);
        _timer_go.update(dt_);
        
        _timer_flow.update(dt_);
        if(_timer_flow.val()==1) _timer_flow.restart();
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
    bool goFinished(){
        return _timer_go.finish();
    }
    bool initFinished(){
        return _timer_in.finish();
    }
    ofPoint lerpPoint(ofPoint pt1_,ofPoint pt2_,float p){
        return ofVec2f(ofLerp(pt1_.x,pt2_.x,p),ofLerp(pt1_.y,pt2_.y,p));
    }
    void reset(){
        _timer_in.reset();
        _timer_out.reset();
    }
    void moveSrcX(float dx_){
        for(int i=0;i<4;++i) _pos_src[i].x+=dx_;
    }
};

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
        
        resetBound();
        
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
        
        _timer_in=FrameTimer(TIME_POEM_IN,ofRandom(.1,.5)*TIME_POEM_IN);
        _timer_out=FrameTimer(TIME_POEM_OUT,ofRandom(.1,.5)*TIME_POEM_OUT);
        _timer_go=FrameTimer(POEM_TRANS_TIME,ofRandom(.1,.5)*POEM_TRANS_TIME);
        
        // parse string
        if(mline){
           // auto text=ofSplitString(str_,"/");
            int mstr=str_.size();
            int per_line=3;//max((int)ceil((mstr/3)/3),3);
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
    void draw(float a_){
        
        if(_rect.size()!=_mline)
            return;
        
        ofPushStyle();
        float alpha_=255*_timer_in.valEaseInOut()*(1-_timer_out.valEaseInOut())*a_;
        
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
    bool goFinished(){
        return _timer_go.finish();
    }
    bool initFinished(){
        return _timer_in.finish();
    }
    ofRectangle getRect(){
        return _bound;
    }
    void setWidth(float wid){
        _bound.width=wid;
        for(auto& r:_rect){
            r.width=wid;
        }
    }
    void resetBound(){
        
        _rect.clear();
        _pos_text.clear();
        
        if(_mline==1){
            auto r=FontPoem.getStringBoundingBox(*_str.begin(),0,0);
            if(r.width>MAX_POEM_LENGTH){
                _scale*=(float)MAX_POEM_LENGTH/r.width;
                //r.width=MAX_POEM_LENGTH;
                ofLog()<<"reset scale= "<<_scale;
            }
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
//            for(auto& r:_rect){
//                r.setWidth(w);
//            }
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
    void draw(float a_){
        
        for(auto& s:_poem) s.draw(a_);
        for(auto& s:_line) s.draw(a_);
        
    }
    
    void update(float dt_){
        for(auto& s:_poem) s.update(dt_);
        for(auto& s:_line) s.update(dt_);
    }
    void reset(){
        for(auto& s:_poem) s.reset();
        for(auto& s:_line) s.reset();
     
        resetPos();
        resetLine();
        
    }
    void clear(){
        _poem.clear();
        _line.clear();
        _state=PoemState::EMPTY;
    }
    void init(){
        
        for(auto& s:_poem) s.init();
        for(auto& s:_line) s.init();
        
        _state=PoemState::IN;
    }
    void end(){
        for(auto& s:_poem) s.end();
        for(auto& s:_line) s.end();
        _state=PoemState::END;
    }
    void goOut(){
        ofLog()<<"Poem go out!";
        for(auto&p:_poem) p.goOut();
        for(auto&p:_line) p.goOut();
        _state=PoemState::OUT;
    }
    void goIn(){
        ofLog()<<"Poem go in!";
        for(auto&p:_poem) p.goIn();
        for(auto&p:_line) p.goIn();
        _state=PoemState::IN;
    }
    bool goFinished(){
        for(auto&p: _poem){
            if(!p.goFinished()) return false;
        }
        for(auto&p: _line){
            if(!p.goFinished()) return false;
        }
        return true;
    }
    bool initFinished(){
        for(auto&p: _poem){
            if(!p.initFinished()) return false;
        }
        for(auto&p: _line){
            if(!p.initFinished()) return false;
        }
        return true;
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

        index_max_=ceil((_poem.size())/2)+1;
        float cent_wid=_poem[index_max_].getRect().width;
        
        
        
        //adjust width
        if(m>5){
            float w1=max(_poem[2].getRect().width,_poem[3].getRect().width);
            _poem[2].setWidth(w1);
            _poem[3].setWidth(w1);
        }
        if(m>6){
            float w2=max(_poem[5].getRect().width,_poem[6].getRect().width);
            _poem[5].setWidth(w2);
            _poem[6].setWidth(w2);
        }
        
        
        int src_y=200;
        int left_x=ofGetWidth()/2-cent_wid/2;
        float right_x=ofGetWidth()/2+cent_wid/2;
        
        _poem[i].setPos(ofPoint(ofGetWidth()/2-_poem[i].getRect().width/2,src_y),0,ofPoint(0,0),0);
        src_y+=_poem[i].getRect().height;
        i++;
        
        float dd_=(m>5)?(_poem[2].getRect().width,_poem[1].getRect().width):_poem[1].getRect().width;
        float y_=ofRandom(_poem[0].getRect().height+dd_,ofGetHeight()-100);
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
    void resetLine(){
        _line.clear();
        
        int m=_poem.size();
        int mid_=ceil(m/2)+1;
        
        // L1: add bewteen p1, p2&p3
        ofRectangle b1(_poem[1]._pos_src.x,_poem[1]._pos_src.y,_poem[1].getRect().width,_poem[1].getRect().height);
        ofRectangle b2(_poem[2]._pos_src.x,_poem[2]._pos_src.y,_poem[2].getRect().width,_poem[2].getRect().height);
        if(m>5) b2.height+=_poem[3].getRect().height;
        
        ofPoint src1_[4],dest1_[4];
        int res1_;
        if(b1.width<b2.width){
            src1_[0]=b1.getTopRight();
            src1_[1]=b1.getBottomRight();
            src1_[2]=b2.getTopRight()+ofPoint(0,-b1.height);;
            src1_[3]=b2.getTopRight();
            res1_=b1.height/LINE_MARGIN;
        }else if(b1.width>b2.width){
            src1_[0]=b2.getTopRight();
            src1_[1]=b2.getBottomRight();
            src1_[2]=b1.getBottomRight();
            src1_[3]=b1.getBottomRight()+ofPoint(0,b2.height);
            res1_=b2.height/LINE_MARGIN;
        }
        _line.push_back(PPoemLine(res1_,src1_,dest1_));
        
        
        // L2: add bewteen p4, p5&p6
        ofRectangle b3(_poem[4]._pos_src.x,_poem[4]._pos_src.y,_poem[4].getRect().width,_poem[4].getRect().height);
        ofRectangle b4(_poem[5]._pos_src.x,_poem[5]._pos_src.y,_poem[5].getRect().width,_poem[5].getRect().height);
        if(m>6) b4.height+=_poem[6].getRect().height;
        
        ofPoint src2_[4],dest2_[4];
        int res2_;
        if(b3.width<b4.width){
            src2_[0]=b3.getTopLeft();
            src2_[1]=b3.getBottomLeft();
            src2_[2]=b4.getTopLeft();
            src2_[3]=b3.getBottomLeft();
            res2_=b3.height/LINE_MARGIN;
        }else if(b3.width>b4.width){
            src2_[0]=b3.getBottomLeft();
            src2_[1]=b4.getTopLeft();
            src2_[2]=b4.getBottomLeft();
            src2_[3]=b4.getTopLeft();
            res2_=b4.height/LINE_MARGIN;
        }
        
        dest2_[0]=ofPoint(0,_poem[1]._pos_dest.y);
        dest2_[1]=(m>5)?(_poem[2]._pos_dest+ofPoint(_poem[2].getRect().height,0)):(_poem[1]._pos_dest+ofPoint(_poem[1].getRect().height,0));
        dest2_[2]=ofPoint(0,(m>5)?_poem[3]._pos_dest.y:_poem[2]._pos_dest.y);
        dest2_[3]=ofPoint((m>5)?_poem[2]._pos_dest.x+_poem[2].getRect().height:_poem[1]._pos_dest.x+_poem[1].getRect().height,
                          (m>5)?_poem[3]._pos_dest.y:_poem[2]._pos_dest.y);
        
        _line.push_back(PPoemLine(res2_,src2_,dest2_));
        
        
        // add between p0,L1,L2
        ofRectangle b5(_poem[0]._pos_src.x,_poem[0]._pos_src.y,_poem[0].getRect().width,_poem[0].getRect().height);
        ofRectangle b6(_poem[1]._pos_src.x,_poem[1]._pos_src.y,max(b1.width,b2.width),b1.height+b2.height);
        ofRectangle b7(b3.x,b3.y,b3.width,b3.height+b4.height);
        
        //adjust b6
        float edge_=min(b5.getRight(),b7.getRight());
        if(b6.getRight()>edge_){
            float move_=edge_-ofRandom(60,200)-b6.getRight();
            ofLog()<<"move b6 by "<<move_;
            
            for(int i=1;i<mid_;++i) _poem[i]._pos_src.x+=move_;
            _line[0].moveSrcX(move_);
            b6.setX(b6.x+move_);
        }
    
        
        ofPoint src3_[4],dest3_[4];
        int res3_;
        if(b5.getRight()<b7.getRight()){
            src3_[0]=b6.getTopRight();
            src3_[1]=b5.getBottomRight();
            src3_[2]=b6.getBottomRight();
            src3_[3]=b5.getBottomRight()+ofPoint(0,b6.height);
            res3_=abs(src3_[1].x-src3_[0].x)/LINE_MARGIN;
            
        }else if(b5.getRight()>b7.getRight()){
            src3_[0]=b6.getTopRight();
            src3_[1]=b7.getTopRight()+ofPoint(0,-b6.height);
            src3_[2]=b6.getBottomRight();
            src3_[3]=b7.getTopRight();
            res3_=abs(src3_[1].x-src3_[0].x)/LINE_MARGIN;
        }
        
        dest3_[0]=ofPoint(ofGetWidth()-_poem[m-1].getRect().height,_poem[m-1]._pos_dest.y);
        dest3_[1]=ofPoint(ofGetWidth(),_poem[m-1]._pos_dest.y);
        dest3_[2]=ofPoint(ofGetWidth()-_poem[m-1].getRect().height,_poem[mid_].getRect().height);
        dest3_[3]=ofPoint(ofGetWidth(),_poem[mid_].getRect().height);
        
        _line.push_back(PPoemLine(res3_,src3_,dest3_));
        
        
        
        
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
