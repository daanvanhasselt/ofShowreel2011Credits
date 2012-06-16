attribute vec3 noiseVec1;
attribute vec3 noiseVec2;

varying vec3 noiseVecVar1;
varying vec3 noiseVecVar2;

void main(){
    noiseVecVar1 = noiseVec1;
    noiseVecVar2 = noiseVec2;
    
    gl_Position = ftransform();
}