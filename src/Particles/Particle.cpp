#include "Particle.h"
#include "Attractor.h"

//--------------------------------------------------------------
Particle::Particle(ofPoint _pos){
    pos = _pos;
    speed = 1;
    damp = 0.93;
    attractor = NULL;
}

//--------------------------------------------------------------
void Particle::update(){
    if(attractor != NULL){
        attractor->attract(this);
    }
    vel += acc;
    vel *= damp;
    acc.set(0);
    pos += vel * speed;
}

//--------------------------------------------------------------
void Particle::draw(){       
    if(attractor != NULL)
        glVertex3fv(pos.getPtr());
}

//--------------------------------------------------------------
void Particle::addForce(ofPoint force){
    acc += force * speed;
}