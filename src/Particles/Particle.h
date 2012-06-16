#pragma once

#include "ofMain.h"

class Attractor;
class Particle{
public:
    Particle(ofPoint pos);
    void update();
    void draw();
    void addForce(ofPoint force);

    ofPoint pos;
    ofPoint vel;
    ofPoint acc;
    float speed;
    float damp;
    
    Attractor *attractor;
};
