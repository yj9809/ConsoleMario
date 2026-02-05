#include "Ground.h"

Ground::Ground(const Vector2& position)
	: super("G", position, Color::Green)
{
	sortingOrder = 0;
}
