#include "Bacteria.h"

Bacteria::Bacteria(){
	//initial bacteria position
	position.x = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.3) - 0.15f;
	position.y = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)*0.1) - 0.05f;
	position.z = -3.0f;
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
}