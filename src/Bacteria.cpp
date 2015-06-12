#include "Bacteria.h"

Bacteria::Bacteria(gl::Texture bac){
	//initial bacteria position
	position.x = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.3) - 0.15f;
	position.y = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.1) - 0.05f;
	position.z = -3.0f;

	bacTexture = bac;
}

void Bacteria::updatePosition(){
	//update z
	float z = position.z;
	position.z += vel;
	if (position.z > 1.0f){
		isOutOfBound = true;
	}
}

void Bacteria::draw(){
	gl::color(ColorAf::white());

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

	bacTexture.enableAndBind();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!isHit){
		gl::drawSphere(position, bacteriaSize, 10);
	}
	else {
		bacteriaSize = bacteriaSize / 1.5;

		//draw split
		float x, y, z;
		x = (cos(splitAngle + (rand() % 80))*0.05f) + position.x;
		y = (sin(splitAngle + (rand() % 80))*0.05f) + position.y;
		z = position.z + vel / (rand() % 4 + 2);
		gl::drawSphere(Vec3f(x, y, z), bacteriaSize, 10);

		x = (cos(splitAngle + (rand() % 80 + 90.0f))*0.05f) + position.x;
		y = (sin(splitAngle + (rand() % 80 + 90.0f))*0.05f) + position.y;
		z = position.z + vel / (rand() % 4 + 2);
		gl::drawSphere(Vec3f(x, y, z), bacteriaSize, 10);

		x = (cos(splitAngle + (rand() % 80 + 180.0f))*0.05f) + position.x;
		y = (sin(splitAngle + (rand() % 80 + 180.0f))*0.05f) + position.y;
		z = position.z + vel / (rand() % 4 + 2);
		gl::drawSphere(Vec3f(x, y, z), bacteriaSize, 10);

		x = (cos(splitAngle + (rand() % 80 + 135.0f))*0.025f) + position.x;
		y = (sin(splitAngle + (rand() % 80 + 135.0f))*0.025f) + position.y;
		z = position.z + vel / (rand() % 4 + 2);
		gl::drawSphere(Vec3f(x, y, z), bacteriaSize, 10);

		splitAngle += 0.1f;
	}

	bacTexture.disable();
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
}