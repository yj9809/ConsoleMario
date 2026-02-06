#include "Goal.h"

Goal::Goal(const Vector2& position)
	: super("G", position, Color::Green)
{
	sortingOrder = 0;
}
