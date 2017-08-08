#pragma once

#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>
#include "Vector3.h"
#include <ctime>

#define TOTAL_PARTICLES 10000

class Firework
{
public:
	Firework();
	~Firework();

	void update();
	void render();

	bool getTiming() const;
	void setTiming(bool pmTiming);

	unsigned int getUpdateTime() const;
	void setUpdateTime(unsigned int pmUpdateTime);

	unsigned int getElapsedTime() const;
	void setElapsedTime(unsigned int pmElapsedTime);

	Vector3& getPosition();
	void setPosition(const Vector3& pmPosition);

private:
	bool timing;
	unsigned int updateTime;
	unsigned int elapsedTime;

	Vector3 position;
	size_t readIndex;
	size_t writeIndex;

	GLuint buffers[2];
	GLuint updateArrays[2];
	GLuint renderArrays[2];
};

inline bool Firework::getTiming() const
{
	return timing;
}

inline void Firework::setTiming(bool pmTiming)
{
	timing = pmTiming;
}

inline unsigned int Firework::getUpdateTime() const
{
	return updateTime;
}

inline void Firework::setUpdateTime(unsigned int pmUpdateTime)
{
	updateTime = pmUpdateTime;
}

inline unsigned int Firework::getElapsedTime() const
{
	return elapsedTime;
}

inline void Firework::setElapsedTime(unsigned int pmElapsedTime)
{
	elapsedTime = pmElapsedTime;
}

inline Vector3& Firework::getPosition()
{
	return position;
}

inline void Firework::setPosition(const Vector3& pmPosition)
{
	position = pmPosition;
}