#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxRay.h"
#include "ofxGraycode.h"
#include "ofxRay.h"
#include "ofxTriangulate.h"

using namespace ofxCvGui;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void triangulate();

		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		Builder gui;
		ElementPtr triangulateButton;
		ElementPtr saveOutputButton;
		
		ofxGraycode::Decoder decoder;
		ofxGraycode::PayloadGraycode payload;
		ofxRay::Camera camera;
		ofxRay::Projector projector;
		ofMesh resultMesh;

		ofParameter<float> distanceThreshold;
		ofParameter<float> pointSize;
};
