#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

struct punkt
{
	double x,y,w;
};

typedef vector <int> cycle;

double punktDystans(punkt a,punkt b)
{
	return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

class Droga
{
	private:
		double dlugosc,wartosc;
		cycle cykl;
	public:
		Droga()
		{
			dlugosc=wartosc=0;
		}
		double getDlugosc()
		{
			return dlugosc;
		}
		double getWartosc()
		{
			return wartosc;
		}
		cycle getCykl()
		{
			return cykl;
		}
		void setCykl(cycle cy)
		{
			cykl=cy;
		}
		void liczDlu(vector<vector<double> > dyst)
		{
			dlugosc=0;
			if(cykl.size()<2)
				return;

			for(int i=0;i<cykl.size()-1;i++)
				dlugosc+=dyst[cykl[i]][cykl[i+1]];

			dlugosc+=dyst[cykl[cykl.size()-1]][cykl[0]];
		}
		void liczWar(punkt tabp[])
		{
			wartosc=0;
			if(cykl.size()==0)
				return;

			for(unsigned int i=0;i<cykl.size();i++)
				wartosc+=tabp[cykl[i]].w;
		}
};

struct osobnik
{
	Droga droga;
	double wynik;
};

void pokazOsobnik(osobnik os)
{
	cout<<"War: "<<os.droga.getWartosc();
	cout<<" Dlu: "<<os.droga.getDlugosc();
	cout<<" Cy: ";
	cycle cy=os.droga.getCykl();

	for(int i=0;i<cy.size();i++)
		cout<<cy[i]<<",";
	cout<<" Wy: "<<os.wynik;
}

osobnik fitness(osobnik os,double limit)
{
	double wy;
	double d=os.droga.getDlugosc();
	double wa=os.droga.getWartosc();

	if(d>limit)
	{
		wy=0;
	}
	else
	wy=wa;

	os.wynik=wy;

	return os;
}

osobnik losOsobnik(int ilePkt, double limit, vector<vector<double> > odleglosci)
{
	double dyst=0;
	int uzyte[ilePkt]={0};
	osobnik ostemp;
	cycle ctemp;
	int losnr;

	for(int i=0;dyst<limit;i++)
	{
		do
		{
		losnr=rand()%ilePkt;
		}while(uzyte[losnr]==1);

		uzyte[losnr]=1;
		if(i>0)
			dyst=dyst-odleglosci[ctemp[i-1]][ctemp[0]]+odleglosci[ctemp[i-1]][losnr]+odleglosci[losnr][ctemp[0]];
		ctemp.push_back(losnr);
	}
	ctemp.erase(ctemp.begin()+ctemp.size()-1);

	ostemp.droga.setCykl(ctemp);
	return ostemp;
}

void usunPowt(osobnik *os,int ilePkt)
{
	vector <int> cy=(*os).droga.getCykl();
	int tab[ilePkt]={0};
	int usun[cy.size()]={0};

	for(int i=0;i<cy.size();i++)
		if(tab[cy[i]]==1)
			usun[i]=1;
		else
			tab[cy[i]]=1;

	for(int i=cy.size()-1;i>=0;i--)
		if(usun[i]==1)
			cy.erase(cy.begin()+i);
	(*os).droga.setCykl(cy);
}

void cross(osobnik* os1, osobnik* os2, int ilePkt) //ilePkt- ilo�� punkt�w grafu
{
	int p1,p2,k1,k2;
	cycle c1,c2;
	c1=(*os1).droga.getCykl();
	c2=(*os2).droga.getCykl();

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

	int uzyto1[ilePkt]={0}; //uzyto w tmp1
	int uzyto2[ilePkt]={0}; //uzyto w tmp2

	//wyliczenie tmp1
	for(int i=0;i<=p1;i++)
	{
		if(uzyto1[c1[i]]==0)
			tmp1.push_back(c1[i]);
		uzyto1[c1[i]]=1;

	}

	for(int i=p2;i<=k2;i++)
	{
		if(uzyto1[c2[i]]==0)
			tmp1.push_back(c2[i]);
		uzyto1[c2[i]]=1;

	}

	for(int i=k1;i<c1.size();i++)
	{
		if(uzyto1[c1[i]]==0)
			tmp1.push_back(c1[i]);
		uzyto1[c1[i]]=1;

	}

	//wyliczenie tmp2
	for(int i=0;i<=p2;i++)
	{
		if(uzyto2[c2[i]]==0)
			tmp2.push_back(c2[i]);
		uzyto2[c2[i]]=1;

	}

	for(int i=p1;i<=k1;i++)
	{
		if(uzyto2[c1[i]]==0)
			tmp2.push_back(c1[i]);
		uzyto1[c1[i]]=1;

	}

	for(int i=k2;i<c2.size();i++)
	{
		if(uzyto2[c2[i]]==0)
			tmp2.push_back(c2[i]);
		uzyto1[c2[i]]=1;

	}

	(*os1).droga.setCykl(tmp1);
	(*os2).droga.setCykl(tmp2);
}

void mutation(osobnik *os,int ilePkt,vector<vector<double> >dyst)
{
	vector <int> cy=os->droga.getCykl();
	int uzyte [ilePkt]={0};

	for(unsigned int i=0;i<cy.size();i++)
		uzyte[cy[i]]=1;

	int los;

	do
	{
		los=rand()%ilePkt;
	}while(uzyte[los]==1);

	double mindyst=dyst[los][cy[0]]+dyst[los][cy[cy.size()-1]];
	double minnr=0;

	for(unsigned int i=1;i<cy.size();i++)
	{
		if(dyst[los][cy[i]]+dyst[los][cy[i-1]]>mindyst)
		{
			mindyst=dyst[los][cy[i]]+dyst[los][cy[cy.size()-1]];
			minnr=i;
		}
	}

	cy.insert(cy.begin()+minnr,los);

	os->droga.setCykl(cy);

}

void zapis(osobnik a, int limit)
{
	fstream plik;
	plik.open("solutions.txt", ios::out | ios::app);

	plik<<endl<<"Limit: "<<limit<<endl;
	plik<<"Dystans: "<<a.droga.getDlugosc()<<endl;
	plik<<"Wynik: "<<a.droga.getWartosc()<<endl;
	plik<<"Droga: ";
	vector <int> v1=a.droga.getCykl();

	for(unsigned int i=0;i<v1.size();i++)
		plik<<v1[i]<<",";
	plik<<endl;

	plik.close();
}

void optimize(osobnik *oso, vector<vector<double> > dys)
{
	vector <int> cy=(*oso).droga.getCykl();

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
	oso->droga.setCykl(cy);

}

void sort(vector <osobnik> *tab)
{
	for(unsigned int i=0;i<tab->size();i++)
		for(unsigned int j=i+1;j<tab->size();j++)
			if((*tab)[i].wynik<(*tab)[j].wynik)
				swap((*tab)[i],(*tab)[j]);
}

void permut(osobnik *os1)
{
	vector <int> vt=(*os1).droga.getCykl();

	int los;

	for(int i=vt.size()-1;i>0;i--)
	{
		los=rand()%i;
		swap(vt[i],vt[los]);
	}
	(*os1).droga.setCykl(vt);
}

int main()
{
	srand(time(NULL));
	int n; //ilosc wierzcholkow grafu
	int rozPok=100; //ilosc osobnikow w pokoleniu
	double Tmax; //limit odleglosci
	int ilec=100;
	int szansamut,szansatop,powt,szansapermut,ileKrzyz,ilePok;
	ileKrzyz=rozPok/4;
	powt=200;
	szansamut=35;
	szansatop=80;
	szansapermut=30;
	ilePok=1000;
	fstream plik;
	plik.open("data.txt", ios::in);

	unsigned long long int psz=0;

	plik>>n;
	plik>>Tmax;

	punkt punkty[n]; //tablica z wierzcholkami
	vector<vector<double> > odleglosci(n,vector<double>(n)); //wektor przechowujacy odleglosci miedzy punktami

	for(int i=0;i<n;i++)
	{
		plik>>punkty[i].x;
		plik>>punkty[i].y;
		plik>>punkty[i].w;
	}

	plik.close();

	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			odleglosci[i][j]=(int)punktDystans(punkty[i],punkty[j]);

	osobnik top,poptop,losos;
	int nrzmiany;

	vector <osobnik> pokolenie;

for(int k=0;k<50;k++)
{
	psz=0;

	for(int i=0;i<rozPok;i++)
	{
		top=losOsobnik(n,Tmax,odleglosci);
		pokolenie.push_back(top);
	}

	for(int i=0;i<rozPok;i++)
		optimize(&(pokolenie[i]),odleglosci);

	top=losOsobnik(n,Tmax,odleglosci);
	top.droga.liczDlu(odleglosci);
	top.droga.liczWar(punkty);
	top=fitness(top,Tmax);

	int los1;

	while(psz<ilePok)
	{
		poptop=top;

		for(int i=0;i<rozPok;i++)
		{
			los1=rand()%100;
			if(los1<szansamut)
				mutation(&(pokolenie[i]),n,odleglosci);

			los1=rand()%100;
			if(los1<szansapermut)
				permut(&(pokolenie[i]));
		}

		for(int i=0;i<rozPok;i++)
		{
			usunPowt(&(pokolenie[i]),n);
			optimize(&(pokolenie[i]),odleglosci);
			pokolenie[i].droga.liczDlu(odleglosci);
			pokolenie[i].droga.liczWar(punkty);
			pokolenie[i]=fitness(pokolenie[i],Tmax);
			do
			{
				if(pokolenie[i].wynik==0)
					pokolenie[i]=losOsobnik(n,Tmax,odleglosci);
				pokolenie[i].droga.liczDlu(odleglosci);
				pokolenie[i].droga.liczWar(punkty);
				pokolenie[i]=fitness(pokolenie[i],Tmax);
			}while(pokolenie[i].wynik==0);
		}

		sort(&pokolenie);
		if(pokolenie[0].wynik>top.wynik)
				top=pokolenie[0];

		los1=rand()%100;
		if(los1<szansatop)
		{
			pokolenie[rozPok-1]=top;
			mutation(&(pokolenie[rozPok-1]),n,odleglosci);
			optimize(&(pokolenie[rozPok-1]),odleglosci);
		}

		for(int i=0;i<ileKrzyz;i++)
		{
			los1=rand()%rozPok;
			if(i!=los1)
			cross(&(pokolenie[i]),&(pokolenie[los1]),n);
		}

		if(poptop.wynik!=top.wynik || psz%ilec==0)
		{
			double s=0;
			for(int i=0;i<rozPok;i++)
				s+=pokolenie[i].wynik;
			s/=rozPok;

			cout<<psz<<". AvgF:"<<s<<" Top:";
			pokazOsobnik(top);
			cout<<endl;
		}

		if(poptop.wynik!=top.wynik)
				nrzmiany=psz;

		if(psz-nrzmiany>powt)
		{
			cout<<"Za dlugo nie bylo zmian, losujemy od nowa!"<<endl;
			nrzmiany=psz;
			for(int i=0;i<rozPok-1;i++)
			{
				losos=losOsobnik(n,Tmax,odleglosci);
				pokolenie[i]=losos;
			}
			pokolenie[rozPok-1]=top;
			permut(&(pokolenie[rozPok-1]));
			optimize(&(pokolenie[rozPok-1]),odleglosci);
		}
		psz++;
	}

cout<<endl<<"Zapis"<<endl<<endl;
zapis(top,Tmax);
}

int koniec;
cout<<"Koniec"<<endl;
cin>>koniec;

return 0;
}
