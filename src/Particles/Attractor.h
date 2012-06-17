#pragma once

#include "ofMain.h"
#include "Particle.h"

class Attractor{
public:
    Attractor(ofPoint pos, float strength, float radius);
    void attract(vector<Particle *>particles);
    void attract(Particle *particle);
    void draw();
    
    ofPoint pos;
    float strength;
    float radius;
    float radiusSquared;
    float minAttractionDistanceSq;
    float minSpeedSquared;
};
