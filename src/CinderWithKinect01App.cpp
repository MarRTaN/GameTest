#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include "Kinect.h"

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
			position.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;
			position.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;
			position.z = -3.0f;
		}

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
	mCamera.lookAt(Vec3f(0.0f, 0.5f, 6.0f), Vec3f::zero());
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
}

void CinderWithKinect01App::update()
{
	// Device is capturing
	if ( mKinect->isCapturing() ) {
		mKinect->update();		
		updateBacteria();
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
	gl::translate(0.0f, 2.0f, 0.0f);

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

		// Draw depth and color textures
		/*gl::color(Colorf::white());
		if (mColorSurface) {
			Area srcArea(0, 0, mColorSurface.getWidth(), mColorSurface.getHeight());
			Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
			gl::draw(gl::Texture(mColorSurface), srcArea, destRect);
		}*/
		/*gl::color(Colorf::white());
		if (mDepthSurface) {
		Area srcArea(0, 0, mDepthSurface.getWidth(), mDepthSurface.getHeight());
		Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
		gl::draw(gl::Texture(mDepthSurface), srcArea, destRect);
		console() << mDepthSurface.getChannelAlpha() << std::endl;
		}*/

		//drawNearestHand();
		drawBacteria();
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
/////
CINDER_APP_BASIC( CinderWithKinect01App, RendererGl )
