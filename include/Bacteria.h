#pragma once

#ifndef BACTERIA_INCLUDE
#define BACTERIA_INCLUDE

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class Bacteria{
public:
	Vec3f			position;
	float			vel = 0.05f;
	float			bacteriaSize = 0.05f;
	float			splitAngle = 0.0f;
	bool			isHit = false;
	bool			isOutOfBound = false;
	gl::Texture		bacTexture;
	
	Bacteria(gl::Texture bac);

	void	updatePosition();
	void	draw();
};

#endif