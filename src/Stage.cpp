#include "Stage.h"

using namespace ci;
using namespace ci::app;

void Stage::setup(){
	ci::Surface8u surface(loadImage(loadAsset("Stage01.PNG")));
	stageTexture = gl::Texture(surface);

	ci::Surface8u surfaceHand(loadImage(loadAsset("hand.png")));
	handTexture = gl::Texture(surfaceHand);
}

void Stage::nextStage(){
	timer = 0;
	stageNum++;
	if (stageNum == 0){
		ci::Surface8u surface(loadImage(loadAsset("Stage01.PNG")));
		stageTexture = gl::Texture(surface);
	}
	else if (stageNum == 1) score = 0;
	else if (stageNum > 2) stageNum = 0;
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

void Stage::updateStage(Vec3f pos, ci::CameraPersp cam){
	setHandPosition(pos);
	sCamera = cam;
	//update stage 0
	if (stageNum == 0){
		if (timer > 120) nextStage();
	}
	//update stage 1
	else if (stageNum == 1){
		timer++;
		if (timer > 61.0f*60.0f) nextStage();
	}
	//update stage 2
	else if (stageNum == 2){
		timer++;
		if (timer > 10.0f*60.0f) nextStage();
	}
	/*//update stage 3
	else if (stageNum == 3){

	}
	//update stage 4
	else if (stageNum == 4){

	}*/
}

void Stage::drawStage(){
	//draw stage 0
	if (stageNum == 0){
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

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
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

		//draw BG
		gl::color(Color(0.0f, 1.0f, 0.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));

	}
	//draw stage 2
	else if (stageNum == 2){
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

		gl::color(Color(0.0f, 0.0f, 1.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));

		gl::enableAlphaBlending();
		//draw score
		gl::drawString("Good Job !!\n Your score is \n"+std::to_string(score), Vec2f(30.0f, 30.0f), Color(1.0f, 1.0f, 1.0f), ci::Font("Tahoma", 80.0f));
		gl::disableAlphaBlending();

		
	}
	/*//draw stage 3
	else if (stageNum == 3){

	}
	//draw stage 4
	else if (stageNum == 4){

	}*/
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