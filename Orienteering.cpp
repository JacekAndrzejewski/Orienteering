#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

struct point
{
	double x,y,w;
};

typedef vector <int> cycle;

double pointDistance(point a,point b)
{
	return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

class Way
{
	private:
		double length,value;
		cycle my_cycle;
	public:
		Way()
		{
			length=value=0;
		}
		double getLength()
		{
			return length;
		}
		double getValue()
		{
			return value;
		}
		cycle getCycle()
		{
			return my_cycle;
		}
		void setCycle(cycle cy)
		{
			my_cycle=cy;
		}
		void calcDist(vector<vector<double> > dist)
		{
			length=0;
			if(my_cycle.size()<2)
				return;

			for(int i=0;i<my_cycle.size()-1;i++)
				length+=dist[my_cycle[i]][my_cycle[i+1]];

			length+=dist[my_cycle[my_cycle.size()-1]][my_cycle[0]];
		}
		void calcScore(point arrp[])
		{
			value=0;
			if(my_cycle.size()==0)
				return;

			for(unsigned int i=0;i<my_cycle.size();i++)
				value+=arrp[my_cycle[i]].w;
		}
};

struct individual
{
	Way way;
	double score;
};

void showIndividual(individual os)
{
	cout<<"Score: "<<os.way.getValue();
	cout<<" Dist: "<<os.way.getLength();
	cout<<" Cycle: ";
	cycle cy=os.way.getCycle();

	for(int i=0;i<cy.size();i++)
		cout<<cy[i]<<",";
	cout<<" Score: "<<os.score;
}

individual fitness(individual os,double limit)
{
	double score;
	double d=os.way.getLength();
	double wayValue=os.way.getValue();

	if(d>limit)
	{
		score=0;
	}
	else
		score=wayValue;

	os.score=score;

	return os;
}

individual randIndividual(int Points, double limit, vector<vector<double> > distances)
{
	double dist=0;
	int used[Points]={0};
	individual ostemp;
	cycle ctemp;
	int randomNum;

	for(int i=0;dist<limit;i++)
	{
		do
		{
		randomNum=rand()%Points;
		}while(used[randomNum]==1);

		used[randomNum]=1;
		if(i>0)
			dist=dist-distances[ctemp[i-1]][ctemp[0]]+distances[ctemp[i-1]][randomNum]+distances[randomNum][ctemp[0]];
		ctemp.push_back(randomNum);
	}
	ctemp.erase(ctemp.begin()+ctemp.size()-1);

	ostemp.way.setCycle(ctemp);
	return ostemp;
}

void removeRepeating(individual *os,int Points)
{
	vector <int> cy=(*os).way.getCycle();
	int arr[Points]={0};
	int del[cy.size()]={0};

	for(int i=0;i<cy.size();i++)
		if(arr[cy[i]]==1)
			del[i]=1;
		else
			arr[cy[i]]=1;

	for(int i=cy.size()-1;i>=0;i--)
		if(del[i]==1)
			cy.erase(cy.begin()+i);
	(*os).way.setCycle(cy);
}

void cross(individual* os1, individual* os2, int Points) 
{
	int p1,p2,k1,k2;
	cycle c1,c2;
	c1=(*os1).way.getCycle();
	c2=(*os2).way.getCycle();

	if(c1.size()<2 || c2.size()<2)
		return;

	p1=rand()%c1.size();
	k1=rand()%c1.size();
	if(p1>k1)
		swap(p1,k1);

	p2=rand()%c2.size();
	k2=rand()%c2.size();
	if(p2>k2)
		swap(p2,k2);

	cycle tmp1,tmp2;

	int used1[Points]={0}; //used in tmp1
	int used2[Points]={0}; //used in tmp2

	//calculating tmp1
	for(int i=0;i<=p1;i++)
	{
		if(used1[c1[i]]==0)
			tmp1.push_back(c1[i]);
		used1[c1[i]]=1;

	}

	for(int i=p2;i<=k2;i++)
	{
		if(used1[c2[i]]==0)
			tmp1.push_back(c2[i]);
		used1[c2[i]]=1;

	}

	for(int i=k1;i<c1.size();i++)
	{
		if(used1[c1[i]]==0)
			tmp1.push_back(c1[i]);
		used1[c1[i]]=1;

	}

	//calculating tmp2
	for(int i=0;i<=p2;i++)
	{
		if(used2[c2[i]]==0)
			tmp2.push_back(c2[i]);
		used2[c2[i]]=1;

	}

	for(int i=p1;i<=k1;i++)
	{
		if(used2[c1[i]]==0)
			tmp2.push_back(c1[i]);
		used1[c1[i]]=1;

	}

	for(int i=k2;i<c2.size();i++)
	{
		if(used2[c2[i]]==0)
			tmp2.push_back(c2[i]);
		used1[c2[i]]=1;

	}

	(*os1).way.setCycle(tmp1);
	(*os2).way.setCycle(tmp2);
}

void mutation(individual *os,int Points,vector<vector<double> >dist)
{
	vector <int> cy=os->way.getCycle();
	int used [Points]={0};

	for(unsigned int i=0;i<cy.size();i++)
		used[cy[i]]=1;

	int los;

	do
	{
		los=rand()%Points;
	}while(used[los]==1);

	double mindist=dist[los][cy[0]]+dist[los][cy[cy.size()-1]];
	double minNum=0;

	for(unsigned int i=1;i<cy.size();i++)
	{
		if(dist[los][cy[i]]+dist[los][cy[i-1]]>mindist)
		{
			mindist=dist[los][cy[i]]+dist[los][cy[cy.size()-1]];
			minNum=i;
		}
	}

	cy.insert(cy.begin()+minNum,los);

	os->way.setCycle(cy);

}

void save(individual a, int limit)
{
	fstream file;
	file.open("solutions.txt", ios::out | ios::app);

	file<<endl<<"Limit: "<<limit<<endl;
	file<<"Distance: "<<a.way.getLength()<<endl;
	file<<"Wynik: "<<a.way.getValue()<<endl;
	file<<"Way: ";
	vector <int> v1=a.way.getCycle();

	for(unsigned int i=0;i<v1.size();i++)
		file<<v1[i]<<",";
	file<<endl;

	file.close();
}

void optimize(individual *oso, vector<vector<double> > dys)
{
	vector <int> cy=(*oso).way.getCycle();

	if(cy.size()<3)
		return;

	int r=cy.size();

	bool dal=true;
	while(dal)
	{
		dal=false;
		for(unsigned int i=1;i<=r;i++)
		{
			if(dys[cy[i%r]][cy[(i+2)%r]]+dys[cy[(i-1)%r]][cy[(i+1)%r]]<dys[cy[(i+1)%r]][cy[(i+2)%r]]+dys[cy[(i-1)%r]][cy[i%r]])
			{
				swap(cy[i%r],cy[(i+1)%r]);
				dal=true;
			}
		}
	}
	oso->way.setCycle(cy);

}

void sort(vector <individual> *arr)
{
	for(unsigned int i=0;i<arr->size();i++)
		for(unsigned int j=i+1;j<arr->size();j++)
			if((*arr)[i].score<(*arr)[j].score)
				swap((*arr)[i],(*arr)[j]);
}

void permut(individual *os1)
{
	vector <int> vt=(*os1).way.getCycle();

	int los;

	for(int i=vt.size()-1;i>0;i--)
	{
		los=rand()%i;
		swap(vt[i],vt[los]);
	}
	(*os1).way.setCycle(vt);
}

int main()
{
	srand(time(NULL));
	int n; //ilosc wierzcholkow grafu
	int generationSize=100; //ilosc individualow w pokoleniu
	double Tmax; //limit distances
	int ilec=100;
	int szansamut,szansatop,powt,szansapermut,ileKrzyz,ilePok;
	ileKrzyz=generationSize/4;
	powt=200;
	szansamut=35;
	szansatop=80;
	szansapermut=30;
	ilePok=1000;
	fstream file;
	file.open("data.txt", ios::in);

	unsigned long long int psz=0;

	file>>n;
	file>>Tmax;

	point points[n]; //arrlica z wierzcholkami
	vector<vector<double> > distances(n,vector<double>(n)); //wektor przechowujacy distances miedzy pointami

	for(int i=0;i<n;i++)
	{
		file>>points[i].x;
		file>>points[i].y;
		file>>points[i].w;
	}

	file.close();

	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			distances[i][j]=(int)pointDistance(points[i],points[j]);

	individual top,poptop,losos;
	int nrzmiany;

	vector <individual> generation;

for(int k=0;k<50;k++)
{
	psz=0;

	for(int i=0;i<generationSize;i++)
	{
		top=randIndividual(n,Tmax,distances);
		generation.push_back(top);
	}

	for(int i=0;i<generationSize;i++)
		optimize(&(generation[i]),distances);

	top=randIndividual(n,Tmax,distances);
	top.way.calcDist(distances);
	top.way.calcScore(points);
	top=fitness(top,Tmax);

	int randomFirst;

	while(psz<ilePok)
	{
		poptop=top;

		for(int i=0;i<generationSize;i++)
		{
			randomFirst=rand()%100;
			if(randomFirst<szansamut)
				mutation(&(generation[i]),n,distances);

			randomFirst=rand()%100;
			if(randomFirst<szansapermut)
				permut(&(generation[i]));
		}

		for(int i=0;i<generationSize;i++)
		{
			removeRepeating(&(generation[i]),n);
			optimize(&(generation[i]),distances);
			generation[i].way.calcDist(distances);
			generation[i].way.calcScore(points);
			generation[i]=fitness(generation[i],Tmax);
			do
			{
				if(generation[i].score==0)
					generation[i]=randIndividual(n,Tmax,distances);
				generation[i].way.calcDist(distances);
				generation[i].way.calcScore(points);
				generation[i]=fitness(generation[i],Tmax);
			}while(generation[i].score==0);
		}

		sort(&generation);
		if(generation[0].score>top.score)
				top=generation[0];

		randomFirst=rand()%100;
		if(randomFirst<szansatop)
		{
			generation[generationSize-1]=top;
			mutation(&(generation[generationSize-1]),n,distances);
			optimize(&(generation[generationSize-1]),distances);
		}

		for(int i=0;i<ileKrzyz;i++)
		{
			randomFirst=rand()%generationSize;
			if(i!=randomFirst)
			cross(&(generation[i]),&(generation[randomFirst]),n);
		}

		if(poptop.score!=top.score || psz%ilec==0)
		{
			double s=0;
			for(int i=0;i<generationSize;i++)
				s+=generation[i].score;
			s/=generationSize;

			cout<<psz<<". AvgF:"<<s<<" Top:";
			showIndividual(top);
			cout<<endl;
		}

		if(poptop.score!=top.score)
				nrzmiany=psz;

		if(psz-nrzmiany>powt)
		{
			cout<<"Za dlugo nie bylo zmian, losujemy od nowa!"<<endl;
			nrzmiany=psz;
			for(int i=0;i<generationSize-1;i++)
			{
				losos=randIndividual(n,Tmax,distances);
				generation[i]=losos;
			}
			generation[generationSize-1]=top;
			permut(&(generation[generationSize-1]));
			optimize(&(generation[generationSize-1]),distances);
		}
		psz++;
	}

cout<<endl<<"Zapis"<<endl<<endl;
save(top,Tmax);
}

int koniec;
cout<<"Koniec"<<endl;
cin>>koniec;

return 0;
}
