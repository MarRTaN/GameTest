#include "Kinect.h"
#include "Player.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

Player::Player(){}

int Player::getGesture(vector<Skeleton> mSkeletons){
	
	float nearestDepth = 100000.0f;
	vector<Skeleton>::const_iterator nearestSkeleton;
	isTrackedSkeleton = false;
	// Iterate through skeletons
	for (vector<Skeleton>::const_iterator skeletonIt = mSkeletons.cbegin(); skeletonIt != mSkeletons.cend(); ++skeletonIt) {
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
			if (boneIt->first == NUI_SKELETON_POSITION_HAND_LEFT) handLeftPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_HAND_RIGHT) handRightPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_LEFT) elbowLeftPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_ELBOW_RIGHT) elbowRightPos = boneIt->second.getPosition();
			else if (boneIt->first == NUI_SKELETON_POSITION_SHOULDER_CENTER) centerPos = boneIt->second.getPosition();
		}
		if (handLeftPos.x < handRightPos.x &&
		handRightPos.y > elbowRightPos.y && elbowRightPos.y > centerPos.y && elbowLeftPos.y > handLeftPos.y){
		console() << "TURN LEFT" << endl;
		gestureId = 1; ///TURN LEFT
	}
		else if (handLeftPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && elbowLeftPos.y > centerPos.y && elbowRightPos.y > handRightPos.y){
			console() << "TURN RIGHT" << endl;
			gestureId = 2; ///TURN RIGHT
		}

		else if (handLeftPos.x < elbowLeftPos.x && handRightPos.x < elbowRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && handLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && handRightPos.y > centerPos.y){
			console() << "UP LEFT" << endl;
			gestureId = 3; /// UP LEFT
		}

		else if (elbowLeftPos.x < handLeftPos.x && elbowRightPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && handLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && handRightPos.y > centerPos.y){
			console() << "UP RIGHT" << endl;
			gestureId = 4; /// UP RIGHT
		}
		else if (handLeftPos.x < centerPos.x && centerPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && elbowLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && elbowRightPos.y > centerPos.y){
			console() << "UP" << endl;
			gestureId = 5; /// UP STRAIGHT
		}
		else if (handLeftPos.x < handRightPos.x && handRightPos.x < centerPos.x &&
			handLeftPos.y < elbowLeftPos.y && elbowLeftPos.y < centerPos.y &&
			handRightPos.y < elbowRightPos.y && elbowRightPos.y < centerPos.y){
			console() << "DOWN LEFT" << endl;
			gestureId = 6; /// DOWN LEFT
		}

		else if (centerPos.x < handLeftPos.x && handLeftPos.x < handRightPos.x &&
			handLeftPos.y < elbowLeftPos.y && elbowLeftPos.y < centerPos.y &&
			handRightPos.y < elbowRightPos.y && elbowRightPos.y < centerPos.y){
			console() << "DOWN RIGHT" << endl;
			gestureId = 7; /// DOWN RIGHT
		}
		else{
			console() << "DOWN" << endl;
			gestureId = 0; /// DOWN
		}
	}
	return gestureId;
}

bool Player::isTracked(){
	return isTrackedSkeleton;
}
