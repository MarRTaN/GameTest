#include "Player.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

Player::Player(){}

void Player::setup(){
	Surface8u surface(loadImage(loadAsset("arm.png")));
	armTexture = gl::Texture(surface);
}

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
			else if (boneIt->first == NUI_SKELETON_POSITION_HEAD) headPos = boneIt->second.getPosition();
		}
		if (handLeftPos.x < handRightPos.x &&
		handRightPos.y > elbowRightPos.y && elbowRightPos.y > centerPos.y && elbowLeftPos.y > handLeftPos.y){
		//console() << "TURN LEFT" << endl;
		gestureId = 1; ///TURN LEFT
	}
		else if (handLeftPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && elbowLeftPos.y > centerPos.y && elbowRightPos.y > handRightPos.y){
			//console() << "TURN RIGHT" << endl;
			gestureId = 2; ///TURN RIGHT
		}

		else if (handLeftPos.x < elbowLeftPos.x && handRightPos.x < elbowRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && handLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && handRightPos.y > centerPos.y){
			//console() << "UP LEFT" << endl;
			gestureId = 3; /// UP LEFT
		}

		else if (elbowLeftPos.x < handLeftPos.x && elbowRightPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && handLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && handRightPos.y > centerPos.y){
			//console() << "UP RIGHT" << endl;
			gestureId = 4; /// UP RIGHT
		}
		else if (handLeftPos.x < centerPos.x && centerPos.x < handRightPos.x &&
			handLeftPos.y > elbowLeftPos.y && elbowLeftPos.y > centerPos.y &&
			handRightPos.y > elbowRightPos.y && elbowRightPos.y > centerPos.y){
			//console() << "UP" << endl;
			gestureId = 5; /// UP STRAIGHT
		}
		else if (handLeftPos.x < handRightPos.x && handRightPos.x < centerPos.x &&
			handLeftPos.y < elbowLeftPos.y && elbowLeftPos.y < centerPos.y &&
			handRightPos.y < elbowRightPos.y && elbowRightPos.y < centerPos.y){
			//console() << "DOWN LEFT" << endl;
			gestureId = 6; /// DOWN LEFT
		}

		else if (centerPos.x < handLeftPos.x && handLeftPos.x < handRightPos.x &&
			handLeftPos.y < elbowLeftPos.y && elbowLeftPos.y < centerPos.y &&
			handRightPos.y < elbowRightPos.y && elbowRightPos.y < centerPos.y){
			//console() << "DOWN RIGHT" << endl;
			gestureId = 7; /// DOWN RIGHT
		}
		else{
			//console() << "DOWN" << endl;
			gestureId = 0; /// DOWN
		}
	}

	return gestureId;
}

void Player::updatePlayer(vector<Skeleton> mSkeletons){

	gestureId = getGesture(mSkeletons);

	float distanceLeftX = abs(handLeftPos.x - centerPos.x);
	float distanceLeftY = abs(handLeftPos.y - centerPos.y);
	float distanceRightX = abs(handRightPos.x - centerPos.x);
	float distanceRightY = abs(handRightPos.y - centerPos.y);

	angle = atanf(distanceLeftY / distanceLeftX) + atanf(distanceRightY / distanceRightX);

	if (angle < 0.3f) { Acc = 0.0001f; }
	else if (0.3f < angle && angle < 0.7f){ Acc = 0.0003f; }
	else if (0.7f < angle && angle < 1.1f){ Acc = 0.0006f; }
	else if (1.1f < angle && angle < 1.5f){ Acc = 0.0009f; }
	else if (1.5f < angle && angle < 1.9f){ Acc = 0.00012f; }
	else if (1.9f < angle) { Acc = 0.00015f; }

	/// UPDATE VELOCITY ///
	switch (gestureId) {
		/// TURN LEFT
	case 1: Vel.x -= Acc;
		if (Vel.y > 0.0f) Vel.y -= Acc;
		else  Vel.y += Acc;
		break;
		/// TURN RIGHT
	case 2: Vel.x += Acc;
		if (Vel.y > 0.0f) Vel.y -= Acc;
		else  Vel.y += Acc;
		break;
		/// UP LEFT
	case 3: Vel.x -= Acc;
		Vel.y += Acc;
		break;
		/// UP RIGHT
	case 4: Vel.x += Acc;
		Vel.y += Acc;
		break;
		/// UP STRAIGHT
	case 5: Vel.y += Acc;
		break;
		/// DOWN LEFT
	case 6: Vel.x -= Acc;
		Vel.y -= Acc;
		break;
		/// DOWN RIGHT
	case 7: Vel.x += Acc;
		Vel.y -= Acc;
		break;
		/// DOWN
	case 0: Vel.y -= Acc;
		if (Vel.x > 0.0f) Vel.x -= Acc;
		else Vel.x += Acc;
		break;
	}

	if (Vel.x > 0.005f)	Vel.x = 0.005f;
	if (Vel.y > 0.005f) 	Vel.y = 0.005f;
	if (Vel.x < -0.005f) Vel.x = -0.005f;
	if (Vel.y < -0.005f)	Vel.y = -0.005f;

	/// UPDATE POSITION ///
	if ((Pos.x + Vel.x) > - width && (Pos.x + Vel.x < width)) Pos.x += Vel.x;
	else Vel.x = 0.0f;
	if ((Pos.y + Vel.y > - height) && (Pos.y + Vel.y < height)) 	Pos.y += Vel.y;
	else Vel.y = 0.0f;
}

void Player::drawPlayer(){

	gl::pushMatrices();

	gl::color(1.0f, 1.0f, 1.0f);
	//gl::drawSolidRect(arm);

	//Rectf		arm(-getWindowWidth() * 2 / 5, -getWindowHeight() / 5, getWindowWidth() * 2 / 5, getWindowHeight() / 5);

	gl::translate(getWindowWidth() / 2, getWindowHeight() - 30.0f);

	float rotRate = 0.6f;
	float transDivider = 100.0f;

	switch (gestureId){
		///TURN LEFT 
	case 1: if (transTemp > -getWindowHeight() / 20.0f){
				transTemp -= getWindowHeight() / transDivider;
	}
			if (rotTemp > -15.0f) {
				rotTemp -= rotRate;
			}

			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///TURN RIGHT
	case 2: if (transTemp > -getWindowHeight() / 20.0f) {
				transTemp -= getWindowHeight() / transDivider;
	}
			if (rotTemp < 15.0f) {
				rotTemp += rotRate;
			}
			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///UP LEFT
	case 3: if (transTemp > -getWindowHeight() / 20.0f){
				transTemp -= getWindowHeight() / transDivider;
	}

			if (rotTemp > -15.0f) {
				rotTemp -= rotRate;
			}

			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///UP RIGHT
	case 4: if (transTemp > -getWindowHeight() / 20.0f) {
				transTemp -= getWindowHeight() / transDivider;
	}
			if (rotTemp < 15.0f) {
				rotTemp += rotRate;
			}
			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///UP
	case 5: if (rotTemp > 0.0f) {
				rotTemp -= rotRate;
				if (rotTemp < 0.0f) rotTemp = 0;
	}
			else if (rotTemp < 0.0f){
				rotTemp += rotRate;
				if (rotTemp > 0.0f) rotTemp = 0;
			}

			if (transTemp > -getWindowHeight() / 20.0f){
				transTemp -= getWindowHeight() / transDivider;
			}
			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///DOWN LEFT
	case 6: if (rotTemp > -10.0f) {
				rotTemp -= rotRate;
	}
			gl::rotate(rotTemp);
			if (transTemp < 0.0f){
				transTemp += getWindowHeight() / transDivider;
			}
			else transTemp -= getWindowHeight() / transDivider;

			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;
			///DOWN RIGHT
	case 7: if (rotTemp < 10.0f) {
				rotTemp += rotRate;
	}
			gl::rotate(rotTemp);
			if (transTemp < 0.0f){
				transTemp += getWindowHeight() / transDivider;
			}
			else transTemp -= getWindowHeight() / transDivider;

			gl::translate(0.0f, transTemp);
			gl::rotate(rotTemp);
			break;

			///DOWN
	case 0: //rotTemp = 0.0f; transTemp = 0.0f;
		if (rotTemp > 0.0f) {
			rotTemp -= rotRate;
			if (rotTemp < 0.0f) rotTemp = 0;
		}
		else if (rotTemp < 0.0f){
			rotTemp += rotRate;
			if (rotTemp > 0.0f) rotTemp = 0;
		}

		if (transTemp < 0.0f){
			transTemp += getWindowHeight() / transDivider;
			if (transTemp > 0.0f) transTemp = 0;
		}
		else if (transTemp > 0.0f){
			transTemp -= getWindowHeight() / transDivider;
			if (transTemp < 0.0f) transTemp = 0;
		}

		gl::translate(0.0f, transTemp);
		gl::rotate(rotTemp);
		break;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gl::draw(armTexture, Rectf(-armTexture.getWidth() / 2, -armTexture.getHeight() / 2, armTexture.getWidth() / 2, armTexture.getHeight() / 2));
	glDisable(GL_BLEND);
	gl::popMatrices();

}

bool Player::isTracked(){
	return isTrackedSkeleton;
}
