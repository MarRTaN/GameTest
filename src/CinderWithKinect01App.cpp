#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include <math.h>
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
	void updatePlayer();
	void drawPlayer();
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

	//Stage
	Stage								stage;

	//Player
	Player								player;

	//Window
	float								width = 0.3;
	float								height = 0.2;



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
}

void CinderWithKinect01App::update()
{
	// Device is capturing
	if ( mKinect->isCapturing() ) {
		mKinect->update();
		stage.updateStage(player.handRightPos, mCamera);
		updatePlayer();
		if (stage.getStage() == 1){
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

		stage.drawStage();
		if (stage.getStage() == 1) {
			drawPlayer();
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
		Bacteria newBac = Bacteria();
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
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();

	for (int i = 0; i < bacterias.size(); i++){
		bacterias[i].draw();
	}

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
}

void CinderWithKinect01App::updatePlayer(){

	player.gestureId = player.getGesture(mSkeletons);

	float distanceLeftX = abs(player.handLeftPos.x - player.centerPos.x);
	float distanceLeftY = abs(player.handLeftPos.y - player.centerPos.y);
	float distanceRightX = abs(player.handRightPos.x - player.centerPos.x);
	float distanceRightY = abs(player.handRightPos.y - player.centerPos.y);

	player.angle = atanf(distanceLeftY / distanceLeftX) + atanf(distanceRightY / distanceRightX);

	if (player.angle < 0.3f) { player.Acc = 0.0001f; }
	else if (0.3f < player.angle && player.angle < 0.7f){ player.Acc = 0.0003f; }
	else if (0.7f < player.angle && player.angle < 1.1f){ player.Acc = 0.0006f; }
	else if (1.1f < player.angle && player.angle < 1.5f){ player.Acc = 0.0009f; }
	else if (1.5f < player.angle && player.angle < 1.9f){ player.Acc = 0.00012f;  }
	else if (1.9f < player.angle) { player.Acc = 0.00015f; }

	/// UPDATE VELOCITY ///
	switch (player.gestureId) {
		/// TURN LEFT
		case 1: player.Vel.x -= player.Acc;
			if (player.Vel.y > 0.0f) player.Vel.y -= player.Acc;
			else  player.Vel.y += player.Acc;
			break;
		/// TURN RIGHT
		case 2: player.Vel.x += player.Acc;
			if (player.Vel.y > 0.0f) player.Vel.y -= player.Acc;
			else  player.Vel.y += player.Acc;
			break;
			/// UP LEFT
		case 3: player.Vel.x -= player.Acc;
			player.Vel.y += player.Acc;
			break;
			/// UP RIGHT
		case 4: player.Vel.x += player.Acc;
			player.Vel.y += player.Acc;
			break;
			/// UP STRAIGHT
		case 5: player.Vel.y += player.Acc;
			break;
			/// DOWN LEFT
		case 6: player.Vel.x -= player.Acc;
			player.Vel.y -= player.Acc;
			break;
			/// DOWN RIGHT
		case 7: player.Vel.x += player.Acc;
			player.Vel.y -= player.Acc;
			break;
			/// DOWN
		case 0: player.Vel.y -= player.Acc;
			if (player.Vel.x > 0.0f) player.Vel.x -= player.Acc;
			else player.Vel.x += player.Acc;
			break;
	}

	if (player.Vel.x > 0.005f)	player.Vel.x = 0.005f; 
	if (player.Vel.y > 0.005f) 	player.Vel.y = 0.005f;
	if (player.Vel.x < -0.005f) player.Vel.x = -0.005f;
	if (player.Vel.y < -0.005f)	player.Vel.y = -0.005f;

	/// UPDATE POSITION ///
	if ((player.Pos.x + player.Vel.x) > -width && (player.Pos.x + player.Vel.x < width)) player.Pos.x += player.Vel.x;
	else player.Vel.x = 0.0f;
	if ((player.Pos.y + player.Vel.y > -height) && (player.Pos.y + player.Vel.y < height)) 	player.Pos.y += player.Vel.y;
	else player.Vel.y = 0.0f;
}

void CinderWithKinect01App::drawPlayer(){

	mCamera.lookAt(player.Pos, Vec3f(player.Pos.x, player.Pos.y, -3.0f));

	gl::pushMatrices();

	Rectf		arm(-getWindowWidth() * 2 / 5, -getWindowHeight() / 5, getWindowWidth() * 2 / 5, getWindowHeight() / 5);

	gl::translate(getWindowWidth() / 2, getWindowHeight());
	
	switch (player.gestureId){
	///TURN LEFT 
	case 1: if (player.transTemp > -getWindowHeight() / 20.0f){
				player.transTemp -= getWindowHeight() / 500.0f;
			}

			if (player.rotTemp > -15.0f) {
				player.rotTemp -= 0.3f;
			}

			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
	///TURN RIGHT
	case 2: if (player.transTemp > -getWindowHeight() / 20.0f) {
				player.transTemp -= getWindowHeight() / 500.0f;
			}
			if (player.rotTemp < 15.0f) {
				player.rotTemp += 0.3f;
			}
			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
	///UP LEFT
	case 3: if (player.transTemp > -getWindowHeight() / 20.0f){
				player.transTemp -= getWindowHeight() / 500.0f;
			}

			if (player.rotTemp > -15.0f) {
				player.rotTemp -= 0.3f;
			}

			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
	///UP RIGHT
	case 4: if (player.transTemp > -getWindowHeight() / 20.0f) {
				player.transTemp -= getWindowHeight() / 500.0f;
			}
			if (player.rotTemp < 15.0f) {
				player.rotTemp += 0.3f;
			}
			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
	///UP
	case 5: if (player.rotTemp > 0.0f) player.rotTemp -= 0.3f;
			else player.rotTemp += 0.3f;
			if (player.transTemp > -getWindowHeight() / 20.0f){
				player.transTemp -= getWindowHeight() / 500.0f;
			}
			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
	///DOWN LEFT
	case 6: if (player.rotTemp > -10.0f) {
				player.rotTemp -= 0.3f;
			}
			gl::rotate(player.rotTemp);
			if (player.transTemp < 0.0f){
				player.transTemp += getWindowHeight() / 500.0f;
			}
			else player.transTemp -= getWindowHeight() / 500.0f;

			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break; 
	///DOWN RIGHT
	case 7: if (player.rotTemp < 10.0f) {
				player.rotTemp += 0.3f;
			}
			gl::rotate(player.rotTemp);
			if (player.transTemp < 0.0f){
				player.transTemp += getWindowHeight() / 500.0f;
			}
			else player.transTemp -= getWindowHeight() / 500.0f;

			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;

	///DOWN
	case 0: //player.rotTemp = 0.0f; player.transTemp = 0.0f;
			if (player.rotTemp > 0.0f) {
				player.rotTemp -= 0.3f;
			}
			else {
				player.rotTemp += 0.3f;
			}
			if (player.transTemp < 0.0f){
				player.transTemp += getWindowHeight() / 500.0f;
			}
			else player.transTemp -= getWindowHeight() / 500.0f;
			
			gl::translate(0.0f, player.transTemp);
			gl::rotate(player.rotTemp);
			break;
			
	}
	gl::color(0.1f, 0.3f, 0.5f);
	gl::drawSolidRect(arm);
	gl::popMatrices();

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
