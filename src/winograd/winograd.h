#ifndef WINOGRAD_H_
#define WINOGRAD_H_

#include "../simplegraph.h"

class Winograd {
  public:
    static SimpleGraph<double> Multiply(const SimpleGraph<double>& g,
                                        const SimpleGraph<double>& h);
};

#endif // WINOGRAD_H_
