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
#include "cinder/qtime/QuickTime.h"
#include "cinder/Surface.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/Capture.h"

#include "cinder/ObjLoader.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"

using namespace ci; 
using namespace ci::app;
using namespace std;

class Stage{
	public:
		int						timer = 0;
		int						score = 0;
		gl::Texture				stageTexture;
		gl::Texture				handTexture;
		gl::Texture				movieTexture;
		gl::Texture				groundTexture;
		gl::Texture				button0Texture;
		gl::Texture				button1Texture;
		gl::Texture				button2Texture;
		gl::Texture				button3Texture;
		qtime::MovieGlRef		mMovie;
		float					worldAngle;
		ci::CameraPersp			sCamera;

		//World
		TriMesh			worldMesh;
		gl::VboMesh		worldVBO;
		gl::GlslProg	worldShader;
		gl::Texture		worldTexture;

		void					setup();
		void					nextStage();
		void					updateStage(Vec3f pos, ci::CameraPersp mCamera);
		void					drawStage();
		void					drawTime();
		void					setHandPosition(Vec3f hand);
		Vec3f					getHandPosition();
		int						getStage();
		void					loadMovieFile();

	private:
		int						stageNum = 0;
		Vec3f					handPos;
		float				    space = 70.0f;
		float					change = 2.0f;
};

#endif