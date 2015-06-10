#include "Stage.h"
#include "cinder/gl/TextureFont.h"

using namespace ci;
using namespace ci::app;

void Stage::nextStage(){
	timer = 0;
	stageNum++;
	if (stageNum == 1) score = 0;
	if (stageNum > 2) stageNum = 0;
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
		if (timer > 2.0f*60.0f) nextStage();
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

		gl::color(Color(1.0f, 0.0f, 0.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));

		// Set up 3D view
		gl::setMatrices(sCamera);
		gl::color(Color(0.0f, 1.0f, 1.0f));

		float buttonSize = 0.1f;
		float handSize = 0.05f;

		gl::drawSphere(Vec3f(0.0f, 0.2f, -1.0f), buttonSize, 10);

		gl::color(Color(1.0f, 1.0f, 1.0f));
		
		Vec3f hand = getHandPosition();
		if (abs(hand.x) < (handSize + buttonSize) / 2 && abs(hand.y - 0.2f) < (handSize + buttonSize) / 2){
			gl::color(Color(0.1f, 0.0f, 1.0f));
			timer++;
		}
		else{
			timer = 0;
		}

		gl::drawSphere(getHandPosition(), handSize, 10);

		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
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