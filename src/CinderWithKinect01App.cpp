#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include <math.h>
#include "Kinect.h"
#include "Bacteria.h"
#include "Stage.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

int posXRange = 2; //Range -2 to 2
int posYRange = 2; //Range -2 to 2
int bacteriaBornTime = 100;
int bacteriaTimeCount = 0;
bool hit = false;

class Player{
	public:
		int			gestureId;
		Skeleton	playerSkel;
		Vec3f		Pos;
		Vec3f		Vel;
		float		Acc;
		Vec3f		handLeftPos, handRightPos, elbowLeftPos, elbowRightPos,centerPos;
		float		angle;
};

class CinderWithKinect01App : public AppBasic 
{
  public:
	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void setup();
	void update();
	void draw();
	void drawNearestSkeleton();
	void drawNearestBone();
	void drawNearestHand();
	void keyDown( KeyEvent event );
	void shutdown();
	void updateBacteria();
	void drawBacteria();
	void getGesture();
	void updatePlayer();
	void drawPlayer();
  private:
	vector<Vec3f>			mPoints;
	ci::CameraPersp			mCamera;

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
	float								stillMove=0.01f;
	int									countStillMove;

	//Window
	float								width = 0.12;
	float								height = 0.12;



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

	player.Pos = Vec3f(0, 0, 1.0f);
}

void CinderWithKinect01App::update()
{
	// Device is capturing
	if ( mKinect->isCapturing() ) {
		mKinect->update();
		updatePlayer();
		stage.updateStage(player.Pos);
		if (stage.getStage() == 1){
			updateBacteria();
		}
	} 
	else {
		// If Kinect initialization failed, try again every 90 frames
		if ( getElapsedFrames() % 90 == 0 ) {
			mKinect->start();
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
		float hitRange = 0.1f;
		Vec3f pl = player.Pos;
		Vec3f ba = bacterias[i].position;

		//Check Baacteria Hit
		if (abs(pl.x - ba.x) < hitRange && abs(pl.y - ba.y) < hitRange && ba.z > -0.1f){
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
	gl::translate(0.0f, 0.0f, 0.0f);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();

	for (int i = 0; i < bacterias.size(); i++){
		bacterias[i].draw();
	}

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
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

		drawPlayer();
		if(stage.getStage() == 1) drawBacteria();
	}
}


void CinderWithKinect01App::keyDown( KeyEvent event )
{
	// Key on key...
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
		case KeyEvent::KEY_f:
			setFullScreen( ! isFullScreen() );
		break;
		case KeyEvent::KEY_a:
			hit = true;
		break;
		case KeyEvent::KEY_s:
			stage.nextStage();
		break;
	}
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

void CinderWithKinect01App::getGesture(){

	float nearestDepth = 100000.0f;
	vector<Skeleton>::const_iterator nearestSkeleton;
	bool isTrackedSkeleton = false;
	// Iterate through skeletons
	uint32_t i = 0;
	for (vector<Skeleton>::const_iterator skeletonIt = mSkeletons.cbegin(); skeletonIt != mSkeletons.cend(); ++skeletonIt, i++) {

		// Iterate through joints
		for (Skeleton::const_iterator boneIt = skeletonIt->cbegin(); boneIt != skeletonIt->cend(); ++boneIt) {

			const Bone& bone = boneIt->second;
			Vec3f position = bone.getPosition();

			if (position.z < nearestDepth) {
				nearestDepth = position.z;
				nearestSkeleton = skeletonIt;
				isTrackedSkeleton = true;
			}
		}
	}

	if (isTrackedSkeleton){
		for (Skeleton::const_iterator boneIt = nearestSkeleton->cbegin(); boneIt != nearestSkeleton->cend(); ++boneIt){
			if (boneIt->first == NUI_SKELETON_POSITION_HAND_LEFT) player.handLeftPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_HAND_RIGHT) player.handRightPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_LEFT) player.elbowLeftPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_RIGHT) player.elbowRightPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_SHOULDER_CENTER) player.centerPos = boneIt->second.getPosition();
		}

		if (player.handLeftPos.x < player.handRightPos.x &&
			player.handLeftPos.y > player.elbowLeftPos.y && player.elbowLeftPos.y > player.centerPos.y && player.centerPos.y > player.elbowRightPos.y && player.elbowRightPos.y > player.handRightPos.y){
			player.gestureId = 1; ///TURN LEFT
		}
		else if (player.handLeftPos.x < player.handRightPos.x &&
				 player.handRightPos.y > player.elbowRightPos.y && player.elbowRightPos.y > player.centerPos.y && player.centerPos.y > player.elbowLeftPos.y && player.elbowLeftPos.y > player.handLeftPos.y){
				 player.gestureId = 2; ///TURN RIGHT
		}
		else if (player.handLeftPos.x < player.handRightPos.x && player.handRightPos.x < player.centerPos.x && 
				 player.handLeftPos.y > player.elbowLeftPos.y && player.elbowLeftPos.y > player.centerPos.y &&
				 player.handRightPos.y > player.elbowRightPos.y && player.elbowRightPos.y > player.centerPos.y){
			player.gestureId = 3; /// UP LEFT
		}

		else if (player.centerPos.x < player.handLeftPos.x && player.handLeftPos.x < player.handRightPos.x &&
				 player.handLeftPos.y > player.elbowLeftPos.y && player.elbowLeftPos.y > player.centerPos.y &&
				 player.handRightPos.y > player.elbowRightPos.y && player.elbowRightPos.y > player.centerPos.y){
			player.gestureId = 4; /// UP RIGHT
		}
		else if (player.handLeftPos.x < player.centerPos.x && player.centerPos.x < player.handRightPos.x &&
				player.handLeftPos.y > player.elbowLeftPos.y && player.elbowLeftPos.y > player.centerPos.y &&
				player.handRightPos.y > player.elbowRightPos.y && player.elbowRightPos.y > player.centerPos.y){
			player.gestureId = 5; /// UP STRAIGHT
		}
		else if (player.handLeftPos.x < player.handRightPos.x && player.handRightPos.x < player.centerPos.x &&
			player.handLeftPos.y < player.elbowLeftPos.y && player.elbowLeftPos.y < player.centerPos.y &&
			player.handRightPos.y < player.elbowRightPos.y && player.elbowRightPos.y < player.centerPos.y){
			player.gestureId = 6; /// DOWN LEFT
		}

		else if (player.centerPos.x < player.handLeftPos.x && player.handLeftPos.x < player.handRightPos.x &&
			player.handLeftPos.y < player.elbowLeftPos.y && player.elbowLeftPos.y < player.centerPos.y &&
			player.handRightPos.y < player.elbowRightPos.y && player.elbowRightPos.y < player.centerPos.y){
			player.gestureId = 7; /// DOWN RIGHT
		}
		else{
			player.gestureId = 0; /// DOWN
		}
	}
	//console() << player.gestureId << endl;
}

void CinderWithKinect01App::updatePlayer(){
	getGesture();
	float distanceLeftX = abs(player.handLeftPos.x - player.centerPos.x);
	float distanceLeftY = abs(player.handLeftPos.y - player.centerPos.y);
	float distanceRightX = abs(player.handRightPos.x - player.centerPos.x);
	float distanceRightY = abs(player.handRightPos.y - player.centerPos.y);

	player.angle = atanf(distanceLeftY / distanceLeftX) + atanf(distanceRightY / distanceRightX);

	if (player.angle < 0.3f) player.Acc = 0.001f;
	else if (0.3f < player.angle && player.angle < 0.7f) player.Acc = 0.001f;
	else if (0.7f < player.angle && player.angle < 1.1f) player.Acc = 0.002f;
	else if (1.1f < player.angle && player.angle < 1.5f) player.Acc = 0.003f;
	else if (1.5f < player.angle && player.angle < 1.9f) player.Acc = 0.004f;
	else if (1.9f < player.angle) player.Acc = 0.005f;

	/// UPDATE VELOCITY ///
	switch (player.gestureId) {
		/// TURN LEFT
		case 2: player.Vel.x -= player.Acc; 
				if (player.Vel.y > 0.0f) player.Vel.y -= player.Acc/50.0f;
				else  player.Vel.y += player.Acc/50.0f;
				break;
		/// TURN RIGHT
		case 1: player.Vel.x += player.Acc;
				if (player.Vel.y > 0.0f) player.Vel.y -= player.Acc/50.0f;
				else  player.Vel.y += player.Acc/50.0f;
				break;
		/// UP LEFT
		case 4: player.Vel.x -= player.Acc / 20.0f; 
				player.Vel.y += player.Acc; 
				break;
		/// UP RIGHT
		case 3: player.Vel.x += player.Acc / 20.0f;
				player.Vel.y += player.Acc;
				break;
		/// UP STRAIGHT
		case 5: player.Vel.y += player.Acc / 20.0f; 
				break;
		/// DOWN LEFT
		case 7: player.Vel.x -= player.Acc / 20.0f;
				player.Vel.y -= player.Acc;
				break;
		/// DOWN RIGHT
		case 6: player.Vel.x += player.Acc / 20.0f;
				player.Vel.y -= player.Acc;
				break;
		/// DOWN
		case 0: player.Vel.y -= player.Acc / 20.0f;
				if (player.Vel.x > 0.0f) player.Vel.x -= player.Acc / 100.0f;
				else player.Vel.x += player.Acc / 100.0f;
				break;
	}

	if (player.Vel.x > 0.01f) player.Vel.x = 0.01f;
	if (player.Vel.y > 0.01f) player.Vel.y = 0.01f;
	if (player.Vel.x < -0.01f) player.Vel.x = -0.01f;
	if (player.Vel.y < -0.01f) player.Vel.y = -0.01f;

	if (player.gestureId !=0){
		/// UPDATE POSITION ///
		if ((player.Pos.x + player.Vel.x) > -width && (player.Pos.x + player.Vel.x < width)) {
			player.Pos.x += player.Vel.x;
		}
		else player.Vel.x = 0.0f;
		if ((player.Pos.y + player.Vel.y > -height) && (player.Pos.y + player.Vel.y < height)) {
			player.Pos.y += player.Vel.y;
		}
		else player.Vel.y = 0.0f;
	}
	
	
}

void CinderWithKinect01App::drawPlayer(){
	//mCamera.lookAt(player.Pos, Vec3f(player.Pos.x,player.Pos.y,0.0f));
	mCamera.lookAt(player.Pos, Vec3f(player.Pos.x, player.Pos.y, -3.0f));
	/*
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();
	gl::color(0, 255, 0);
	gl::drawColorCube(player.Pos, Vec3f(0.2f,0.2f,0.2f));
	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);*/
}


CINDER_APP_BASIC( CinderWithKinect01App, RendererGl )
