#pragma once

#ifndef STAGE_INCLUDE
#define STAGE_INCLUDE

#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"

using namespace ci;

class Stage{
	public:
		int		timer = 0;
		int		score = 0;
		Vec3f	handPos;

		void	nextStage();
		void	updateStage(Vec3f pos);
		void	drawStage();
		void	getHandPosition();
		int		getStage();

	private:
		int		stageNum = 0;
};

#endif