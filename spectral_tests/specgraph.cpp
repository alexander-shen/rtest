// written by Yann OLLIVIER http://www.yann-ollivier.org/specgraph/specgraph

#include "specgraph.h"
#include "rand.h"
#include<iostream>
#include<cmath>

using namespace std;

void AddEdge(Graph& G,int i,int j)
{
 G.Ed.push_back(Edge(i,j));G.deg[i]++;
}

void Graph::AddSymEdge(int i,int j)
{
 AddEdge(*this,i,j);
 // Note that loops are stored twice, to be consistent (locality of
 // degree, #edges=.5sum of degrees)
 // if(j!=i){
 AddEdge(*this,j,i);
 // }
}

int Graph::NEdges()const
{
 int s=Ed.size();
 if(s%2)return -1;//Should never happen with unoriented graphs
 return s/2;
 /* int s=Ed.size();int i;
		for(i=0;i<Ed.size();i++)if(Ed[i].s==Ed[i].g)s++;
		if(s%2)return -1;//Should never happen with unoriented graphs
		return s/2;*/
}

int Graph::outdeg(int i)
{
 return deg[i];
}

TVector Graph::Aver(const TVector& v)
{
 TVector av(n,0);
 vector<Edge>::const_iterator i;
 for(i=Ed.begin();i<Ed.end();i++)
	 av[i->s]+=v[i->g]/deg[i->s];
 return av;
}

TVector Graph::SemiAver(const TVector& v)
{
 TVector av(v);
 TVector::iterator j;
 for(j=av.begin();j!=av.end();j++)(*j)*=.5;
 vector<Edge>::const_iterator i;
 for(i=Ed.begin();i<Ed.end();i++)
	 av[i->s]+=.5*v[i->g]/deg[i->s];
 return av;
}

TVector Graph::Laplacian(const TVector& v)
{
 TVector av=Aver(v);
 int i;
 for(i=0;i<n;i++)
	 av[i]=v[i]-av[i];
 return av;
}

long double Graph::DotProd(const TVector& v1,const TVector& v2)
{
 long double s=0;
 vector<int>::iterator id;vector<long double>::const_iterator iv1,iv2;
 for(id=deg.begin(),iv1=v1.begin(),iv2=v2.begin();id!=deg.end();id++,iv1++,iv2++)
	 s+=(*id)*(*iv1)*(*iv2);
 return s;
}

void Graph::Normalize(TVector& v)
{
 int i;long double m=0,s=0;
 int mass=Ed.size();
 if(!mass)return;
 vector<long double>::iterator iv;vector<int>::iterator id;
 for(iv=v.begin(),id=deg.begin();id!=deg.end();id++,iv++){
	 m+=(*iv)*(*id);s+=(*iv)*(*iv)*(*id);
 }
 m/=mass;
 s=sqrt(s-mass*m*m);
 if(s==0)s=1;
 for(iv=v.begin();iv!=v.end();iv++)
	 *iv=(*iv-m)/s;
}

void Graph::SpecRad( long double gap, long double error, long iterations,
         long double *low, long double *high) // modified by AR, AS
{
  if(n<=1){*low=4;*high=4;} 
  TVector v(n,0);long double lambda=0,alambda=1;
  int i;vector<long double>lognorm2(1,0.);
  for(i=0;i<n;i++)v[i]=alea_norm()/sqrt((long double)deg[i]);
  Normalize(v);
  int c;
  for(c=1; (c<iterations)&&(alambda-lambda>gap); c++){
    v=Aver(v);lognorm2.push_back(lognorm2[c-1]+log(DotProd(v,v)));
    Normalize(v);
    if(!(c%3)){
      long double x;
      int c0=c/3;
      lambda=exp((lognorm2[c]-lognorm2[c0])/2./(c-c0));
      x=(lognorm2[c]-lognorm2[c0])-2*(lognorm2[(c+c0)/2]-lognorm2[c0]);
      if(x>0)x=exp(x)-1;else x=0;//x should always be >=0, but roundoff errors
      x=1+sqrt(n-1)/error*sqrt(x);
      alambda=exp((1./(c-c0))*(lognorm2[(c+c0)/2]-lognorm2[c0]+log(x)));
      // lambda <= specrad <= alambda;  error: alambda-lambda;
    } 
  }   
  *low=lambda;
  *high=alambda;
}

/* original version from OLLIVIER
long double Graph::SpecRad()
{
 if(n<=1)return 4;
 TVector v(n,0);long double lambda=0,alambda=1;
 int i;
 for(i=0;i<n;i++)v[i]=alea_norm()/sqrt((long double)deg[i]);
 Normalize(v);
 int c;
 for(c=1;c<specgraph_maxiter&&
		 alambda-lambda>specgraph_stopthreshold
		 ;c++){
	 cout<<"Iteration "<<c<<endl;
	 long double n1,n2,n3,n4,x;
	 v=Aver(v);n1=DotProd(v,v);
	 v=Aver(v);n2=DotProd(v,v);c++;
	 v=Aver(v);n3=DotProd(v,v);c++;
	 v=Aver(v);n4=DotProd(v,v);c++;
	 Normalize(v);
	 lambda=sqrt(n4/n3);
	 //lambda=sqrt(n2/n1);
	 x=n4-4*n3*n1+6*n2*n1*n1-3*n1*n1*n1*n1;
	 //x=n2-n1*n1;
	 if(x<0)x=0;//Should always be >=0
	 x=n1+sqrt(sqrt(n-2)/specgraph_probaerror*sqrt(x));
	 //x=n1+sqrt(n-2)/specgraph_probaerror*sqrt(x);
	 alambda=sqrt(x);
	 cout<<lambda<<" <= specrad <= "<<alambda<<endl;
	 cout<<"Error:"<<alambda-lambda<<endl;
 }
 return lambda;
}

long double Graph::Lambda1()
{//We just compute 1-2*SpecRad((Id+M)/2)
 if(n<=1)return 4;
 TVector v(n,0);long double lambda=0,alambda=1;
 int i;vector<long double>lognorm2(1,0.);
 for(i=0;i<n;i++)v[i]=alea_norm()/sqrt((long double)deg[i]);
 Normalize(v);
 int c;
 for(c=1;c<specgraph_maxiter&&
		 alambda-lambda>specgraph_stopthreshold/2.
		 ;c++){
	 v=SemiAver(v);lognorm2.push_back(lognorm2[c-1]+log(DotProd(v,v)));
	 Normalize(v);
	 if(!(c%3)){long double x;
		 int c0=c/3;
		 lambda=exp((lognorm2[c]-lognorm2[c0])/2./(c-c0));
		 cout<<"Iteration "<<c<<endl;
		 x=(lognorm2[c]-lognorm2[c0])-2*(lognorm2[(c+c0)/2]-lognorm2[c0]);
		 if(x>0)x=exp(x)-1;else x=0;//x should always be >=0, but roundoff errors
		 x=1+sqrt(n-1)/specgraph_probaerror*sqrt(x);
		 alambda=exp((1./(c-c0))*(lognorm2[(c+c0)/2]-lognorm2[c0]+log(x)));
		 cout<<2.-2.*alambda<<" <= lambda1 <= "<<2.-2.*lambda<<endl;
		 cout<<"Error:"<<2.*alambda-2.*lambda<<endl;
	 }
	 cout<<endl;
 }
 return lambda;
}
*/


void markconnectedcomp(const Graph& G, int i, vector<bool>& v)
{
 v[i]=true;
 int j;
 for(j=0;j<G.NEdges()*2;j++){
	 if(G.Ed[j].s==i&&!v[G.Ed[j].g])markconnectedcomp(G,G.Ed[j].g,v);
 }
 return;
}


Graph ConnectedComp(const Graph& G, int& i)
	//ASSUMES G IS SYMMETRIC
{
 if(i<0||i>=G.n){i=-1;return Graph(0);}
 vector<bool> v(G.n,false);
 markconnectedcomp(G,i,v);
 vector<int> convert(G.n,-1);int c=0;
 int j;
 for(j=0;j<G.n;j++)if(v[j])convert[j]=c++;
 Graph H(c);
 for(j=0;j<G.NEdges()*2;j++)
	 if(v[G.Ed[j].s]&&v[G.Ed[j].g])AddEdge(H,convert[G.Ed[j].s],convert[G.Ed[j].g]);
 i=convert[i];
 return H;
}

