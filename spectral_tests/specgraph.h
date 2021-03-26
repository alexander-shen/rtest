// written by Yann OLLIVIER http://www.yann-ollivier.org/specgraph/specgraph

#include<vector>

using namespace std;

class Edge{
public:
	int s,g;
	Edge(int sp,int gp){s=sp;g=gp;};
};

typedef vector<long double> TVector;

class Graph{
public:
	int n;
	vector<Edge> Ed;//Must be SYMMETRICAL, with loops present twice
	vector<int> deg;
	Graph(int np):deg(np,0){n=np;};
	void AddSymEdge(int,int);
	int NEdges()const;//Number of non-oriented edges
	int outdeg(int); //Outdegree
	long double DotProd(const TVector&,const TVector&);//wrt to measure=deg
	void Normalize(TVector&);//Normalize a fonction on the graph with respect to measure=deg: removes the constant term and sets norm to 1
	TVector Aver(const TVector&);//Simple random walk operator
		//Size of TVector should be n (no control)
	TVector Laplacian(const TVector&);//Harmonic Laplacian, =Id-Aver
		//Size of TVector should be n (no control)
	TVector SemiAver(const TVector&);//Lazy random walk, =Id/2+Aver/2
	// long double Lambda1();//Smallest non-zero eigenvalue of Laplacian
        void SpecRad( long double gap, long double error, long iterations,
                      long double *low, long double *high); // modified by AR, AS
        //Spectral radius of averaging operator on the orthogonal of the constants
	//Convention: returns 4 if G has <=1 point
};

Graph ConnectedComp(const Graph& G, int& i);//returns the (strongly) connected component of G containing vertex i. The new value of i is the index of the vertex in the new graph, -1 if empty
//ASSUMES G SYMMETRIC
