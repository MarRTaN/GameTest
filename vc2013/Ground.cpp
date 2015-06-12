#include "Ground.h"


Ground::Ground(){
	//initial bacteria position
	groundPos = Vec3f(0.0f, 0.0f, 0.0f);
	rotRate = Vec3f(5.0f, 0.0f, 0.0f);
	groundRad = 2.0f;
	groundTexture = gl::Texture(loadImage(loadAsset("Stage01.png")));

}


void Ground::updatePosition(){
	if (rotRate.x > 360.0f) rotRate.x = 0.0f;
	rotRate += Vec3f(5.0f, 0.0f, 0.0f);
}

void Ground::drawGround(){
	gl::pushMatrices();
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

	groundTexture.enableAndBind();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//gl::translate(0.0f, -0.5f, 0.0f);
	gl::rotate(rotRate);
	gl::color(ColorAf::white());
	gl::drawSphere(groundPos, groundRad, 0);

	groundTexture.disable();
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);


}