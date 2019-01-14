#ifndef PDFH
#define PDFH

#include "vec3.h"

class pdf {
	public:
		virtual float value(const vec3& direction) const = 0;
		virtual vec3 generate() const = 0;
};

#endif // !PDFH
