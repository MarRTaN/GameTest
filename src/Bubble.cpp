#include "Bubble.h"

Bubble::Bubble(gl::Texture bubble){
	//initial bacteria position
	position.x = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.7) - 0.35f;
	position.y = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.7) - 0.35f;
	position.z = -4.0f;

	bubbleTexture = bubble;
	bubbleSize = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.2) + 0.01f;
	vel = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.05) + 0.02f;
}

void Bubble::updatePosition(double diffTime){
	//update z
	float curVel = (vel*60.0f*diffTime) / 1000.0f;
	float z = position.z;
	position.z += curVel;
	if (position.z > 1.0f){
		isOutOfBound = true;
	}
}

void Bubble::draw(){
	gl::color(ColorAf::white());

	GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

	
	bubbleTexture.enableAndBind();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gl::drawSphere(position, bubbleSize);
	

	//bubbleTexture.disable();
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
}