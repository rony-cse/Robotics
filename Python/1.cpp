//ID:174067
// c++ code for sub of subset,
//no user input***, uses data from example shown in Report.

#include<bits/stdc++.h>
using namespace std;
#define ll             long long
#define pb              push_back
#define vi              vector<int>
#define mk(arr,n,type)  type *arr=new type[n];
#define test_case(x)    int x; cin>>x; while(x--)
#define w(a)            int w=a+1; while(w-->1)
#define f(a,n)          for(int f=a;f<n;f++) //a to n-1 loop
#define zz              ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
vector<int> x;
int w[999] = {3, 4, 5, 6}, m, n;

void SumofSubset(int s, int k, int r)
{
	if (k > n) return ;
	if (s + w[k] == m)
	{	//cout << "paici \n";
		x.push_back(w[k]);
		for (int i = 0; i < x.size(); i++)
			cout << x[i] << " "; cout << endl;
		x.pop_back();
		return;
	}
	if (s + w[k] < m)
	{
		x.push_back(w[k]);
		SumofSubset(s + w[k], k + 1, r - w[k]);
		x.pop_back();
	}
	if (s + r >= m)
		SumofSubset(s, k + 1, r - w[k]);
}

int  main()
{
	int i, r = 0;
	n = 4; m = 9;
	r = 18;
	SumofSubset(0, 1, r);
	return 0;
}
