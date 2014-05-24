#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofFile configFile;
	configFile.open("config.json");
	auto configString = configFile.readToBuffer().getText();

	Json::Reader reader;
	Json::Value config;
	reader.parse(configString, config);

	this->width = 1024;
	this->height = 768;

	const auto & widthParam = config["projector"]["width"];
	const auto & heightParam = config["projector"]["height"];
	if (widthParam.asBool()) {
		this->width = widthParam.asInt();
	}
	if (heightParam.asBool()) {
		this->height = heightParam.asInt();
	}
	
	payload.init(this->width, this->height);
	decoder.init(payload);

	this->thresholdParameter.set("Threshold", this->decoder.getThreshold(), 0, 255);

	this->gui.init();
	this->gui.addInstructions();
	this->gui.add(this->decoder.getCameraInProjector(), "Camera in projector");
	this->gui.add(this->decoder.getProjectorInCamera(), "Projector in camera");
	this->gui.add(this->decoder.getMedian(), "Median image");
	auto medianInversePreviewPanel = this->gui.add(this->decoder.getDataSet().getMedianInverse(), "Median inverse image");

	auto widgets = this->gui.addScroll();
	widgets->add(ofxCvGui::Widgets::LiveValueHistory::make("Framerate", [] () {
		return ofGetFrameRate();
	}, true));
	auto thresholdSlider = shared_ptr<ofxCvGui::Widgets::Slider>(new ofxCvGui::Widgets::Slider(this->thresholdParameter));
	widgets->add(thresholdSlider);
	thresholdSlider->onValueChange += [this, medianInversePreviewPanel] (ofParameter<float> & value) {
		this->decoder.setThreshold(value);
		this->decoder.update();
	};
	auto resetButton = shared_ptr<ofxCvGui::Widgets::Button>(new ofxCvGui::Widgets::Button("[R]eset decoder", [this] () {
		this->keyPressed('r');
	}));
	widgets->add(resetButton);
	this->saveButton = shared_ptr<ofxCvGui::Widgets::Button>(new ofxCvGui::Widgets::Button("[S]ave output", [this] () {
		this->keyPressed('s');
	}));
	widgets->add(this->saveButton);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (this->decoder.hasData()) {
		this->saveButton->enable();
	} else {
		this->saveButton->disable();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 's':
			{
				decoder.saveDataSet();
				const auto basename = decoder.getDataSet().getFilename();
				auto medianInverse = decoder.getDataSet().getMedianInverse();
				auto median = decoder.getDataSet().getMedian();
				ofSaveImage(medianInverse, basename + "medianInverse.png", ofImageQualityType::OF_IMAGE_QUALITY_BEST);
				ofSaveImage(median, basename + "median.png", ofImageQualityType::OF_IMAGE_QUALITY_BEST);
				decoder.getCameraInProjector().saveImage(basename + "cameraInProjector.png");
			}
			break;
		case 'r':
			this->decoder.reset();
			break;
		default:
			break;
	}
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
	for(auto item : dragInfo.files) {
		if (ofFile(item).isDirectory()) {
			ofSetWindowTitle(item);
			auto dir = ofDirectory();
			dir.sort();
			dir.listDir(item);
			
			int index = 0;
			decoder.reset();
			ofImage loader;
			for (auto & file : dir.getFiles()) {
				this->processFile(file);
			}
			decoder.update();
		} else {
			ofFile file(item);
			this->processFile(file);
		}
	}
}

//--------------------------------------------------------------
void ofApp::processFile(ofFile & file) {
	cout << file.getAbsolutePath() << endl;
	auto extension = ofToLower(file.getExtension());
	cout << extension << endl;
	if (extension == "jpg" || extension == "jpeg" || extension == "png") {
		ofImage loader;
		ofLogNotice("ofApp::dragEvent") << "loading " << file.getFileName() << " #" << decoder.getFrame();
		loader.loadImage(file);
		ofLogNotice("ofApp::dragEvent") << "adding " << file.getFileName();
		decoder << loader;
	} else if (extension == "sl") {
		this->decoder.loadDataSet(file.getAbsolutePath());
	}
}
