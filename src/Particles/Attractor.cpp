#include "Attractor.h"

//--------------------------------------------------------------
Attractor::Attractor(ofPoint _pos, float _strength, float _radius){
    pos = _pos;
    strength = _strength;
    radius = _radius;
    radiusSquared = radius * radius;
}

//--------------------------------------------------------------
void Attractor::attract(vector<Particle *>particles){    
    for(int i = 0; i < particles.size(); i++){
        attract(particles[i]);
    }
}

//--------------------------------------------------------------
void Attractor::attract(Particle *particle){
    float distanceSquared = pos.distanceSquared(particle->pos);
    if(distanceSquared > minAttractionDistanceSq && particle->vel.lengthSquared() < minSpeedSquared){
        float s = distanceSquared / (radiusSquared / 3.) + 0.05;
        float f = pow(3.0 * (double)s, 0.3);
        f = f / radius;
        f /= 0.5;
        f *= strength;
        
        ofPoint delta = particle->pos - pos;
        particle->addForce(delta.normalized() * f);
    }
}

//--------------------------------------------------------------
void Attractor::draw(){
    ofFill();
    ofCircle(pos.x, pos.y, 2);
}