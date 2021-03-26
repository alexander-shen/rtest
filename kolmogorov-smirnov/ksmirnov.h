// code adapted from R: http://ftp.stjude.org/pub/software/JUMPm/labeled/R-3.1.0/src/library/stats/src/ks.c
// scientific paper https://www.jstatsoft.org/v39/i11/paper for one-sample KS test 
// (comparison of different methods)

#include "../general/general.h"

// asymptotic computation
// taken from R library; approximates 1-pvalue in our sense
// when applied to sqrt((n*m)/(n+m))*deviation 
 
double pkstwo(double x);
/*
 * Compute
 *   \sum_{k=-\infty}^\infty (-1)^k e^{-2 k^2 x^2}
 *   = 1 + 2 \sum_{k=1}^\infty (-1)^k e^{-2 k^2 x^2}
 *   = \frac{\sqrt{2\pi}}{x} \sum_{k=1}^\infty \exp(-(2k-1)^2\pi^2/(8x^2))
 *
 * See e.g. J. Durbin (1973), Distribution Theory for Tests Based on the
 * Sample Distribution Function.  SIAM.
 *
 * The 'standard' series expansion obviously cannot be used close to 0;
 * we use the alternative series for x < 1, and a rather crude estimate
 * of the series remainder term in this case, in particular using that
 * ue^(-lu^2) \le e^(-lu^2 + u) \le e^(-(l-1)u^2 - u^2+u) \le e^(-(l-1))
 * provided that u and l are >= 1.
 *
 * (But note that for reasonable tolerances, one could simply take 0 as
 * the value for x < 0.2, and use the standard expansion otherwise.)
 *
 */

 
// Iterative exact computation using double reals
// taken from R library, produces 1-pvalue in our sense
long double psmirnov2x(long double deviation, int m, int n);
