//
//
// Explore poointer casting.

#include <opencog/atoms/base/Node.h>

using namespace opencog;

int main()
{
	Handle h = createNode(CONCEPT_NODE, "foobar");

	asm("int $3");
	ValuePtr vp = h; // ValueCast(h);
	asm("int $3");
	asm("int $3");

	ValuePtr fvp = createFloatValue(3.0);
	asm("int $3");
	asm("int $3");
	asm("int $3");
	vp = fvp;

	asm("int $3");
	asm("int $3");
	asm("int $3");
	asm("int $3");

	h = HandleCast(vp);
	asm("int $3");
	exit(1);
}
