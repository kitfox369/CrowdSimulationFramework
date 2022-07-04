#ifndef _SIAGNET_
#define _SIAGNET_

#include<iostream>
#include<vector>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<math.h>

class SiAgent {
private:
	int id;									//몇번째 사람인가
	glm::vec2 initPos;				//맨처음 Position	
	glm::vec2 goalPos;						//goal Position
	glm::vec2 velocity;
	float speed;

	/// initPos : 처음 시작하는 위치, goalPos : 마지막 목표 위치
	void initialize(int id, glm::vec2 initPos,float speed=rand()%5+5)
	{
		this->initPos = initPos;
		this->id = id;
		this->speed = speed;
		position = initPos;
	}

	void modelSetGoalPos(glm::vec2 goalPos)
	{
		this->goalPos = goalPos;
	}

	glm::vec2 position;		//position

public:
	float dir;
	// int reachGoalFN = 0;

	void doStep(float _deltaTime) {
		setPrefVelocity();
		position += velocity * _deltaTime* speed;
	}

	SiAgent() {};
	SiAgent(int id, glm::vec2& initPos)
	{
		initialize(id, initPos);
	};

	void addGoalPos(glm::vec2& goalPos)
	{
		modelSetGoalPos(goalPos);
	}

	void setPrefVelocity() {
		velocity = glm::normalize(goalPos - position);
	}

	//float calcuAngle(RVO::Vector2 dir)
	//{
	//	//std::cout << atan2(dir.y() - simPos.y(), dir.x() - simPos.x())*(180 / 3.1415f) << " ";
	//	return atan2(dir.y() ,dir.x())*(180 / 3.1415f);	
	//}

	void calcuAngle(int frameNum)
	{
		//std::cout << atan2(dir.y() - simPos.y(), dir.x() - simPos.x())*(180 / 3.1415f) << " ";
		//dir.push_back(atan2(0- position[frameNum+1].y, 0- position[frameNum+1].x)*(180 / 3.1415f));
		glm::vec2 next = glm::vec2(position.x, position.y);
		glm::vec2 current = glm::vec2(position.x, position.y);
		glm::vec2 d = current - next;
		d = glm::normalize(d);
		dir = atan2f(d.y, d.x) * (180.0f / 3.1415f);
		//std::cout << " "<< atan2f(d.y, d.x)*(180.0f / 3.1415f);
	}

	///position : 프레임 당 위치 값을 matrix로 변환한것
	void addPosition(glm::vec2& position)
	{
		this->position = position;
	}

	void addDirection(glm::vec2& pos)
	{
		//glm::vec2 temp = glm::vec2(velo.x(), velo.y());
		//glm::vec2 temp = 
		//this->dir.push_back(temp);
	}


	glm::mat4 getPosition()
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
	}

};

#endif