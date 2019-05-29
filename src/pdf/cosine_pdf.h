#ifndef COSINE_PDFH
#define COSINE_PDFH

#include "../onb.h"
#include "pdf.h"

class cosine_pdf : public pdf {
 public:
  cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

  virtual float value(const vec3& direction) const {
    float cosine = dot(unit_vector(direction), uvw.w());

    if (cosine > 0)
      return cosine / PI_F;
    else
      return 0;
  }

  virtual vec3 generate() const { return uvw.local(random_cosine_direction()); }

  onb uvw;
};

#endif  // !COSINE_PDFH
