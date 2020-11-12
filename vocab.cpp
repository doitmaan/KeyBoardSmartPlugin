#include <string>
#include<algorithm>
#include<ctime>
#include<unordered_map>
#include<iostream>

#include"fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"

#define T string
int main(int argc, char **argv)
{
	double delta = 0.01;
	string modelFile = argv[1]; //file name by argument
	string sentanceFile = argv[2];

	int n = stoi(argv[3]);
	int threshold = stod(argv[4]);
	
	vector<int> maxCounts; // will store the count of most frequent nGram
	int N = 0;        // will store the number of tokens read from file 


	try
	{
		// First read all tokens from file into a vector 		
		vector<T> tokens;     // stores tokens read from file


		read_tokens(modelFile, tokens, false); // reads tokens from file without EOS 
											   //read_tokens(filename, tokens, true); // reads tokens from file with EOS 

		N = tokens.size();    // that is the number of words to process

							  // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
							  // Associated with the key is the count of that n-gram
		vector<T> sentance;

		read_tokens(sentanceFile, sentance, false);

		vector < unordered_map<vector<T>, int> > databases;


		// Now create all n-grams from vector of tokens and insert them into the database 
		if (tokens.size() < n)
			cout << "\nInput file is too small to create any nGrams of size " << n;
		else
		{
			//cout << "Creating DBs\n";
			// create n n-grams, with n decreasing
			for (int m = 1; m <= n; m++) {
				//cout << "Creating DB  "<<m <<"\n";
				int maxCount = 0;
				unordered_map<vector<T>, int> database;
				for (int i = 0; i <= tokens.size() - m; i++)
				{
					//n-gram model
					vector<T> nGram(m);   // for temporarily storing tokens to go into next n-gram

										  // Take next n tokens read from the input file 
					for (unsigned int j = 0; j < m; j++) // put next n tokens into vector temp
						nGram[j] = tokens[i + j];

					if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
						database[nGram] = 1;
					else // nGram is already in the database, increase its count by 1
						database[nGram] = database[nGram] + 1;

					if (maxCount < database[nGram])   // update maximum nGram count, if needed 
						maxCount = database[nGram];

				}
				databases.push_back(database);
				maxCounts.push_back(maxCount);
			}

			//create vocab V
			vector<T> V;
			for (auto i = databases[0].begin(); i != databases[0].end(); ++i) {
				V.push_back(i->first[0]);
			}
			// calcualte prob for nGrams 
			// = V.size() + 1;

			//create Nr heres
			//Nrs will be vector of Nr vectors of length maxCount, with an index for each Nr for uni, bi, grams

			
			vector < vector<double> > Nrs;
			for (int m = 1; m <= n; m++) {
				int maxCount = maxCounts[m-1];
				double vocLen = pow(V.size() + 1, m);
				vector<double> Nr(maxCount+1);
				int mycount = 0;
				for (auto i = databases[m-1].begin(); i != databases[m-1].end(); ++i) {
					//second is the amount of times data occured
					mycount++;
					if (i->second <= maxCount)
						Nr[i->second]++;
				}
				//N0 = V^(m+1) - SUM(N0, Nr)
				Nr[0] = vocLen;
				for (int i = 1; i < maxCount+1; i++) {
					Nr[0] -= Nr[i];
				}
				bool restart = false;
				for (int i = 0; i < threshold; i++) {
					if (Nr[i] == 0) {
						//set the threshold to be i, the count thats zero
						threshold = i;
						//change m to zero so we restart our loop
						m = 0;
						Nrs.clear();
						restart = true;
						break;
					}
						
				}
				if(!restart)
					Nrs.push_back(Nr);
			}
			vector< vector<double> > pGT;

			//calculate probability of each gram of repeats l
			for (int m = 0; m < n; m++) {
				int maxCount = maxCounts[m];
				vector<double> temp(maxCount+1);
				double seenP = 0;
				for (int l = 0; l <= maxCount; l++) {
					if (Nrs[m][l] == 0 || l >= threshold)
						temp[l] = (double)l/N;
					else
						temp[l] = (double)(l + 1) * Nrs[m][l + 1] / (N * Nrs[m][l]);
					if (l > 0)
						seenP += temp[l] * Nrs[m][l];
				}
				double unseenP = temp[0] * Nrs[m][0];
				double reWeight = (1 - unseenP) / seenP;
				for (int l = 1; l <= maxCount; l++) 
					temp[l] = temp[l] * reWeight;
				pGT.push_back(temp);
			}

			int m, p, cp, cm;

			double B, pr, pp, pm;
			vector<double> perc;
			for (int i = 0; i < sentance.size(); i++) {
				p = n;
				if (i < n)
					p = i + 1;
				B = pow(V.size() + 1, p);

				
				vector<T> pGram(p);
				if (i == 0 || n == 1) {
					pGram[0] = sentance[i];
					if (databases[p - 1].count(pGram) == 1) 
						cp = databases[p - 1][pGram];	
					else
						cp = 0;
					pr = pGT[p-1][cp];
	
					perc.push_back(pr);
				}
				else {
					m = p - 1;
					vector<T> mGram(m);

					// put previous words in nGram
					//index to m, in both, then add the current to pGram
					for (int j = 1; j <= m; j++) {
						if (sentance.size() - j < 0)
							break;
						pGram[m - j] = sentance[i - j];
						mGram[m - j] = sentance[i - j];
					}
					//fill in the current token
					pGram[m] = sentance[i];
					//then push back the percent in the perc vector
					if (databases[m - 1].count(mGram) == 1)
						cm = databases[m - 1][mGram];
					else
						cm = 0;
					if (databases[p - 1].count(pGram) == 1)
						cp = databases[p - 1][pGram];
					else
						cp = 0;

					
					pp = pGT[p-1][cp];
					
					pm = pGT[m-1][cm];
					
					pr = (double)pp / pm;
					perc.push_back(pr);
				}
			}
			double result = 0;
			for (double d : perc) {
				result += log(d);
			}
			cout << result << endl;
		}
	}
	catch (FileReadException e)
	{
		e.Report();
	}

	system("PAUSE");
}