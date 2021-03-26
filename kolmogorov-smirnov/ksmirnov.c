// code adapted from R: http://ftp.stjude.org/pub/software/JUMPm/labeled/R-3.1.0/src/library/stats/src/ks.c
// scientific paper https://www.jstatsoft.org/v39/i11/paper for one-sample KS test 
// (comparison of different methods)

#include "ksmirnov.h"

#define M_1_SQRT_2PI 0.398942280401432677939946059934

// asymptotic distribution
double pkstwo(double x)
{
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
    double tol = 1e-10;
    double new_val, old, s, w, z;
    int k, k_max;

    k_max = (int) sqrt(2 - log(tol));


	if(x < 1) {
	    z = - (M_PI_2 * M_PI_4) / (x * x);
	    w = log(x);
	    s = 0;
	    for(k = 1; k < k_max; k += 2) {
	    	s += exp(k * k * z - w);
	    }
	    x = s / M_1_SQRT_2PI;
	}
	else {
	    z = -2 * x * x;
	    s = -1;
	    k = 1;
	    old = 0;
	    new_val = 1;
	    while(fabs(old - new_val) > tol) {
	    	old = new_val;
	    	new_val += 2 * s * exp(z * k * k);
	    	s *= -1;
	    	k++;
	    }
	    x = new_val;
	}
	return x;
}

/* Iterative exact computation using double reals */
long double psmirnov2x(long double x, int m, int n)
{
    long double md, nd, q, *u, w;
    int i, j;

    if(m > n) {
      i = n; n = m; m = i;
    }
    md = (long double) m;
    nd = (long double) n;
    /*
       q has 0.5/mn added to ensure that rounding error doesn't
       turn an equality into an inequality, eg abs(1/2-4/5)>3/10

    */
    q = (0.5 + floor(x * md * nd - 1e-7)) / (md * nd);
    u = (long double *) malloc((n + 1) * sizeof(long double));

    for(j = 0; j <= n; j++) {
      u[j] = ((j / nd) > q) ? 0 : 1;
    }
    for(i = 1; i <= m; i++) {
      w = (long double)(i) / ((long double)(i + n));
      if((i / md) > q)
        u[0] = 0;
      else
        u[0] = w * u[0];
      for(j = 1; j <= n; j++) {
        if(fabs(i / md - j / nd) > q)
          u[j] = 0;
        else
          u[j] = w * u[j] + u[j - 1];
      }
    }
    long double result = u[n];
    free(u);
    return (result);
}
