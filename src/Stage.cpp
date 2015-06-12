#include "Stage.h"

using namespace ci;
using namespace ci::app;

void Stage::setup(){
	ci::Surface8u surface(loadImage(loadAsset("Stage01-2.PNG")));
	stageTexture = gl::Texture(surface);

	ci::Surface8u surfaceHand(loadImage(loadAsset("hand.png")));
	handTexture = gl::Texture(surfaceHand);

	Surface8u surfaceButton0(loadImage(loadAsset("button0.png")));
	button0Texture = gl::Texture(surfaceButton0);
	
	Surface8u surfaceButton1(loadImage(loadAsset("button1.png")));
	button1Texture = gl::Texture(surfaceButton1);
	
	Surface8u surfaceButton2(loadImage(loadAsset("button2.png")));
	button2Texture = gl::Texture(surfaceButton2);
	
	Surface8u surfaceButton3(loadImage(loadAsset("button3.png")));
	button3Texture = gl::Texture(surfaceButton3);
	//worldTexture = gl::Texture(loadImage(loadAsset("football.jpg")));

	bubbleTexture = gl::Texture(loadImage(loadAsset("obj/bubble.png")));

	logoTexture = gl::Texture(loadImage(loadAsset("logo.png")));

	//import obj
	/*ObjLoader loader(loadAsset("obj/Cap.obj"));
	loader.load(&worldMesh);
	worldVBO = gl::VboMesh(worldMesh);

	worldTexture = gl::Texture(loadImage(loadAsset("football.jpg")));
	worldShader = gl::GlslProg(loadAsset("obj/shader.vert"), loadAsset("obj/shader.frag"));*/

}

void Stage::nextStage(){
	timer = 0;
	stageNum++;
	if (stageNum > 3) stageNum = 0;
	if (stageNum == 0){
		ci::Surface8u surface(loadImage(loadAsset("Stage01-2.PNG")));
		stageTexture = gl::Texture(surface);
	}
	else if (stageNum == 1){
		loadMovieFile();
		ci::Surface8u surface(loadImage(loadAsset("Stage01-2.PNG")));
		stageTexture = gl::Texture(surface);
	}
	else if (stageNum == 2) {
		score = 0;
		ci::Surface8u surface(loadImage(loadAsset("Stage02.PNG")));
		stageTexture = gl::Texture(surface);
	}
	else if (stageNum == 3){
		ci::Surface8u surface(loadImage(loadAsset("Stage03.PNG")));
		stageTexture = gl::Texture(surface);
	}
	
}

int	Stage::getStage(){
	return stageNum;
}

Vec3f Stage::getHandPosition(){
	return handPos;
}

void Stage::setHandPosition(Vec3f hand){
	handPos = hand;
	handPos.z = -1.0f;
}

void Stage::loadMovieFile()
{
	try {
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGl::create(loadAsset("test.mov"));
		//mMovie->setLoop();
		//mMovie->play();
	}
	catch (...) {
		console() << "Unable to load the movie." << std::endl;
		mMovie->reset();
	}

	movieTexture.reset();
}

void Stage::updateStage(Vec3f pos, ci::CameraPersp mCamera){
	setHandPosition(pos);
	//update stage 0
	if (stageNum == 0){
		if (timer > 120) nextStage();
	}
	//update stage 1
	else if (stageNum == 1){
		timer++;
		if (timer > 2.0f*60.0f) nextStage();
		if (mMovie)
			movieTexture = mMovie->getTexture();
	}
	//update stage 2
	else if (stageNum == 2){
		sCamera = mCamera;
		timer++;
		if (timer > 61.0f*60.0f) nextStage();
	}
	//update stage 3
	else if (stageNum == 3){
		timer++;
		if (timer > 10.0f*60.0f) nextStage();
	}
}

void Stage::drawStage(){
	//draw stage 0
	if (stageNum == 0){
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/*gl::color(Color(1.0f, 0.0f, 0.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));*/
		Area srcArea(0, 0, stageTexture.getWidth(), stageTexture.getHeight());
		Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
		gl::draw(stageTexture, srcArea, destRect);

		float buttonSize = 90.0f;
		float handSize = 55.0f;
		Vec2f buttonPos = Vec2f(getWindowWidth() / 2, getWindowHeight() / 2 + 100.0f);
		Vec2f handIn2D = Vec2f(getWindowWidth()*(getHandPosition().x + 0.5) / 1.0f, -1.0f*getWindowHeight()*(getHandPosition().y - 0.75) / 1.5f);

		//draw button
		if (abs(handIn2D.x - buttonPos.x) < (handSize + buttonSize) / 2 && abs(handIn2D.y - buttonPos.y) < (handSize + buttonSize) / 2){
			gl::color(Color(0.1f, 0.0f, 1.0f));
			timer++;
		}
		else { timer = 0; }


		float shift = 130.0f;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl::color(1, 1, 1);
		if (timer > 0 && timer < 40) draw(button3Texture, Rectf(getWindowWidth()/2 - buttonSize, getWindowHeight()/2 - buttonSize + shift, getWindowWidth()/2 + buttonSize, getWindowHeight()/2 + buttonSize + shift));
		else if (timer > 40 && timer < 80) draw(button2Texture, Rectf(getWindowWidth() / 2 - buttonSize, getWindowHeight() / 2 - buttonSize + shift, getWindowWidth() / 2 + buttonSize, getWindowHeight() / 2 + buttonSize + shift));
		else if (timer > 80 && timer < 120) draw(button1Texture, Rectf(getWindowWidth() / 2 - buttonSize, getWindowHeight() / 2 - buttonSize + shift, getWindowWidth() / 2 + buttonSize, getWindowHeight() / 2 + buttonSize + shift));
		else draw(button0Texture, Rectf(getWindowWidth() / 2 - buttonSize, getWindowHeight() / 2 - buttonSize + shift, getWindowWidth() / 2 + buttonSize, getWindowHeight() / 2 + buttonSize + shift));
		//gl::draw(armTexture, Rectf(-armTexture.getWidth() / 2, -armTexture.getHeight() / 2, armTexture.getWidth() / 2, armTexture.getHeight() / 2));
		glDisable(GL_BLEND);
		gl::popMatrices();



		//gl::drawSolidCircle(buttonPos, buttonSize);
		gl::color(1.0f, 1.0f, 1.0f);

	}
	//draw stage 1
	else if (stageNum == 1){

		//draw BG
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
		Area srcArea(0, 0, stageTexture.getWidth(), stageTexture.getHeight());
		Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
		gl::draw(stageTexture, srcArea, destRect);

		//draw video
		gl::enableAlphaBlending();
		if (movieTexture) {
			gl::scale(Vec2f(0.8f, 0.8f));
			Rectf centeredRect = Rectf(movieTexture.getBounds()).getCenteredFit(getWindowBounds(), true);
			centeredRect.x1 += 0.1f*getWindowWidth();
			centeredRect.x2 += 0.15f*getWindowWidth();
			centeredRect.y1 -= 100.0f;
			centeredRect.y2 -= 100.0f;
			gl::draw(movieTexture, centeredRect);
			gl::scale(Vec2f(1.0f, 1.0f));
		}
		gl::disableAlphaBlending();
	}
	//draw stage 2 :: game
	else if (stageNum == 2){

		//draw BG
		gl::setMatrices(sCamera);
		gl::pushMatrices();
		/*gl::color(1.0f, 1.0f, 1.0f);
		Area srcArea(0, 0, stageTexture.getWidth(), stageTexture.getHeight());
		Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
		gl::draw(stageTexture, srcArea, destRect);*/

		gl::color(1.0f, 1.0f, 1.0f);
		stageTexture.enableAndBind();
		gl::rotate(180.0f);
		gl::drawCube(Vec3f(0.0f, 0.0f, -5.0f), Vec3f(3.9f,6.0f, 0.01f));
		stageTexture.disable();

		//draw Big Bubble BG
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

		gl::drawSphere(Vec3f(0.0f, -4.0f, -4.5f), 0.3f);


		//bubbleTexture.disable();
		glDisable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHT0);
		glDisable(GL_NORMALIZE);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);

		gl::popMatrices();

		//draw world

		// Set up 3D view
		gl::setMatrices(sCamera);

		gl::pushMatrices();
		/*worldTexture.bind();
		worldShader.bind();
		worldShader.uniform("tex0", 0);
		gl::draw(worldVBO);
		worldTexture.unbind();
		worldShader.unbind();*/
		
		/*gl::color(Color(1.0f, 1.0f, 1.0f));
		worldTexture.enableAndBind();
		glEnable(GL_TEXTURE_2D);
		gl::rotate(worldAngle);
		gl::drawSphere(Vec3f(0.0f, 0.0f, -0.5f), 0.1f, 10);
		gl::rotate(-worldAngle);
		worldTexture.disable();

		worldAngle += 0.01;*/

		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
	}
	//draw stage 3
	else if (stageNum == 3){
		
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
		gl::color(1.0f, 1.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Area srcArea(0, 0, stageTexture.getWidth(), stageTexture.getHeight());
		Rectf destRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());
		gl::draw(stageTexture, srcArea, destRect);
		gl::drawString(std::to_string(score), Vec2f(50.0f, 120.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 150.0f));
		gl::disableAlphaBlending();
		/*
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

		gl::color(Color(0.0f, 0.0f, 1.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));

		gl::enableAlphaBlending();
		//draw score



		gl::drawString("Good Job !!\n Your score is \n"+std::to_string(score), Vec2f(30.0f, 30.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 80.0f));
		gl::disableAlphaBlending();
		*/
		
	}
}

void Stage::drawTime(){

	gl::enableAlphaBlending();

	float textShift;
	if (score > 99) textShift = 60.0f;
	else if (score > 9) textShift = 40.0f;
	else textShift = 20.0f;

	//draw score
	gl::drawString(std::to_string(score), Vec2f(getWindowWidth() / 2 - textShift, 70.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 80.0f));

	//draw time
	float newTime = 60.0f - (timer / 60.0f);
	float timeBarWidth = ((getWindowWidth() - 20.0f)*(newTime / 60.0f));
	if (timeBarWidth < 0.0f) timeBarWidth = 0.0f;
	gl::color(Color(1.0f, 1.0f, 1.0f));
	gl::drawSolidRect(Rectf(15.0f, 15.0f, getWindowWidth() - 15.0f, 55.0f));
	gl::color(Color(0.3f, 0.7f, 1.0f));
	if (newTime < 15.0f) gl::color(Color(1.0f, 0.3f, 0.3f));
	else if (newTime < 25.0f) gl::color(Color(0.8f, 0.7f, 0.0f));
	gl::drawSolidRect(Rectf(20.0f, 20.0f, timeBarWidth - 5.0f, 50.0f));

	gl::disableAlphaBlending();
}

void Stage::drawLogo(){

	float xx = (getWindowWidth() / 2) - (logoTexture.getWidth()*0.88f / 2);
	float yy = 250.0f;

	gl::popMatrices();
	gl::setMatricesWindow(getWindowSize(), true);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Area srcArea(0, 0, logoTexture.getWidth(), logoTexture.getHeight());
	Rectf destRect(xx, yy, xx + logoTexture.getWidth()*0.88f, yy + logoTexture.getHeight()*0.88f);
	gl::draw(logoTexture, srcArea, destRect);
}

void Stage::drawHand(){
	Vec2f handIn2D = Vec2f(getWindowWidth()*(getHandPosition().x + 0.5) / 1.0f, -1.0f*getWindowHeight()*(getHandPosition().y - 0.75) / 1.5f);

	//draw hand
	gl::color(1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gl::draw(handTexture, handIn2D);
	glDisable(GL_BLEND);
}