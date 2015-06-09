#include "Stage.h"
#include "cinder/gl/TextureFont.h"

using namespace ci;
using namespace ci::app;

void Stage::nextStage(){
	stageNum++;
	if (stageNum == 1) score = 0;
	if (stageNum > 2) stageNum = 0;
}

int	Stage::getStage(){
	return stageNum;
}

void Stage::updateStage(){
	//update stage 0
	if (stageNum == 0){

	}
	//update stage 1
	else if (stageNum == 1){

	}
	//update stage 2
	else if (stageNum == 2){

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
	}
	//draw stage 1
	else if (stageNum == 1){
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);
		gl::color(Color(0.0f, 1.0f, 0.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));
		gl::drawString(std::to_string(score), Vec2f(10.0f, 10.0f), Color(1.0f, 1.0f, 1.0f));
	}
	//draw stage 2
	else if (stageNum == 2){
		gl::popMatrices();
		gl::setMatricesWindow(getWindowSize(), true);

		gl::color(Color(0.0f, 0.0f, 1.0f));
		gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));



		
	}
	/*//draw stage 3
	else if (stageNum == 3){

	}
	//draw stage 4
	else if (stageNum == 4){

	}*/
}