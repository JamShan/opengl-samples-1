#include "Firework.h"
#include "Vector3.h"
#include "Vector4.h"
#include <utility>

struct Particle
{
	Vector3 currentPosition;
	Vector3 previousPosition;
	Vector3 direction;
	Vector4 color;

	GLuint life;
	float speed;
	float size;
};

void Firework::resetBuffer()
{
	//glBindVertexArray(updateArrays[readIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[readIndex]);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memset(ptr, 0, TOTAL_PARTICLES * sizeof(Particle));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Firework::Firework()
{
	timing = false;
	updateTime = 0;
	elapsedTime = 0;

	readIndex = 0;
	writeIndex = 1;

	glGenBuffers(2, buffers);
	glGenVertexArrays(2, updateArrays);
	glGenVertexArrays(2, renderArrays);

	for (size_t u = 0; u < 2; ++u)
	{
		glBindVertexArray(updateArrays[u]);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[u]);
		glBufferData(GL_ARRAY_BUFFER, TOTAL_PARTICLES * sizeof(Particle), 0, GL_DYNAMIC_COPY);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(sizeof(Vector3)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(sizeof(Vector3)* 2));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3)));

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3) + sizeof(Vector4)));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3) + sizeof(Vector4) + sizeof(float)));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3) + sizeof(Vector4) + (sizeof(float) * 2)));
	}

	for (size_t r = 0; r < 2; ++r)
	{
		glBindVertexArray(renderArrays[r]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[r]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>((sizeof(Vector3) * 3) + sizeof(Vector4) + (sizeof(float) * 2)));
	}
}

Firework::~Firework()
{
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(2, updateArrays);
	glDeleteVertexArrays(2, renderArrays);
}

void Firework::update()
{
	glBindVertexArray(updateArrays[readIndex]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffers[writeIndex]);

	glEnable(GL_RASTERIZER_DISCARD);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, TOTAL_PARTICLES);
	glEndTransformFeedback();

	glFlush();
	glDisable(GL_RASTERIZER_DISCARD);
	std::swap(readIndex, writeIndex);
}

void Firework::render()
{
	glBindVertexArray(renderArrays[readIndex]);
	glDrawArrays(GL_POINTS, 0, TOTAL_PARTICLES);
	glBindVertexArray(0);
}