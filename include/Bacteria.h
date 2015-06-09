#pragma once

#ifndef BACTERIA_INCLUDE
#define BACTERIA_INCLUDE

#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"

using namespace ci;

class Bacteria{
public:
	Vec3f	position;
	float	vel = 0.05f;
	float	bacteriaSize = 0.05f;
	float	splitAngle = 0.0f;
	bool	isHit = false;
	bool	isOutOfBound = false;
	
	Bacteria();

	void	updatePosition();
	void	draw();
};

#endif