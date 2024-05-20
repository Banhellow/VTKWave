#include "MathUtils.h"

int MathUtils::Modulo(int base, int divider)
{
	return ((base % divider) + divider) % divider;
}
