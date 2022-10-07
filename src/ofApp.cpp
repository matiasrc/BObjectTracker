#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    //ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    //----------------- XML -------------------
    loadSettings();
    
    //----------------- CAM -------------------
    //viewCam            = false;  // en el XML
    camWidth            = 640;
    camHeight           = 480;
    //deviceID            = 1;
    deviceName          = "NO DEVICE AVAILABLE";
       
    isOneDeviceAvailable =  false;
    
    setupCam(deviceID);

    //----------------- WARP -------------------
    warpON =  false;
    cualPunto = 0;
    
    moverPunto = false;
        
    mirroredImg.allocate(camWidth,camHeight);
    warpedImg.allocate(camWidth,camHeight);
    
    //----------------- OBJECT FINDER -------------------
    ofDirectory dataDirectory(ofToDataPath("", true));

    files = dataDirectory.getFiles();
    for(size_t i=0; i<files.size(); i++){
        
        if (files[i].exists()){
            
            ofLog(OF_LOG_NOTICE)<< "The file exists - now checking the type via file extension";
            string fileExtension = files[i].getExtension();
            
            //We only want images
            if (fileExtension == "xml") {
                fileNames.push_back(files[i].getFileName());
                ofLog(OF_LOG_NOTICE) << "Archivo: " << i <<" "<< files[i].getFileName();
            }
        }
    }
    
    cascadeFile = fileNames[fileID];
    finder.setup(cascadeFile);
    
    if(preset == 0 ){
        finder.setPreset(ObjectFinder::Fast);
    }else if(preset == 1){
        finder.setPreset(ObjectFinder::Accurate);
    }else if(preset == 2){
        finder.setPreset(ObjectFinder::Sensitive);
    }
    
    objectsNumber = 0;
    //----------------- GUI -------------------

    gui.setup();
    
    ImGui::GetIO().MouseDrawCursor = false;
            
    //----------------- OSC -------------------
    sender.setup(host, puerto);
}

void ofApp::update() {
	cam.update();
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
	if(cam.isFrameNew()) {
                
        mirroredImg.setFromPixels(cam.getPixels());
        mirroredImg.mirror(vMirror, hMirror);
        
        warpedImg = mirroredImg;
        warpedImg.warpPerspective(warp[0], warp[1], warp[2], warp[3]);
        
        //------- OBJETC FINDER ---------
        finder.update(warpedImg);
        
        objectsNumber = finder.size();
        
        for(int i=0; i<objectsNumber; i++){
            
            // ID
            int label = finder.getLabel(i);
            
            // velocity
            ofVec2f velocity = toOf(finder.getVelocity(i));
            // bounding rects
            ofRectangle r = finder.getObjectSmoothed(i);
            
            if(enviarDatos){
                vector<float> data;
                
                // 1
                data.push_back(static_cast<float>(label));         // 0
                
                // 2
                data.push_back(velocity.x / camWidth);             // 1
                data.push_back(velocity.y / camHeight);            // 2

                // 4
                data.push_back(r.x / camWidth);                    // 3
                data.push_back(r.y / camHeight);                   // 4
                data.push_back(r.width / camWidth);                // 5
                data.push_back(r.height / camHeight);              // 6
                
                enviarOsc(etiquetaMensajeDatos, data);
            }
        }
    }
    warpingReset();
}

void ofApp::draw() {
    
    ofSetColor(255);
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
   if(imageView == 0){
        mirroredImg.draw(0, 0, w, h);
    }
    else if(imageView == 1){
        warpedImg.draw(0, 0, w, h);
    }
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 255, 0);
    for(int i = 0; i < finder.size(); i++) {
        ofRectangle object = finder.getObject(i);
        float rx = object.x / camWidth * w;
        float ry = object.y / camHeight * h;
        float rw = object.width / camWidth * w;
        float rh = object.height / camHeight * h;
        ofDrawRectangle(rx, ry, rw, rh);
        ofSetColor(255, 0, 0);
        
        float cx = rx + rw/2;
        float cy = ry + rh/2;
        ofVec2f velocity = toOf(finder.getVelocity(i));
        float vx = velocity.x / camWidth * w;
        float vy = velocity.y / camHeight * h;
        ofDrawLine(cx, cy, cx + vx , cy + vy);
        ofDrawBitmapStringHighlight(ofToString(finder.getLabel(i)), object.x / camWidth * w, object.y / camHeight * h);
    }
    
    ofPopStyle();
    
    if(warpON){
        ofPushStyle();
        ofFill();
        ofPolyline pl;
        
        float cornerSize = 15;
        
        for(int i=0; i<4; i++){
            float x = warp[i].x / camWidth * w;
            float y = (warp[i].y / camHeight * h);
        
            pl.addVertex(x, y);
            
            corner[i].setFromCenter(x, y, cornerSize, cornerSize);
            
            ofFill();
            //ofDrawCircle(x, y, 5);
            if(i == cualPunto){
                ofSetColor(255, 0, 0);
            }else{
                ofSetColor(0, 255, 255);
            }
            ofDrawRectangle(corner[i]);
        }
        ofSetColor(0, 255, 255);
        pl.close();
        pl.draw();
        
        ofDrawBitmapStringHighlight("Deformación de entrada activada, presionad tecla w para salir", 5, ofGetHeight() - 25, ofColor(255,0, 0));

        ofPopStyle();
    }
    
    drawGui();
    
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()));
}

void ofApp::setupCam(int devID){
    
    wdevices = cam.listDevices();
    for(int i=0;i<static_cast<int>(wdevices.size());i++){
        if(wdevices[i].bAvailable){
            isOneDeviceAvailable = true;
            devicesVector.push_back(wdevices[i].deviceName);
            devicesID.push_back(i);

            for(size_t f=0;f<wdevices[i].formats.size();f++){
                ofLog(OF_LOG_NOTICE,"Capture Device format vailable: %ix%i",wdevices[i].formats.at(f).width,wdevices[i].formats.at(f).height);
            }
        }
    }
    
    cam.setDeviceID(devID);
    cam.setup(camWidth, camHeight);
}
void ofApp::resetCameraSettings(int devID){
    if(devID!=deviceID){
        ofLog(OF_LOG_NOTICE,"Changing Device to: %s",devicesVector[devID].c_str());

        deviceID = devID;
        deviceName = devicesVector[deviceID];
    }
    
    if(cam.isInitialized()){
        cam.close();
        //cam = new ofVideoGrabber();
        cam.setDeviceID(deviceID);
        cam.setup(camWidth, camHeight);
    }
    
}

//--------------------------------------------------------------
void ofApp::enviarOsc(string etiqueta, float valor){
    ofxOscMessage m;
    m.setAddress(etiqueta);
    m.addFloatArg(valor);
    sender.sendMessage(m, false);
}
//--------------------------------------------------------------
void ofApp::enviarOsc(string etiqueta, vector<float> valores){
    ofxOscMessage m;
    m.setAddress(etiqueta);
    for( int i=0; i<valores.size(); i++){
        m.addFloatArg(valores[i]);
    }
    sender.sendMessage(m, false);
    /*
     [0] --> object ID
     [1] --> object velocity X
     [2] --> object velocity Y
     [3] --> object bounding rect X
     [4] --> object bounding rect Y
     [5] --> object bounding rect Width
     [6] --> object bounding rect Height
    */
    
}
//--------------------------------------------------------------
void ofApp::loadSettings(){
    //-----------
    //the string is printed at the top of the app
    //to give the user some feedback
    xmlMessage = "loading mySettings.xml";

    //we load our settings file
    //if it doesn't exist we can still make one
    //by hitting the 's' key
    if( XML.loadFile("settings/mySettings.xml") ){
        xmlMessage = "mySettings.xml loaded!";
    }else{
        xmlMessage = "unable to load mySettings.xml check data/ folder";
    }

    //read the colors from XML
    //if the settings file doesn't exist we assigns default values (170, 190, 240)
    //red        = XML.getValue("BACKGROUND:COLOR:RED", 170);
    
    //---------------- OSC --------------------
    puerto = XML.getValue("OSC:PUERTO", 3333);
    host = XML.getValue("OSC:HOST", "127.0.0.1");
    etiquetaMensajeDatos = XML.getValue("OSC:ETIQUETA:DATOS", "/bobjecttracker/datos");
    enviarDatos = XML.getValue("OSC:ENVIARDATOS", true);
    
    //---------------- CAM --------------------
    deviceID = XML.getValue("CAM:DEVICEID", 0);
    hMirror = XML.getValue("CAM:HMIRROR", false);
    vMirror = XML.getValue("CAM:VMIRROR", false);
    
    paso = XML.getValue("CAM:WARPING:PASO", 5);
    
    warp[0].x = XML.getValue("CAM:WARPING:AX", 0);
    warp[0].y = XML.getValue("CAM:WARPING:Ay", 0);
    warp[1].x = XML.getValue("CAM:WARPING:BX", camWidth);
    warp[1].y = XML.getValue("CAM:WARPING:BY", 0);
    warp[2].x = XML.getValue("CAM:WARPING:CX", camWidth);
    warp[2].y = XML.getValue("CAM:WARPING:CY", camHeight);
    warp[3].x = XML.getValue("CAM:WARPING:DX", 0);
    warp[3].y = XML.getValue("CAM:WARPING:DY", camHeight);
    
    //---------------- OBJECT FINDER --------------------
    fileID = XML.getValue("OBJECTFINDER:FILEID", 0);
    preset = XML.getValue("OBJECTFINDER:PRESET", 0);

    
    ofLog(OF_LOG_NOTICE,xmlMessage);
}
//--------------------------------------------------------------
void ofApp::saveSettings(){
    //XML.setValue("BACKGROUND:COLOR:RED", red);
    XML.clear();

    //---------------- OSC --------------------
    XML.setValue("OSC:PUERTO", puerto);
    XML.setValue("OSC:HOST", host);
    XML.setValue("OSC:ETIQUETA:DATOS", etiquetaMensajeDatos);
    XML.setValue("OSC:ENVIARDATOS", enviarDatos);
    
    //---------------- CAM --------------------
    XML.setValue("CAM:DEVICEID", deviceID);
    XML.setValue("CAM:HMIRROR", hMirror);
    XML.setValue("CAM:VMIRROR", vMirror);
    
    XML.setValue("CAM:WARPING:PASO", paso);
    XML.setValue("CAM:WARPING:AX", warp[0].x);
    XML.setValue("CAM:WARPING:Ay", warp[0].y);
    XML.setValue("CAM:WARPING:BX", warp[1].x);
    XML.setValue("CAM:WARPING:BY", warp[1].y);
    XML.setValue("CAM:WARPING:CX", warp[2].x);
    XML.setValue("CAM:WARPING:CY", warp[2].y);
    XML.setValue("CAM:WARPING:DX", warp[3].x);
    XML.setValue("CAM:WARPING:DY", warp[3].y);
    
    //---------------- OBJECT FINDER --------------------
    XML.setValue("OBJECTFINDER:FILEID", fileID);
    XML.setValue("OBJECTFINDER:PRESET", preset);

        
    XML.saveFile("settings/mySettings.xml");
    xmlMessage ="settings saved to xml!";
    ofLog(OF_LOG_NOTICE,xmlMessage);
}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs& e){
    
    #if __APPLE__
    if(e.key == 's' && e.hasModifier(OF_KEY_COMMAND)){
        saveSettings();
    }
    #else
    if(e.key == 19 ){
        saveSettings();
    }
    
    #endif
    else if(e.key == '1'){
        cualPunto = 0;
    }
    else if(e.key == '2'){
        cualPunto = 1;
    }
    else if(e.key == '3'){
        cualPunto = 2;
    }
    else if(e.key == '4'){
        cualPunto = 3;
    }
    else if(e.key == 'w'){
        warpON = !warpON;
    }
    else if(e.key == OF_KEY_LEFT && warpON){
        warp[cualPunto].x -= paso;
        warp[cualPunto].x = ofClamp(warp[cualPunto].x, 0, camWidth);
    }
    else if(e.key == OF_KEY_RIGHT && warpON){
        warp[cualPunto].x += paso;
        warp[cualPunto].x = ofClamp(warp[cualPunto].x, 0, camWidth);
    }
    else if(e.key == OF_KEY_UP && warpON){
        warp[cualPunto].y -= paso;
        warp[cualPunto].y = ofClamp(warp[cualPunto].y, 0, camHeight);
    }
    else if(e.key == OF_KEY_DOWN && warpON){
        warp[cualPunto].y += paso;
        warp[cualPunto].y = ofClamp(warp[cualPunto].y, 0, camHeight);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs& e){

}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(warpON){
        for(int i=0; i<4; i++){
            if(corner[i].inside(x, y)){
                cualPunto = i;
                moverPunto = true;
                break;
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(moverPunto){
        if(x >= 0 && x<= ofGetWidth() && y>=0 && y <=ofGetHeight()){
            warp[cualPunto].x = ofMap(x, 0, ofGetWidth(), 0, camWidth);
            warp[cualPunto].y = ofMap(y, 0, ofGetHeight(), 0, camHeight);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    moverPunto = false;
}

//--------------------------------------------------------------
void ofApp::warpingReset(){
    
    if(resetWarping){
        A = ofPoint(0, 0);
        B = ofPoint(camWidth, 0);
        C = ofPoint(camWidth, camHeight);
        D = ofPoint(0, camHeight);
        
        warp[0] = A;
        warp[1] = B;
        warp[2] = C;
        warp[3] = D;
        resetWarping = false;
    }
}
