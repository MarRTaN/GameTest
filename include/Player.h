#pragma once

#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "Kinect.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace KinectSdk;

class Player{
public:
	Player();

	int					gestureId;
	Vec3f				Pos;
	Vec3f				Vel;
	float				Acc;
	Vec3f				handLeftPos, handRightPos, elbowLeftPos, elbowRightPos, centerPos, headPos;
	float				angle;
	bool				isTrackedSkeleton;
	float				rotTemp=0.0f;
	float				transTemp=0.0f;
	gl::Texture			armTexture;

	//Window
	float								width = 0.3;
	float								height = 0.2;

	void		setup();
	int			getGesture(vector<Skeleton> mSkeletons);
	bool		isTracked();
	void		createDummy();
	void		drawPlayer();
	void		updatePlayer(vector<Skeleton> mSkeletons);
};

#endif