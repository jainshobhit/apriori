#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <queue>
#include <unistd.h>
#include <assert.h>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <map>
#include <algorithm>
using namespace std;

// A data structure for storing the transactions
struct transaction
{
	int transaction_id;
	vector<int> items;
};

vector<transaction> global_transactions;

//A function that will take the input from the file and populate the database
void take_input(char *inputfile)
{
	ifstream infile;
	infile.open(inputfile);
	string str;
	int count = 0;
	while(getline(infile, str)){
		transaction t;
		t.transaction_id = count;
		count++;

		std::istringstream buffer(str);
    	string item;
    	while(getline(buffer,item,' '))
        	t.items.push_back(atoi(item.c_str()));
    	global_transactions.push_back(t);
	}

}


//Function that will go through the first scan of the dataset
void firstscan3(vector<int> &frequency)
{
	for(int i=0;i<global_transactions.size();i++)
	{
		vector<int> tempItems = global_transactions[i].items;
		for(int j=0;j<tempItems.size();j++)
		{
			if(tempItems[j]<frequency.size())
				frequency[tempItems[j]]+=1;
			else
				frequency.push_back(1);
		}
	}
}

//Check the support after the first scan of all single item candidates
void check_support_first(vector<int> firstSet, float supportpercent, vector<int> &itemSet, vector<int> &freq)
{
	int support = int(supportpercent*global_transactions.size())/100;
	for(int i=0;i<firstSet.size();i++)
	{
		if(firstSet[i]>=support)
		{
			if(i==0)
				cout<<"for 0: "<<firstSet[i]<<endl;
			itemSet.push_back(i);
			freq.push_back(firstSet[i]);
		}
	}
}

//Check the support of candidates other than single item ones
void check_support(vector<vector<int>> &itemsets, vector<int> frequency, float supportpercent, vector<vector<int>> &newitemsets)
{
	newitemsets.clear();
	int support = int(supportpercent*global_transactions.size())/100;
	for(int i=0;i<itemsets.size();i++)
	{
		if(frequency[i]>support)
		{
			newitemsets.push_back(itemsets[i]);
		}
	}
}

//Generate the candidates for two item itemsets
void candidate_gen_second(vector<int> set, vector<int> freq, vector<vector<int>> &v)
{
	for(int i=0;i<set.size();i++)
	{		
		for(int j=i+1;j<set.size();j++)
		{
			vector<int> temp;
			temp.push_back(set[i]);
			temp.push_back(set[j]);
			v.push_back(temp);
		}
	}
}

//Generate the candidates for the next itemsets in general
void candidate_gen(vector<vector<int>> &input, vector<vector<int>> &output)
{
	output.clear();
	std::map<vector<int>, bool> mapmap;
	for(int i=0;i<input.size();i++)
		mapmap[input[i]]=1;
	for(int i=0;i<input.size();i++)
	{
		vector<int> temp1 = input[i];
		temp1.pop_back();
		//cout<<temp1[0]<<endl;
		for(int j=i+1;j<input.size();j++)
		{
			vector<int> temp2 = input[j];
			temp2.pop_back();
			//cout<<temp2[0]<<endl;
			if(temp1==temp2){
				//cout<<"yo"<<endl;
				vector<int> temp3 = input[i];
				temp3.push_back(input[j].back());
				//cout<<temp3[0]<<" "<<temp3[1]<<" "<<temp3[2]<<endl;
				vector<int> temp4;
				for(int k=0;k<temp3.size()-1;k++)
				{
					temp4 = temp3;
					temp4.erase(temp4.begin()+k);
					if(!mapmap[temp4])
						break;
					if(k==temp3.size()-2)
						output.push_back(temp3);		
				}
			}
			else
				break;
		}
	}
}

//Check if the genrated candidates are a part of the transaction database or not
bool checkcontained(vector<int> candidate, vector<int> transaction)
{
	if(transaction.size()<candidate.size())
		return false;
	int index=0;
	for(int i=0;i<candidate.size();i++)
	{
		int j;
		for(j=index;j<transaction.size();j++)
		{
			if(candidate[i]==transaction[j]){
				index = j+1;
				break;
			}
			else if(transaction[j]>candidate[i])
				return false;
		}

		if(j==transaction.size() and candidate[i]!=transaction[j-1])
			return false;
	}

	return true;
}

//Auxillary function similar to checkcontained function
int checkcontained_1(vector<int> candidate, vector<int> transaction, int inputindex)
{
	if(transaction.size()<candidate.size())
		return -1;
	int index=inputindex;
	for(int i=1;i<candidate.size();i++)
	{
		int j;
		for(j=index;j<transaction.size();j++)
		{
			if(candidate[i]==transaction[j]){
				index = j+1;
				break;
			}
			else if(transaction[j]>candidate[i])
				return -1;
		}

		if(j==transaction.size() and candidate[i]!=transaction[j-1])
			return -1;
	}

	return index;
}

//Check the frequency of the itemsets
void checkcount(vector<vector<int>> &input, vector<int> &freq)
{
	freq.clear();
	int size = input.size();
	//cout<<"size checkcount "<<size<<endl;
	freq.resize(size);
	for(int i=0;i<global_transactions.size();i++)
	{
		//cout<<"num: "<<i<<endl; 
		vector<int> transaction = global_transactions[i].items;
		for(int j=0;j<size;j++)
		{
			if(checkcontained(input[j], transaction))
			{
				freq[j]++;
			}
		}
	}
}

int check_first(vector<int> candidate, vector<int> transaction)
{
	int i=0;
	while(i<transaction.size() && candidate[0]!=transaction[i])
		i++;
	if(i==transaction.size() && candidate[0]!=transaction[i])
		return -1;
	else 
		return i;
}

void checkcount_C2(vector<vector<int>> &input, vector<int> &freq)
{
	freq.clear();
	int size = input.size();
	//cout<<"size checkcount C2 "<<size<<endl;
	freq.resize(size);
	for(int i=0;i<global_transactions.size();i++)
	{
		vector<int> transaction = global_transactions[i].items;
		vector<int> vindex;
		int index=0;
		vindex.push_back(index);
		for(int j=0;j<size;j++){
			while(index<size && input[j][0]==input[index][0]){
				index++;
			}
			j=index-1;
			vindex.push_back(index);
		}
			
		for(int k=0;k<vindex.size()-1;k++)
		{
			int index_first = check_first(input[vindex[k]], transaction);
			if(index_first!=-1)
			{
				for(int l=vindex[k];l<vindex[k+1];l++){
					int var = checkcontained_1(input[l], transaction,index_first);
					if(var!=-1)
					{
						freq[l]++;
						index_first=var;
					}

				}
			}
		}
	}
}


int main(int arg,char *arv[])
{
	char *inputfile=arv[1];
	take_input(inputfile);
	vector<int> frequency(10000);
	firstscan3(frequency);

	float supportpercent = atof(arv[2]);
	vector<int> F1;
	vector<int> freqF1;
	check_support_first(frequency, supportpercent, F1, freqF1);

	vector<vector<int>> finalF;
	vector<vector<int>> C2;
	candidate_gen_second(F1, freqF1, C2);
	vector<int> freqC2;
	checkcount_C2(C2, freqC2);
	vector<vector<int>> F2;
	check_support(C2, freqC2, supportpercent, F2);
	finalF=F2;

	bool flag = true;
	vector<vector<int>> Fk1;
	Fk1=F2;
	vector<vector<int>> Ck;
	vector<int> freqCk;
	while(flag)
	{
		candidate_gen(Fk1, Ck);
		checkcount(Ck, freqCk);
		check_support(Ck, freqCk, supportpercent, Fk1);
		if(Fk1.size()==0)
			flag=false;
		finalF.insert(finalF.end(), Fk1.begin(), Fk1.end());
	}

	//generate the output file
	ofstream myfile ("output.txt");
  	if (myfile.is_open())
  	{
  		for(int i=0;i<F1.size();i++)
		{
			myfile<<F1[i]<<endl;
		}

		for(int i=0;i<finalF.size();i++)
		{
			for(int j=0;j<finalF[i].size();j++)
				myfile<<finalF[i][j]<<" ";
			myfile<<endl;
		}
  	}
  }