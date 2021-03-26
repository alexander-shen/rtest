// From https://www.jstatsoft.org/article/view/v008i18 (Marsaglia-Tsang-Wang)
// Removed the main() program and replaced double by long double
// inverted: ks1(n.d) = 1-K(n,d) = Pr[deviation_n >= d]

/*
 The C program to compute Kolmogorov's distribution

             K(n,d) = Prob(D_n < d),         where

      D_n = max(x_1-0/n,x_2-1/n...,x_n-(n-1)/n,1/n-x_1,2/n-x_2,...,n/n-x_n)

    with  x_1<x_2,...<x_n  a purported set of n independent uniform [0,1)
    random variables sorted into increasing order.
    See G. Marsaglia, Wai Wan Tsang and Jingbo Wong, J.Stat.Software.
*/

long double ks1 (int n, long double d);

