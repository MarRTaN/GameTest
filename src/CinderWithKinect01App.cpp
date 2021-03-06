#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include <math.h>
#include "cinder/ImageIo.h"
#include "Kinect.h"
#include "Bacteria.h"
#include "Stage.h"
#include "Player.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

int posXRange = 2; //Range -2 to 2
int posYRange = 2; //Range -2 to 2
int bacteriaBornTime = 100;
int bacteriaTimeCount = 0;
bool hit = false;


class CinderWithKinect01App : public AppBasic 
{
  public:
	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void setup();
	void update();
	void draw();
	void updateBacteria();
	void drawBacteria();
	void keyDown(KeyEvent event);
	void reset();
	void shutdown();

  private:
	vector<Vec3f>						mPoints;
	ci::CameraPersp						mCamera;

	// Kinect
	ci::Surface8u						mColorSurface;
	ci::Surface16u						mDepthSurface;
	int32_t								mDeviceCount;
	DeviceOptions						mDeviceOptions;
	KinectRef							mKinect;
	std::vector<Skeleton>				mSkeletons;
	int32_t								mTilt;
	int32_t								mTiltPrev;
	int32_t								mUserCount;
	void								startKinect();

	// Kinect callbacks
	int32_t								mCallbackDepthId;
	uint32_t							mCallbackSkeletonId;
	int32_t								mCallbackColorId;
	void								onColorData(ci::Surface8u surface, const DeviceOptions& deviceOptions);
	void								onDepthData(ci::Surface16u surface, const DeviceOptions& deviceOptions);
	void								onSkeletonData(std::vector<Skeleton> skeletons, const DeviceOptions& deviceOptions);

	//Bacteria
	vector<Bacteria>					bacterias;
	gl::Texture							bacTexture;

	//Stage
	Stage								stage;

	//Player
	Player								player;

	float								space = 40.0f;
	float								change = 2.0f;

};

// Kinect image size
const Vec2i	kKinectSize( 454, 750 );

void CinderWithKinect01App::prepareSettings( Settings *settings )
{
	settings->setWindowSize(454, 750);
	settings->setFrameRate(60.0f);
}

// Receives color data
void CinderWithKinect01App::onColorData(Surface8u surface, const DeviceOptions& deviceOptions)
{
	mColorSurface = surface;
}

// Receives depth data
void CinderWithKinect01App::onDepthData(Surface16u surface, const DeviceOptions& deviceOptions)
{
	mDepthSurface = surface;
}

// Receives skeleton data
void CinderWithKinect01App::onSkeletonData(vector<Skeleton> skeletons, const DeviceOptions& deviceOptions)
{
	mSkeletons = skeletons;
}

// Set up
void CinderWithKinect01App::setup()
{
	// Set up OpenGL

	// Start Kinect with isolated depth tracking only
	mKinect = Kinect::create();
	mKinect->start();
	

	// Set the skeleton smoothing to remove jitters. Better smoothing means
	// less jitters, but a slower response time.
	mKinect->setTransform(Kinect::TRANSFORM_SMOOTH);

	// Add callbacks
	mCallbackDepthId = mKinect->addDepthCallback(&CinderWithKinect01App::onDepthData, this);
	mCallbackSkeletonId = mKinect->addSkeletonTrackingCallback(&CinderWithKinect01App::onSkeletonData, this);
	mCallbackColorId = mKinect->addColorCallback(&CinderWithKinect01App::onColorData, this);

	// Set up camera
	mCamera.lookAt(Vec3f(0.0f, 0.0f, 1.0f), Vec3f::zero());
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
	player.Pos = Vec3f(0, -0.2f, 1.0f);
	stage.setup();

	//player setup
	player.setup();

	//bacteria setup
	bacTexture = gl::Texture(loadImage(loadAsset("obj/bacteria.png")));
}

void CinderWithKinect01App::update()
{
	// Device is capturing
	if ( mKinect->isCapturing() ) {
		mKinect->update();

		player.updatePlayer(mSkeletons);

		//check hand and update stage
		stage.updateStage(player.handRightPos, mCamera);

		//Update bacteria & camera
		if (stage.getStage() == 1){
			console() << "";
		}
		else if (stage.getStage() == 2){
			updateBacteria();
		}
		else{
			mCamera.lookAt(Vec3f(0.0f, 0.0f, 1.0f), Vec3f::zero());
		}
	} 
	else {
		// If Kinect initialization failed, try again every 90 frames
		if ( getElapsedFrames() % 90 == 0 ) {
			mKinect->start();
		}
	}

}

void CinderWithKinect01App::draw()
{
	// We're capturing
	if (mKinect->isCapturing()) {

		// Clear window
		gl::clear();
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

		if (stage.getStage() == 0){

			/// feed vid 
			if (mColorSurface) {
				Area srcArea(0, 0, mColorSurface.getWidth(), mColorSurface.getHeight());
				Rectf destRect(getWindowWidth() / 10, getWindowHeight() / 10, getWindowWidth() * 9 / 10, getWindowHeight() * 4 / 10);
				gl::draw(gl::Texture(mColorSurface), srcArea, destRect);
				Vec2f headInColor = Vec2f(mKinect->getSkeletonColorPos(player.headPos));
				if (space < 30.0f || space > 50.0f) change *= -1;
				space += change;
				headInColor.x = (getWindowWidth() / 10) + (headInColor.x / mColorSurface.getWidth() * getWindowWidth() * 8 / 10);
				headInColor.y = (getWindowHeight() / 10) + (headInColor.y / mColorSurface.getHeight() * getWindowHeight() * 3 / 10) - space;
				console() << destRect << endl;
				float scale = (3.0f - player.headPos.z) * 15;

				gl::pushMatrices();
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				Surface8u surface(loadImage(loadAsset("arrow.png")));
				gl::Texture arrowTexture = gl::Texture(surface);
				gl::draw(arrowTexture, Rectf(headInColor.x - scale*2, headInColor.y - scale, headInColor.x + scale*2, headInColor.y + scale));

				glDisable(GL_BLEND);
				gl::popMatrices();
			}
		}

		stage.drawStage();
	
		if (stage.getStage() == 2) {
			mCamera.lookAt(player.Pos, Vec3f(player.Pos.x, player.Pos.y, -3.0f));
			player.drawPlayer();
			drawBacteria();
			stage.drawTime();
		}
		else{
			reset();
		}
	}
}

void CinderWithKinect01App::updateBacteria(){
	if (bacteriaTimeCount > bacteriaBornTime){
		Bacteria newBac = Bacteria(bacTexture);
		bacterias.push_back(newBac);
		bacteriaTimeCount = 0;
		bacteriaBornTime = rand() % 20 + 30;
	}

	bacteriaTimeCount++;
	for (int i = 0; i < bacterias.size(); i++){
		float hitRangeX = 0.19f;
		float hitRangeY = 0.17f;
		Vec3f pl = player.Pos;
		Vec3f ba = bacterias[i].position;

		//Check Baacteria Hit
		if (abs(pl.x - ba.x) < hitRangeX && abs(pl.y - 0.1f - ba.y) < hitRangeY && ba.z > 0.05f && ba.z < 0.1f){
			stage.score++;
			bacterias[i].isHit = true;
			hit = false;
		}
		bacterias[i].updatePosition();
		if (bacterias[i].isOutOfBound){
			bacterias.erase(bacterias.cbegin());
		}
	}
}

void CinderWithKinect01App::drawBacteria(){
	for (int i = 0; i < bacterias.size(); i++){
		gl::setMatrices(mCamera);
		gl::pushMatrices();
		bacterias[i].draw();
		gl::popMatrices();
	}
	gl::setMatricesWindow(getWindowSize(), true);
}

void CinderWithKinect01App::keyDown(KeyEvent event)
{
	// Key on key...
	switch (event.getCode()) {
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		setFullScreen(!isFullScreen());
		break;
	case KeyEvent::KEY_a:
		hit = true;
		break;
	case KeyEvent::KEY_s:
		stage.nextStage();
		break;
	}
}

void CinderWithKinect01App::reset(){
	// Set up camera
	mCamera.lookAt(Vec3f(0.0f, 0.0f, 1.0f), Vec3f::zero());
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.01f, 1000.0f);

	player.Pos = Vec3f(0, -0.2f, 1.0f);
	bacterias.clear();
}


void CinderWithKinect01App::shutdown()
{
	// Stop input
	mKinect->removeCallback(mCallbackDepthId);
	mKinect->removeCallback(mCallbackSkeletonId);
	mKinect->removeCallback(mCallbackColorId);
	mKinect->stop();
	mPoints.clear();
}
CINDER_APP_BASIC( CinderWithKinect01App, RendererGl )
