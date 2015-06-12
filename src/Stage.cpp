#include "Stage.h"

using namespace ci;
using namespace ci::app;

void Stage::setup(){
	ci::Surface8u surface(loadImage(loadAsset("Stage01-2.PNG")));
	stageTexture = gl::Texture(surface);

	ci::Surface8u surfaceHand(loadImage(loadAsset("hand.png")));
	handTexture = gl::Texture(surfaceHand);

	worldTexture = gl::Texture(loadImage(loadAsset("football.jpg")));
	

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
	else if (stageNum > 3) stageNum = 0;
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


		float buttonSize = 50.0f;
		float handSize = 55.0f;
		Vec2f buttonPos = Vec2f(getWindowWidth() / 2, getWindowHeight() / 2 + 100.0f);
		Vec2f handIn2D = Vec2f(getWindowWidth()*(getHandPosition().x + 0.5) / 1.0f, -1.0f*getWindowHeight()*(getHandPosition().y - 0.75) / 1.5f);

		//draw button
		if (abs(handIn2D.x - buttonPos.x) < (handSize + buttonSize) / 2 && abs(handIn2D.y - buttonPos.y) < (handSize + buttonSize) / 2){
			gl::color(Color(0.1f, 0.0f, 1.0f));
			timer++;
		}
		else { timer = 0; }

		gl::drawSolidCircle(buttonPos, buttonSize);
		gl::color(1.0f, 1.0f, 1.0f);

		//draw hand
		gl::color(1.0f, 1.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl::draw(handTexture, handIn2D);
		glDisable(GL_BLEND);

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

		gl::color(Color(0.0f, 0.0f, 1.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));

		gl::enableAlphaBlending();
		//draw score
		gl::drawString("Good Job !!\n Your score is \n"+std::to_string(score), Vec2f(30.0f, 30.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 80.0f));
		gl::disableAlphaBlending();

		
	}
}

void Stage::drawTime(){

	gl::enableAlphaBlending();

	//draw score
	gl::drawString(std::to_string(score), Vec2f(30.0f, 30.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 40.0f));

	//draw time
	float newTime = 60.0f - (timer / 60.0f);
	float timeBarWidth = ((getWindowWidth() - 20.0f)*(newTime / 60.0f));
	if (timeBarWidth < 0.0f) timeBarWidth = 0.0f;
	gl::color(Color(1.0f, 1.0f, 1.0f));
	gl::drawSolidRect(Rectf(10.0f, 10.0f, getWindowWidth() - 10.0f, 40.0f));
	gl::color(Color(0.0f, 0.0f, 1.0f));
	gl::drawSolidRect(Rectf(10.0f, 10.0f, timeBarWidth + 10.0f, 40.0f));

	gl::disableAlphaBlending();
}