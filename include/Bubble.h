#pragma once

#ifndef BUBBLE_INCLUDE
#define BUBBLE_INCLUDE

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class Bubble{
public:
	Vec3f			position;
	float			vel = 0.01f;
	float			bubbleSize = 0.05f;
	float			splitAngle = 0.0f;
	bool			isOutOfBound = false;
	gl::Texture		bubbleTexture;

	Bubble(gl::Texture bubble);

	void	updatePosition(double diffTime);
	void	draw();
};

#endif