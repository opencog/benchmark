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

	exit(1);
}
