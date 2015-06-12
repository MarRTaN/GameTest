#pragma once

#ifndef GROUND_INCLUDE
#define GROUND_INCLUDE
#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppNative.h"
#include <math.h>
#include "cinder/ImageIo.h"
#include "Kinect.h"
#include "cinder/ImageIo.h"


using namespace ci;
using namespace ci::app;

class Ground{
public:
	Vec3f			groundPos;
	Vec3f			rotRate;
	float			groundRad;
	
	gl::Texture		groundTexture;
	
	Ground();

	void	groundSetup();
	void	updatePosition();
	void	drawGround(ci::CameraPersp &mCamera);
};




#endif