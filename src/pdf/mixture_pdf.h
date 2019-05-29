#ifndef MIXTURE_PDFH
#define MIXTURE_PDFH

class mixture_pdf : public pdf {
 public:
  mixture_pdf(pdf *p0, pdf *p1) {
    p[0] = p0;
    p[1] = p1;
  }

  virtual float value(const vec3 &direction) const {
    return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
  }

  virtual vec3 generate() const {
    if (RFG() < 0.5f)
      return p[0]->generate();
    else
      return p[1]->generate();
  }

  pdf *p[2];
};

#endif  // !MIXTURE_PDFH
