#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(ofKeyEventArgs& e);
    void keyReleased(ofKeyEventArgs& e);
    void setupCam(int devID);
    void resetCameraSettings(int devID);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);

	//----------------- VIEW -------------------
    int                     imageView;
    //----------------- CAM -------------------
    ofVideoGrabber          cam;
    ofxCvColorImage         mirroredImg;
    ofxCvColorImage         warpedImg;
    vector<ofVideoDevice>   wdevices;
    vector<string>          devicesVector;
    vector<int>             devicesID;
    float                   camWidth, camHeight;
    string                  deviceName;
    int                     deviceID;
    bool                    needReset;
    bool                    isOneDeviceAvailable;
    bool                    hMirror, vMirror;
    
    //----------------- WARP -------------------
    bool                    warpON;
    ofPoint                 warp[4];
    ofRectangle             corner[4];
    ofPoint                 A, B, C, D;
    int                     cualPunto;
    bool                    moverPunto;
    int                     paso;
    bool                    resetWarping;
    void                    warpingReset();
    
    //----------------- OBJECT FINDER -------------------
    ofxCv::ObjectFinder     finder;
    string                  cascadeFile;
    vector<string>          fileNames;
    vector<ofFile>          files;
    int                     fileID;
    int                     objectsNumber;
    int                     preset;
    //----------------- GUI -------------------
    void                    drawGui();
    ofxImGui::Gui           gui;
        
    //----------------- OSC -------------------
    ofxOscSender sender;
    int puerto;
    string host;
    string etiquetaMensajeDatos;
    bool enviarDatos;
    
    void enviarOsc(string etiqueda, float valor);
    void enviarOsc(string etiqueda, vector<float> valores);
    
    //----------------- XML -------------------
    ofxXmlSettings XML;
    void saveSettings();
    void loadSettings();
    string xmlMessage;    
};
