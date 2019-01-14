#ifndef HITABLE_PDFH
#define HITABLE_PDFH

#include "pdf.h"
#include "hitable.h"

class hitable_pdf : public pdf {
	public:
		hitable_pdf(hitable *p, const vec3& origin) : ptr(p), o(origin) {}

		virtual float value(const vec3& direction) const {
			return ptr->pdf_value(o, direction);
		}

		virtual vec3 generate() const {
			return ptr->random(o);
		}

		vec3 o;
		hitable *ptr;
};

#endif // !HITABLE_PDFH
