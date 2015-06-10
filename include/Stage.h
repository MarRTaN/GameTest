#pragma once

#ifndef STAGE_INCLUDE
#define STAGE_INCLUDE

#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"

using namespace ci;

class Stage{
	public:
		int						timer = 0;
		int						score = 0;
		ci::CameraPersp			sCamera;
		gl::Texture				stageTexture;
		gl::Texture				handTexture;

		void					setup();
		void					nextStage();
		void					updateStage(Vec3f pos, ci::CameraPersp cam);
		void					drawStage();
		void					drawTime();
		void					setHandPosition(Vec3f hand);
		Vec3f					getHandPosition();
		int						getStage();

	private:
		int						stageNum = 0;
		Vec3f					handPos;
};

#endif