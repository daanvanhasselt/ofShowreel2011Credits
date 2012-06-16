#include "testApp.h"

#define USE_KDTREE
//#define SAVE_FRAMES

#define FILE_NAME "authors"
#define NUM_PARTICLES 50000
#define TEXT_SIZE 55
#define ALPHA_FACTOR 0.0075
#define START_WIDTH 1600

//#define FILE_NAME "urls"
//#define NUM_PARTICLES 10000
//#define TEXT_SIZE 35
//#define ALPHA_FACTOR 0.035
//#define START_WIDTH 500

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_WARNING);
    
    glEnable(GL_POINT_SMOOTH);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    font.loadFont("Sathu.ttf", TEXT_SIZE, true, false, true);
    
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles.push_back(new Particle(ofPoint(ofRandom(START_WIDTH), 0)));
    }
    
#ifdef USE_KDTREE
    tree = new ofxKdTree<ofPoint>;    
#endif
    
    ofFile file = ofFile(FILE_NAME);
    buffer = file.readToBuffer();
    
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.internalformat = GL_RGB;
    
    scene.allocate(settings);
    noiseFbo.allocate(settings);
    abberationFbo.allocate(settings);
    
    abberationShader.load("shaders/abberation");
    noiseShader.load("shaders/noise");
    glitchShader.load("shaders/glitch");
    
    // GUI
    gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    int w = 300;
    int h = 40;
    
    gui->addWidgetDown(new ofxUISlider(w, h, -50, 0, strength, "STRENGTH"));
    gui->addWidgetDown(new ofxUISlider(w, h, 0, 1000, radius, "RADIUS"));
    gui->addWidgetDown(new ofxUISlider(w, h, 0.2, 1.0, damp, "DAMP"));
    gui->addWidgetDown(new ofxUISlider(w, h, 0.5, 10., speed, "SPEED"));
    
    gui->addWidgetEastOf(new ofxUISlider(w, h, 1, 100, fontPathSpacing, "FONTPATHSPACING"), "STRENGTH");
    gui->addWidgetEastOf(new ofxUISlider(w, h, 0, 20, simplifyLines, "SIMPLIFYLINES"), "RADIUS");
    gui->addWidgetEastOf(new ofxUISlider(w, h, 1, 500, minAttractionDistanceSq, "ATTRDIST"), "DAMP");
    gui->addWidgetEastOf(new ofxUISlider(w, h, 0., .2, abberation, "ABBERATION"), "SPEED");
    
    gui->addWidgetEastOf(new ofxUI2DPad(100, 100, ofPoint(1, 1000), ofPoint(1, 1000),noiseCoords1, "NOISE1"), "FONTPATHSPACING");
    gui->addWidgetEastOf(new ofxUI2DPad(100, 100, ofPoint(1, 1000), ofPoint(1, 1000), noiseCoords2, "NOISE2"), "NOISE1");
    gui->addWidgetEastOf(new ofxUISlider(w, h, 0., 1000., noiseTime, "NOISETIME"), "ATTRDIST");  
    gui->addWidgetEastOf(new ofxUIToggle(70, 70, autoAdvance, "AUTOADVANCE"), "ABBERATION");  
    
    gui->addWidgetEastOf(new ofxUISlider(w, h, 0., 100., glitchAmount, "GLITCHAMOUNT"), "NOISE2");  
    
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    gui->loadSettings("GUI/settings.xml");
    gui->setVisible(false);
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    if(name == "STRENGTH"){
        strength = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "RADIUS"){
        radius = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "DAMP"){
        damp = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "SPEED"){
        speed = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "FONTPATHSPACING"){
        fontPathSpacing = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "SIMPLIFYLINES"){
        simplifyLines = roundf(((ofxUISlider *) e.widget)->getScaledValue());
    }
    else if(name == "ATTRDIST"){
        minAttractionDistanceSq = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "ABBERATION"){
		abberation = ((ofxUISlider *) e.widget)->getScaledValue();
    }
    else if(name == "NOISE1"){
        noiseCoords1 = ((ofxUI2DPad *) e.widget)->getScaledValue();
    }
    else if(name == "NOISE2"){
        noiseCoords2 = ((ofxUI2DPad *) e.widget)->getScaledValue();
    }
    else if(name == "NOISETIME"){
		noiseTime = ((ofxUISlider *) e.widget)->getScaledValue();
    }   
    else if(name == "AUTOADVANCE"){
		autoAdvance = ((ofxUIToggle *) e.widget)->getValue();
    }
    else if(name == "GLITCHAMOUNT"){
		glitchAmount = ((ofxUISlider *) e.widget)->getScaledValue();
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    gui->saveSettings("GUI/settings.xml");
}

//--------------------------------------------------------------
void testApp::update(){
    for (int i = 0; i < attractors.size(); i++) {
        attractors[i]->strength = strength;
        attractors[i]->radius = radius;
        attractors[i]->minAttractionDistanceSq = minAttractionDistanceSq;
    }
    
    for (int i = 0; i < particles.size(); i++) {
        particles[i]->damp = damp;
        particles[i]->speed = speed;
        particles[i]->update();
    }    
    
    // generate noise texture (perlin noise on GPU)
    noiseFbo.begin();
    ofClear(0);
        noiseShader.begin();
            GLint vecLoc1 = noiseShader.getAttributeLocation("noiseVec1");
            GLint vecLoc2 = noiseShader.getAttributeLocation("noiseVec2");
    
            int x = 0;
            int y = 0;
            glBegin(GL_QUADS);            
                noiseShader.setAttribute3f(vecLoc1, x / noiseCoords1.x, y / noiseCoords1.y, ofGetFrameNum() / noiseTime);
                noiseShader.setAttribute3f(vecLoc2, x / noiseCoords2.x, y / noiseCoords2.y, ofGetFrameNum() / noiseTime);
                glVertex2f(x, y);
                
                x = noiseFbo.getWidth();
                noiseShader.setAttribute3f(vecLoc1, x / noiseCoords1.x, y / noiseCoords1.y, ofGetFrameNum() / noiseTime);
                noiseShader.setAttribute3f(vecLoc2, x / noiseCoords2.x, y / noiseCoords2.y, ofGetFrameNum() / noiseTime);
                glVertex2f(x, y);
                
                y = noiseFbo.getHeight();
                noiseShader.setAttribute3f(vecLoc1, x / noiseCoords1.x, y / noiseCoords1.y, ofGetFrameNum() / noiseTime);
                noiseShader.setAttribute3f(vecLoc2, x / noiseCoords2.x, y / noiseCoords2.y, ofGetFrameNum() / noiseTime);
                glVertex2f(x, y);
                
                x = 0;
                noiseShader.setAttribute3f(vecLoc1, x / noiseCoords1.x, y / noiseCoords1.y, ofGetFrameNum() / noiseTime);
                noiseShader.setAttribute3f(vecLoc2, x / noiseCoords2.x, y / noiseCoords2.y, ofGetFrameNum() / noiseTime);
                glVertex2f(x, y);
            glEnd();
        noiseShader.end();
    noiseFbo.end();
    
    if(autoAdvance && ofGetFrameNum() % 120 == 0)
        next();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    glPointSize(1);

    scene.begin();
    ofClear(0);
    
    ofPushMatrix();
    ofTranslate(20, ofGetHeight() / 2.0 + TEXT_SIZE / 2.0);
    
        // draw lines in between particles
        glBegin(GL_LINES);
        for (int i = 0; i < particles.size(); i += 1 + simplifyLines) {
            glColor4f(.9, .75, .6, .05);
            if(i >= particles.size())
                break;
            particles[i]->draw();
        }
        glEnd();
        
        // draw particles
        float alpha = ALPHA_FACTOR * word.length();
        glBegin(GL_POINTS);
        for (int i = 0; i < particles.size(); i++) {
            glColor4f(.9, .85, .7, alpha);
            particles[i]->draw();
        }
        glEnd();
    
    ofPopMatrix();
    
    scene.end();
    
    // chromatic abberation
    abberationFbo.begin();
    ofClear(0);
        abberationShader.begin();
        abberationShader.setUniformTexture("tex0", scene, 0);
        abberationShader.setUniformTexture("noiseTex", noiseFbo, 1);
        abberationShader.setUniform1f("amount", abberation);
            scene.draw(0, 0);
        abberationShader.end();
    abberationFbo.end();
    
    // generate hard noise texture (1 px wide) to use for the glitches
    unsigned char *pixels = new unsigned char[ofGetHeight() * 3];
    for(int i = 0; i < ofGetHeight() * 3; ){
        
        int numberOfRows = roundf(ofRandom(10, 100));   // set the value for a 'block' of rows, so the displacement happens in bigger chunks
        int randomValue = roundf(ofRandom(255));
        
        for (int j = 0; j < numberOfRows * 3; j+=3) {
            if (i + j >= ofGetHeight() * 3) {
                break;
            }
            
            pixels[i + j] = randomValue;
            pixels[i + j + 1] = 0;
            pixels[i + j + 2] = 0;
        }
        i += numberOfRows * 3;
    }
    ofImage img;
    img.setFromPixels(pixels, 1, ofGetHeight(), OF_IMAGE_COLOR);
    
    glitchShader.begin();
    glitchShader.setUniformTexture("tex0", abberationFbo, 0);
    glitchShader.setUniform1f("amount", 0.6 + glitchAmount * (ofRandom(100) < 1 ? 1.0 : 0.0));
    glitchShader.setUniform2f("dimensions", ofGetWidth(), ofGetHeight());
    glitchShader.setUniformTexture("noiseTex", img, 1);
        abberationFbo.draw(0, 0);
    glitchShader.end();

    
#ifdef SAVE_FRAMES
    ofSaveFrame();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'g')
        gui->toggleVisible();

    else if(key == ' ')
        next();
}

//--------------------------------------------------------------
void testApp::next(){
    word = nextWord();
    
    // the attractors are points on the outlines of the letters
    
    // clear all the attractors
    for (int i = 0; i < attractors.size(); i++) {
        delete attractors[i];
    }
    attractors.clear();
    
    vector <ofTTFCharacter> characters = font.getStringAsPoints(word);      // every letter as an ofTTFCharacter (== ofPath) in a vector
    for (int x = 0; x < characters.size(); x++) {
        ofTTFCharacter character = characters[x];
        
        vector <ofPolyline> polylines = character.getOutline();             // get the outlines
        vector <Attractor *> attractorsForChar;                             
        
        for (int i = 0; i < polylines.size(); i++) {
            ofPolyline polyline = polylines[i].getResampledBySpacing(fontPathSpacing);  // resample every outline for this character (so points are evenly distributed)
            vector<ofPoint> vertices = polyline.getVertices();                          // and get all the vertices of the path
            for (int j = 0; j < vertices.size(); j++) {
                if(!isnan(vertices[j].x)){                                              // sometimes, vertex.x, y and z are all nan.. bug?
                    Attractor *attr = new Attractor(vertices[j], strength, radius); 
                    attractors.push_back(attr);
                    attractorsForChar.push_back(attr);
                }
            }
        }
        
        // 'bind' attractors to particles
        vector <Attractor *> tmpAttractors = attractorsForChar;         // copy all attractors for this char into a temporary vector
        int numberOfParticlesPerCharacter = ceilf(particles.size() / (float)characters.size());
        for (int i = numberOfParticlesPerCharacter * x; i < numberOfParticlesPerCharacter * (x + 1); i++) {
            if(i >= particles.size())   // check for boundries, because we're rounding the index it might be too high
                break;
            
#ifdef USE_KDTREE            
            tree->clear();                              // update our kd-tree (it would be better if we could do tree->setVertex(...) but we can't)
            for (int j = 0; j < tmpAttractors.size(); j++) {
                tree->insert(tmpAttractors[j]->pos);    // so we clear and re-insert attractors
            }
            
            ofPoint *nearest = tree->getNearest(particles[i]->pos);
            for (int j = 0; j < tmpAttractors.size(); j++) {        // we have the nearest position, but we have to get the nearest attractor
                if(tmpAttractors[j]->pos == *nearest){
                    particles[i]->attractor = tmpAttractors[j];
                    tmpAttractors.erase(tmpAttractors.begin() + j);     // and we want to prevent duplicate-bindings
                    break;
                }
            }
            
#else            
            int random = floorf(ofRandom(tmpAttractors.size()));       // bind to a random attractor
            particles[i]->attractor = tmpAttractors[random];
            tmpAttractors.erase(tmpAttractors.begin() + random);            
#endif                       

            if(tmpAttractors.size() == 0)                   // we are out of attractors, but there may be particles left to bind to attractors so we want to refill the tmpAttractors
                tmpAttractors = attractorsForChar;
            
        }
    }
}

//--------------------------------------------------------------
string testApp::nextWord(){     // read the next line from the file
    if(buffer.isLastLine())
        buffer.resetLineReader();
    return buffer.getNextLine();
}