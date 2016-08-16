#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <SDL.h>

struct Clock
{
public:
	inline float getElapsedTime() const { return SDL_GetTicks() - m_startTime; }

	inline float restart()
	{
		float now = SDL_GetTicks();
		float elapsed = now - m_startTime;
		m_startTime = now;

		return elapsed;
	}

private:
	float m_startTime = SDL_GetTicks();
};


#endif // CLOCK_H_INCLUDED
