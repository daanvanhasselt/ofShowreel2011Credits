#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "Attractor.h"
#include "ofxUI.h"
#include "ofxKdTree.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void next();
    
    ofTrueTypeFont font;
    string lastWord;
    string word;
    string nextWord();
    
    vector <Particle *> particles;
    vector <Attractor *> attractors;
    
    float strength;
    float radius;
    float damp;
    float speed;
    float fontPathSpacing;
    int simplifyLines;
    float minAttractionDistanceSq;
    float minSpeedSq;
    float speedIncrementPerLetter;
    float dampSpeed;
    float dampSpeedIncrement;
    
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);

    ofxKdTree<ofPoint> *tree;
    
    ofBuffer buffer;
    
    ofFbo scene;
    ofFbo noiseFbo;
    ofFbo abberationFbo;
    ofShader noiseShader;
    ofShader abberationShader;
    ofShader glitchShader;
    
    float abberation;
    ofPoint noiseCoords1;
    ofPoint noiseCoords2;
    float noiseTime;
    
    float glitchAmount;
    
    bool autoAdvance;
};
