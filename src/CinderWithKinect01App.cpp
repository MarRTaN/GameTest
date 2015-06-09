#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include "Kinect.h"
#include <math.h>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

int posXRange = 2; //Range -2 to 2
int posYRange = 2; //Range -2 to 2
int bacteriaBornTime = 10;
int bacteriaTimeCount = 0;

class Bacteria{
	public:
		Vec3f	position;
		bool	isHit;
		float	vel = 0.05f;
		Bacteria(){
			//initial bacteria position
			position.x = rand() % (posXRange * 2) - posXRange;
			position.y = rand() % (posYRange * 2) - (posYRange * 2);
			position.z = -3.0f;
		}

};

class Player{
	public:
		int			gestureId;
		Skeleton	playerSkel;
		Vec3f		playerPos;
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

	//Player
	Player								player;

};

// Kinect image size
const Vec2i	kKinectSize( 640, 480 );

void CinderWithKinect01App::prepareSettings( Settings *settings )
{
	settings->setWindowSize(640,480);
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
	mCamera.lookAt(Vec3f(0.0f, 0.0f, 2.0f), Vec3f::zero());
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.01f, 1000.0f);

	player.playerPos = Vec3f(0, 0, -1.0f);
}

void CinderWithKinect01App::update()
{
	// Device is capturing
	if ( mKinect->isCapturing() ) {
		mKinect->update();		
		updateBacteria();
		updatePlayer();
		///test
	} 
	else {
		// If Kinect initialization failed, try again every 90 frames
		if ( getElapsedFrames() % 90 == 0 ) {
			mKinect->start();
		}
	}

}

void CinderWithKinect01App::drawNearestSkeleton(){
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();

	//gl::scale(Vec2f::one() * 1.7f);
	gl::translate(0.0f, 0.0f, 1.8f);

	float nearestDepth = 100000.0f;
	vector<Skeleton>::const_iterator nearestSkeleton;
	bool isTrackedSkeleton = false;;

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
		// Set color
		Colorf color = mKinect->getUserColor(i);

		// Iterate through joints
		for (Skeleton::const_iterator boneIt = nearestSkeleton->cbegin(); boneIt != nearestSkeleton->cend(); ++boneIt) {

			// Set user color
			gl::color(color);

			// Get position and rotation
			const Bone& bone = boneIt->second;
			Vec3f position = bone.getPosition();
			Matrix44f transform = bone.getAbsoluteRotationMatrix();
			Vec3f direction = transform.transformPoint(position).normalized();
			direction *= 0.05f;
			position.z *= -1.0f;

			// Draw bone
			glLineWidth(2.0f);
			JointName startJoint = bone.getStartJoint();
			if (nearestSkeleton->find(startJoint) != nearestSkeleton->end()) {
				Vec3f destination = nearestSkeleton->find(startJoint)->second.getPosition();
				destination.z *= -1.0f;
				gl::drawLine(position, destination);
			}

			// Draw joint
			gl::drawSphere(position, 0.025f, 5);

			// Draw joint orientation				
			glLineWidth(0.5f);
			gl::color(ColorAf::white());
			gl::drawVector(position, position + direction, 0.05f, 0.01f);

		}
	}

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
}

void CinderWithKinect01App::drawNearestBone(){
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();
	//gl::scale(Vec2f::one() * 1.7f);
	gl::translate(0.0f, 0.0f, 1.8f);

	float nearestDepth = 100000.0f;
	Skeleton::const_iterator nearestBone;
	vector<Skeleton>::const_iterator nearestSkeleton;
	int countSkeleton = 0;

	// Iterate through skeletons
	uint32_t i = 0;
	for (vector<Skeleton>::const_iterator skeletonIt = mSkeletons.cbegin(); skeletonIt != mSkeletons.cend(); ++skeletonIt, i++) {

		// Iterate through joints
		for (Skeleton::const_iterator boneIt = skeletonIt->cbegin(); boneIt != skeletonIt->cend(); ++boneIt) {

			const Bone& bone = boneIt->second;
			Vec3f position = bone.getPosition();

			if (position.z < nearestDepth) {
				nearestDepth = position.z;
				nearestBone = boneIt;
				nearestSkeleton = skeletonIt;
				countSkeleton++;
			}

		}

	}


	if (countSkeleton > 0){
		// Set color
		Colorf color = mKinect->getUserColor(i);

		// Set user color
		gl::color(color);

		// Get position and rotation
		const Bone& bone = nearestBone->second;
		Vec3f position = bone.getPosition();
		Matrix44f transform = bone.getAbsoluteRotationMatrix();
		Vec3f direction = transform.transformPoint(position).normalized();
		direction *= 0.05f;
		position.z *= -1.0f;

		// Draw bone
		glLineWidth(2.0f);
		JointName startJoint = bone.getStartJoint();
		if (nearestSkeleton->find(startJoint) != nearestSkeleton->end()) {
			Vec3f destination = nearestSkeleton->find(startJoint)->second.getPosition();
			destination.z *= -1.0f;
			gl::drawLine(position, destination);
		}


		console() << startJoint << std::endl;

		// Draw joint
		gl::drawSphere(position, 0.1f, 16);
	}

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
}

void CinderWithKinect01App::drawNearestHand(){
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();
	//gl::scale(Vec2f::one() * 1.7f);
	gl::translate(0.0f, 0.0f, 1.8f);

	float nearestDepth = 100000.0f;

	// Iterate through skeletons
	uint32_t i = 0;
	for (vector<Skeleton>::const_iterator skeletonIt = mSkeletons.cbegin(); skeletonIt != mSkeletons.cend(); ++skeletonIt, i++) {

		// Iterate through joints
		for (Skeleton::const_iterator boneIt = skeletonIt->cbegin(); boneIt != skeletonIt->cend(); ++boneIt) {

			const Bone& bone = boneIt->second;
			Vec3f position = bone.getPosition();

			// Set color
			Colorf color = mKinect->getUserColor(i);

			// Iterate through joints
			for (Skeleton::const_iterator boneIt = skeletonIt->cbegin(); boneIt != skeletonIt->cend(); ++boneIt) {

				const Bone& bone = boneIt->second;
				JointName startJoint = bone.getStartJoint();

				if (startJoint == 6 || startJoint == 10){

					// Set user color
					gl::color(color);


					// Get position and rotation
					Vec3f position = bone.getPosition();
					Matrix44f transform = bone.getAbsoluteRotationMatrix();
					Vec3f direction = transform.transformPoint(position).normalized();
					direction *= 0.05f;
					position.z *= -1.0f;

					// Draw joint
					//console() << position.x << "," << position.y << "," << position.z << std::endl;
					gl::drawSphere(position, 0.05f, 10);

					// Draw joint orientation				
					glLineWidth(0.5f);
					gl::color(ColorAf::white());
					gl::drawVector(position, position + direction, 0.05f, 0.01f);				
				}
			}

		}
	}

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
}

void CinderWithKinect01App::updateBacteria(){
	if (bacteriaTimeCount > bacteriaBornTime){
		Bacteria newBac = Bacteria();
		bacterias.push_back(newBac);
		bacteriaTimeCount = 0;
	}

	bacteriaTimeCount++;

	for (int i = 0; i < bacterias.size(); i++){
		float z = bacterias[i].position.z;
		bacterias[i].position.z += bacterias[i].vel;
		if (z > 0.0f){
			//bacterias.erase(std::find(bacterias.begin(), bacterias.end(), bacterias[i]));
		}
	}
}

void CinderWithKinect01App::drawBacteria(){
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();

	for (int i = 0; i < bacterias.size(); i++){
		gl::color(ColorAf::white());
		gl::drawSphere(bacterias[i].position, 0.05f, 10);
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

		drawBacteria();
		drawPlayer();
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
			//else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_LEFT) player.elbowLeftPos = boneIt->second.getPosition();
			//else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_RIGHT) player.elbowRightPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_SHOULDER_CENTER) player.centerPos = boneIt->second.getPosition();
		}

		if (player.handLeftPos.y < player.centerPos.y && player.centerPos.y < player.handRightPos.y && player.handLeftPos.x < player.centerPos.x && player.centerPos.x < player.handRightPos.x){
			player.gestureId = 1;
			console() << "FLY LEFT" << endl;
		}
		else if (player.handLeftPos.y > player.centerPos.y && player.centerPos.y > player.handRightPos.y && player.handLeftPos.x < player.centerPos.x && player.centerPos.x < player.handRightPos.x){
			player.gestureId = 2;
			console() << "FLY RIGHT" << endl;
		}
		else if (player.handLeftPos.y > player.centerPos.y && player.centerPos.y < player.handRightPos.y && player.handLeftPos.x < player.centerPos.x && player.centerPos.x < player.handRightPos.x){
			player.gestureId = 3;
			console() << "FLY UP" << endl;
		}
		else if (player.handLeftPos.y < player.centerPos.y && player.centerPos.y > player.handRightPos.y && player.handLeftPos.x < player.centerPos.x && player.centerPos.x < player.handRightPos.x){
			player.gestureId = 4;
			console() << "FLY DOWN" << endl;
		}
		else{
			player.gestureId = 0;
			console() << " STILL " << endl;
		}
	}
}

void CinderWithKinect01App::updatePlayer(){
	getGesture();
	float distanceLeftX = abs(player.handLeftPos.x - player.centerPos.x);
	float distanceLeftY = abs(player.handLeftPos.y - player.centerPos.y);
	float distanceRightX = abs(player.handRightPos.x - player.centerPos.x);
	float distanceRightY = abs(player.handRightPos.y - player.centerPos.y);
	player.angle = atanf(distanceLeftY / distanceLeftY) + atanf(distanceRightY / distanceRightY);
	player.angle = player.angle/50.0f;
	switch (player.gestureId) {
		case 1: if (player.playerPos.x > -1.0f && player.playerPos.y < 0.4f) player.playerPos.x -= player.angle; player.playerPos.y += player.angle; break;
		case 2: if (player.playerPos.x < 1.0f && player.playerPos.y < 0.4f) player.playerPos.x += player.angle; player.playerPos.y += player.angle; break;
		case 3: if (player.playerPos.y < 0.6f) player.playerPos.y += player.angle; break;
		case 4: if (player.playerPos.y > -0.6f) player.playerPos.y -= player.angle; break;
	}
}

void CinderWithKinect01App::drawPlayer(){
	// Set up 3D view
	gl::setMatrices(mCamera);

	// Move skeletons down below the rest of the interface
	gl::pushMatrices();
	gl::color(0, 255, 0);
	gl::drawColorCube(player.playerPos, Vec3f(0.5f,0.5f,0.5f));
	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);
}


CINDER_APP_BASIC( CinderWithKinect01App, RendererGl )
