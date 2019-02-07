#pragma once
#ifndef PMUSE_FLOW_H
#define PMUSE_FLOW_H

#include "ofxSvg.h"
#include "FrameTimer.h"

class PMuseFlow{
    
private:
    
    FrameTimer _timer;
    ofxSVG _svg_out;
    list<ofVbo> _vbo;
    ofImage _img_back;
    
    ofImage _img[10];
    int _index_draw,_index_last;
    
public:
	PMuseFlow(){
		_timer=FrameTimer(150);
		
//        _svg_out.load("img/logo-out.svg");
//        //_svg_in.load("img/logo-in.svg");
//
//        int num=_svg_out.getNumPath();
//        for(int i=0;i<num;++i){
//            ofVbo v_;
//            ofPath p_=_svg_out.getPathAt(i);
//            p_.setCurveResolution(10);
//            p_.setArcResolution(10);
//
//            auto poly_=p_.getOutline()[0];
//            poly_=poly_.getResampledBySpacing(2);
//           // poly_=poly_.getSmoothed(3);
//
//            v_.setVertexData(poly_.getVertices().data(),poly_.size(),GL_STATIC_DRAW);
//            ofLog()<<"poly size= "<<poly_.size();
//            _vbo.push_back(v_);
//        }
//
//        _img_back.load("img/logo-base.png");
        
        for(int i=0;i<10;++i){
            _img[i]=ofImage("img/logo/logo-Layer-"+ofToString(i+2)+".png");
        }
    
    }

	void draw(float alpha_){
		ofPushMatrix();
		ofTranslate(283,200);
		//ofNoFill();

		//_svg_out.draw();
//        _img_back.draw(0,0);
//
//        ofPushStyle();
//        ofSetColor(0);
//        ofSetLineWidth(5);
//
//        ofNoFill();
//
//
//        int end_=(int)(floor((float)ofGetFrameNum()/1.5))%5000;
//        for(auto& v:_vbo){
//            int m_=v.getNumVertices();
//            v.draw(GL_LINE_STRIP,0,min(end_%(m_+1000),m_));
//        }
//
//
//        ofPopStyle();
        
        
        for(int i=0;i<_index_last;++i){
            if(i!=_index_last-1 || i==0) _img[i].draw(0,0);
            else{
                ofPushStyle();
                if(_index_draw%18<9) ofSetColor(255,255*_timer.valEaseInOut()*alpha_);
                else ofSetColor(255,(255-255*_timer.valEaseInOut())*alpha_);
                
                _img[i].draw(0,0);
                
                ofPopStyle();
            }
        }
        
        
		ofPopMatrix();

	}
	void update(float dt_){
        _timer.update(dt_);
        if(_timer.finish()){
            _index_draw++;
            
            _index_last=(_index_draw%18>=9)?(9-(_index_draw%9)):((_index_draw%9)+1);
            //ofLog()<<_index_last;
            
            _timer.setDue(20*_index_last+100);
            _timer.restart();
            
        }
	}
	void restart(){
        _timer.restart();
        _index_last=1;
        _index_draw=0;
	}


};





#endif
