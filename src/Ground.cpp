#include "Ground.h"


Ground::Ground(){
	groundPos = Vec3f(0.0f, -6.5f, -6.0f);
	groundRad = 5.0f;
}

void Ground::groundSetup(){
	rotRate = Vec3f(0.0f, 0.0f, 90.0f);
	Surface8u surface(loadImage(loadAsset("cloth.png")));
	groundTexture = gl::Texture(surface);
}
void Ground::updatePosition(){
	if (rotRate.x > 360.0f) rotRate.x = 0.0f;
	rotRate += Vec3f(0.1f, 0.0f, 0.0f);
}

void Ground::drawGround(ci::CameraPersp &mCamera){
	gl::pushMatrices();
	gl::setMatrices(mCamera);
	gl::color(1,1,1);
	
	/*
	GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_specular[] = { 1.2f, 1.2f, 1.2f, 1.0f };
	GLfloat light_position[] = { 30.0f, 30.0f, 30.0f };

	GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat high_shininess[] = { 10.0f };

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	*/
	groundTexture.enableAndBind();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	gl::translate(groundPos);
	gl::rotate(rotRate);
	gl::drawSphere(Vec3f(0.0f,0.0f,0.0f), groundRad,100);

	groundTexture.disable();
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	gl::popMatrices();
}