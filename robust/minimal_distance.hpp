#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

using std::pair;
using std::vector;
using std::sort;
using std::make_pair;
using std::min;
using std::back_inserter;

template <class T>
struct Point {
    T x, y;
};

template <class T>
T squared_distance(Point<T> p0, Point<T> p1)
{
    return (p0.x - p1.x) * (p0.x - p1.x) + (p0.y - p1.y) * (p0.y - p1.y);
}

template <class T>
pair<vector<Point<T>>, vector<Point<T>>> preprocess(vector<Point<T>> & P)
{
    vector<Point<T>> X(P), Y(P);
    sort(X.begin(), X.end(), [](Point<T> & p0, Point<T> & p1) {return p0.x < p1.x || (p0.x == p1.x && p0.y < p1.y);});
    sort(Y.begin(), Y.end(), [](Point<T> & p0, Point<T> & p1) {return p0.y < p1.y || (p0.y == p1.y && p0.x < p1.x);});
    return make_pair(X, Y);
}

template <class T>
pair<Point<T>, Point<T>> argmin_distance(vector<Point<T>> & X, vector<Point<T>> & Y)
{
    int n = X.size();

    if(n < 2)
	exit(1);
    else if(n == 2)
	return make_pair(X[0], X[1]);
    else if(n == 3) {
	T d01 = squared_distance(X[0], X[1]);
	T d02 = squared_distance(X[0], X[2]);
	T d12 = squared_distance(X[1], X[2]);

	if(d01 <= d02 && d01 <= d12)
	    return make_pair(X[0], X[1]);
	else
	    return (d02 <= d12) ? make_pair(X[0], X[2]) : make_pair(X[1], X[2]);
    }

    vector<Point<T>> X0(X.begin(), X.begin() + n/2);
    vector<Point<T>> X1(X.begin() + n/2, X.end());
    vector<Point<T>> Y0, Y1;
    Y0.reserve(n/2);
    Y1.reserve(n - n/2);
    
    Point<T> p0 = X[n/2];
    for(Point<T> & p : Y) {
	if(p.x < p0.x || (p.x == p0.x && p.y < p0.y))
	    Y0.push_back(p);
	else
	    Y1.push_back(p);
    }

    auto argmin0 = argmin_distance(X0, Y0);
    auto argmin1 = argmin_distance(X1, Y1);
    T sd0 = squared_distance(argmin0.first, argmin0.second);
    T sd1 = squared_distance(argmin1.first, argmin1.second);

    auto argmin = (sd0 <= sd1) ? argmin0 : argmin1;
    T sd = min(sd0, sd1);

    vector<Point<T>> Y_d_band;
    auto it = back_inserter(Y_d_band);
    copy_if(Y.begin(), Y.end(), it, [&](Point<T> & p) {return (p.x - p0.x) * (p.x - p0.x) < sd;});

    int m = Y_d_band.size();    
    for(int i = 0; i < m; ++i) {
	for(int j = i + 1; j < i + 8 && j < m; ++j) {
	    T cur_sd = squared_distance(Y_d_band[i], Y_d_band[j]);
	    if(cur_sd < sd) {
		sd = cur_sd;
		argmin = make_pair(Y_d_band[i], Y_d_band[j]);
	    }
	}
    }

    return argmin;
}

template <class T>
pair<Point<T>, Point<T>> argmin_distance(vector<Point<T>> & P)
{
    auto XY = preprocess(P);
    return argmin_distance(XY.first, XY.second);
}

void argmin_distance_real (long double * x0, long double * y0, long double * x1, long double * y1, int n, long double * X, long double * Y)
{
    vector<Point<long double>> P(n);
    for(int k = 0; k < n; ++k) {
	P[k].x = X[k];
	P[k].y = Y[k];
    }

    auto argmin = argmin_distance(P);
    *x0 = argmin.first.x;
    *y0 = argmin.first.y;
    *x1 = argmin.second.x;
    *y1 = argmin.second.y;
}

template <class T>
pair<Point<T>, Point<T>> argmin_distance_bruteforce(vector<Point<T>> & P)
{
    int n = P.size();

    if(n < 2)
	exit(1);

    auto argmin = make_pair(P[0], P[1]);
    T sd = squared_distance(P[0], P[1]);

    for(int i = 0; i < n; ++i) {
	for(int j = i + 1; j < n; ++j) {
	    T cur_sd = squared_distance(P[i], P[j]);
	    if(cur_sd < sd) {
		sd = cur_sd;
		argmin = make_pair(P[i], P[j]);
	    }
	}
    }

    return argmin;
}
