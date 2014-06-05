#include "ofApp.h"
//--------------------------------------------------------------
vector<double> loadDoubles(string& filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str(), ios::in | ios::binary);
	vector<double> result;
	if (file.is_open()) {
		while(!file.eof()) {
			double peek;
			file.read((char*)&peek, sizeof(double));
			result.push_back(peek);
		}
		result.pop_back();
	} else {
		ofLogError() << "Failed to load file " << filename;
	}
	return result;
}

//--------------------------------------------------------------
ofMatrix4x4 loadMatrix(string& filename) {
	if (filename == "") {
		return ofMatrix4x4();
	}

	auto buffer = loadDoubles(filename);
	if (buffer.size() >= 16) {
		ofMatrix4x4 m;
		auto floatBuffer = m.getPtr();
		for(int i=0; i<16; i++) {
			floatBuffer[i] = buffer[i];
		}
		
		ofMatrix4x4 reverseZ;
		reverseZ.scale(1.0f, 1.0f, -1.0f);
		return reverseZ * m * reverseZ;
	} else {
		return ofMatrix4x4();
	}
}

//--------------------------------------------------------------
void ofApp::setup(){
	this->distanceThreshold.set("Distance threshold", 0.05, 0.0, 10.0);
	this->pointSize.set("Point size", 1.0, 1.0, 10.0);

	this->payload.init(1024, 768);
	this->decoder.init(this->payload);

	this->camera = ofxRay::Camera(ofVec2f(1.0f), ofVec2f(), 1920, 1080);
	this->projector = ofxRay::Projector(1024, 768);

	this->gui.init();
	auto worldPanel = this->gui.addWorld("World");
	worldPanel->onDrawWorld += [this] (ofCamera &) {
		this->camera.draw();
		this->projector.draw();
		if (this->resultMesh.getNumVertices() > 0) {
			glPushAttrib(GL_POINT_SIZE);
			this->resultMesh.drawVertices();
			glPopAttrib();
		}
		this->camera.drawOnNearPlane(this->decoder.getProjectorInCamera());
		this->projector.drawOnNearPlane(this->decoder.getCameraInProjector());
	};

	auto widgetPanel = this->gui.addScroll();
	widgetPanel->add(Widgets::Title::make("Triangulate", Widgets::Title::Level::H1));

	widgetPanel->add(Widgets::Button::make("Load graycode", [this] () {
		this->decoder.loadDataSet();
	}));
	
	auto distanceThresholdSlider = Widgets::Slider::make(this->distanceThreshold);
	widgetPanel->add(distanceThresholdSlider);

	widgetPanel->add(Widgets::Slider::make(this->pointSize));

	widgetPanel->add(Widgets::Button::make("Load camera intrinsics", [this] () {
		auto response = ofSystemLoadDialog("Load camera intriniscs matrix");
		this->camera.setProjection(loadMatrix(response.filePath));
	}));
	widgetPanel->add(Widgets::Button::make("Load camera extrinsics", [this] () {
		auto response = ofSystemLoadDialog("Load camera extrinsics matrix");
		this->camera.setView(loadMatrix(response.filePath));
	}));
	widgetPanel->add(Widgets::Button::make("Load projector intrinsics", [this] () {
		auto response = ofSystemLoadDialog("Load projector intriniscs matrix");
		this->projector.setProjection(loadMatrix(response.filePath));
	}));
	widgetPanel->add(Widgets::Button::make("Load projector extrinsics", [this] () {
		auto response = ofSystemLoadDialog("Load projector extrinsics matrix");
		this->projector.setView(loadMatrix(response.filePath));
	}));

	this->triangulateButton = Widgets::Button::make("Triangulate", [this] () {
		this->triangulate();
	});
	widgetPanel->add(this->triangulateButton);

	this->saveOutputButton = Widgets::Button::make("Save output", [this] () {
		const auto vertices = this->resultMesh.getVertices();
		ofFloatImage outputImage;
		outputImage.allocate(this->projector.getWidth(), this->projector.getHeight(), OF_IMAGE_COLOR_ALPHA);
		outputImage.getPixelsRef().set(0.0f);
		for(int i=0; i<vertices.size(); i++) {
			const auto projectorPosition = this->resultMesh.getTexCoord(i);
			const unsigned int pixelIndex = projectorPosition.x + projectorPosition.y * this->projector.getWidth();
			if (pixelIndex >= outputImage.getPixelsRef().size()) {
				continue;
			}
			auto outputPixels = (ofVec4f*) outputImage.getPixels();
			auto & outputPixel = outputPixels[pixelIndex];
			outputPixel = vertices[i];
			outputPixel.w = 1.0f;
		}
		auto saveDialog = ofSystemSaveDialog("output.raw", "Save World map");
		if (saveDialog.bSuccess) {
			cout << "Saving world map to " << saveDialog.filePath;
			ofSaveImage(outputImage.getPixelsRef(), saveDialog.filePath);
		}
	});
	widgetPanel->add(this->saveOutputButton);

	distanceThresholdSlider->onValueChange += [this] (ofParameter<float> &) {
		if (this->triangulateButton->getEnabled()) {
			this->triangulate();
		}
	};
}

//--------------------------------------------------------------
void ofApp::triangulate() {
	ofxTriangulate::Triangulate(this->decoder.getDataSet(), this->camera, this->projector, this->resultMesh, (float) this->distanceThreshold);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (this->decoder.hasData() && !this->camera.getProjectionMatrix().isIdentity() && !this->projector.getProjectionMatrix().isIdentity()) {
		this->triangulateButton->enable();
	} else {
		this->triangulateButton->disable();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
