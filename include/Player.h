#pragma once

#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include "Kinect.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

class Player{
public:
	Player();

	int			gestureId;
	Skeleton	playerSkel;
	Vec3f		Pos;
	Vec3f		Vel;
	float		Acc;
	Vec3f		handLeftPos, handRightPos, elbowLeftPos, elbowRightPos, centerPos;
	float		angle;
	bool		isTrackedSkeleton;
	int getGesture(vector<Skeleton> mSkeletons);
	bool isTracked();
};

#endif