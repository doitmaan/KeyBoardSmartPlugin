#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <math.h>       /* pow */
#include <numeric>
#include <vector>

#include "fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"

typedef string T;

using namespace std;

unordered_map<vector<T>, int> ngramgenerator(int n, T filename) {


	// specifies the "n" for the nGram model
	// text for language modeling 

	cout << "\n\n creating language model with n = " << n;

	int maxCount = 0; // will store the count of most frequent nGram
	int N = 0;        // will store the number of tokens read from file 

	try
	{
		// First read all tokens from file into a vector 		
		vector<T> tokens;     // stores tokens read from file

		N = tokens.size();    // that is the number of words to process

		read_tokens(filename, tokens, false); // reads tokens from file without EOS 
											  //read_tokens(filename, tokens, true); // reads tokens from file with EOS 


											  // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
											  // Associated with the key is the count of that n-gram
		unordered_map<vector<T>, int> database;


		// Now create all n-grams from vector of tokens and insert them into the database 
		if (tokens.size() < n)
			cout << "\nInput file is too small to create any nGrams of size " << n;
		else
		{
			for (int i = 0; i <= tokens.size() - n; i++)
			{
				vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

									  // Take next n tokens read from the input file 
				for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
					nGram[j] = tokens[i + j];

				if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
					database[nGram] = 1;
				else // nGram is already in the database, increase its count by 1
					database[nGram] = database[nGram] + 1;

				if (maxCount < database[nGram])   // update maximum nGram count, if needed 
					maxCount = database[nGram];

			}

			///////////////////////////////////////////////////////////////////////////
			// now go over all nGrams in the database and print them out with their counts

			// prints summary of nGrams inserted
			//cout << "\nMax nGram count " << maxCount;
			//cout << "\nNumber of distinct nGrams " << database.size();
			//cout << "\nTotal number of words read " << N;


			// now go over all nGrams in the database and print them out with their counts
			//cout << "\nPrinting out all the nGrams with their counts";

			//for (auto i = database.begin(); i != database.end(); ++i)
			//{
			//	vector<T> nGram = i->first;
			//	cout << "\n";

			//	for (unsigned int j = 0; j < nGram.size(); j++)
			//		cout << nGram[j] << " ";  // prints out next string in the current nGram

			//	cout << " " << i->second; // prints out the count of this nGram
			//}
		}

		return database;

	}
	catch (FileReadException e)
	{
		e.Report();
	}
}

double p4(T textModel, T sentence, int n, double delta) {
	unordered_map<vector<T>, int> database_n, database_n2, sentence_n2, sentence_n, database_size;

	double size_n, size_nn, b_1, b_2, p_unseen_n, p_unseen_nn, probabilityn, probabilitynn;
	float N, N1, N2;
	probabilityn = 1;
	probabilitynn = 1;

	vector<T> tokensentence;

	vector<T> tokens;     // stores tokens read from file

						  // that is the number of words to process

	read_tokens(textModel, tokens, false);

	N1 = tokens.size();//number of unique words 

	double log_probability = 0;

	read_tokens(sentence, tokensentence, false);

	N2 = tokensentence.size();

	/* cout << N1 << endl << "number of words in the testing sentence ^^" << endl; */

		if (n == 1) {
			sentence_n = ngramgenerator(n, sentence);
			database_n = ngramgenerator(n, textModel);

			N = database_n.size();

			cout << endl;
			b_1 = (pow((N), n));
			if (delta == 0)
			{
				return DBL_MAX;
			}
			if (delta >= 0)
			{
				p_unseen_n = double(delta / double(((delta *b_1) + double(N1))));


				cout << "probability of unseen for unigram model ==" << p_unseen_n << endl;
			}
			for (auto i = sentence_n.begin(); i != sentence_n.end(); i++)
			{
				vector<T> gram = i->first;

				//if the gram isn't in the first text, add it to the count of no occurances
				if (database_n[gram] == 0) {
					log_probability += database_n[gram]*log(p_unseen_n);
				}
				else { // if it's in there and print is true, print it!

					log_probability += database_n[gram]*log((database_n[gram] + delta) / double(((delta *b_1) + double(N1))));

				}
			}
			cout << log_probability;

		}


		else {
			database_n = ngramgenerator(n, textModel);
			database_n2 = ngramgenerator(n - 1, textModel);


			database_size = ngramgenerator(1, textModel);
			N = database_size.size(); // for the number of unique words 


			sentence_n = ngramgenerator(n, sentence); // approve fpr the finding the probabilityn 


			sentence_n2 = ngramgenerator(n - 1, sentence); // will need to coverte this to a vector of ordered strings 


			vector<vector<T> >nGramnn/*(N2 - 1); */; // for temporarily storing tokens to go into next n-gram
			for (int i = 1; i <= tokensentence.size() - 1 - (n - 1); i++)
			{
				vector<T> nGram(n - 1);

				// Take next n tokens read from the input file 
				for (unsigned int j = 0; j < n - 1; j++) { // put next n tokens into vector temp
					nGram[j] = tokensentence[i + j];
				}

				nGramnn.push_back((nGram));
				//if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
				//	database[nGram] = 1;
				//else // nGram is already in the database, increase its count by 1
				//	database[nGram] = database[nGram] + 1;

				//if (maxCount < database[nGram])   // update maximum nGram count, if needed 
				//	maxCount = database[nGram];

			}

			cout << endl;
			//cout << nGramnn.size() << endl;
			cout << endl;

			b_1 = (pow((N+1), n));
			b_2 = float((pow((N+1), n - 1)));

			if (delta >= 0)
			{
				p_unseen_n = double(delta / double(((delta *b_1) + double(N1))));
				p_unseen_nn = double(delta / double(((delta *b_2) + double(N1))));


				cout << "probability of unseen for the ngram =" << p_unseen_n << endl;
				cout << "probability of unseen for the n -1 gram =" << p_unseen_nn << endl;
				cout << endl;

			}

			else if (delta == 0) {
				cout << "probability of unseen (as delta=0) = " << 0;
				return DBL_MAX;

			}




			for (auto i = sentence_n.begin(); i != sentence_n.end(); i++)
			{
				vector<T> gram = i->first;

				//if the gram isn't in the first text, add it to the count of no occurances
				if (database_n.count(gram) == 0) {
					log_probability += log(p_unseen_n);
				}
				else { // if it's in there and print is true, print it!

					log_probability +=  log(double((database_n[gram] + delta) / double(((delta *b_1) + double(N1)))));

				}
			}

			//cout << endl;

			//cout << probabilityn;

			//cout << endl;


			//cout << " token size aka words number " << tokensentence.size() << endl;




			for (int i = 0; i < nGramnn.size(); i++)
			{
				vector<string> gram;
				gram = (nGramnn[i]);

				//if the gram isn't in the first text, add it to the count of no occurances
				if (database_n2.count(gram) == 0) {
					log_probability -= log(p_unseen_nn);
				}
				else { // if it's in there and print is true,  it!

					log_probability -= log((database_n2[gram] + delta) / double(((delta *b_2) + double(N1))));

				}
			}


			//cout << endl;

			//cout << probabilitynn;

			//cout << endl;

			cout << log_probability;

			//double tottalprobability = probabilityn / probabilitynn;


			//cout << log(tottalprobability);
			//cout << endl; cout << endl; cout << endl; cout << endl; cout << endl; cout << endl;

			/*double x = (54 / pow(327, 4));
			cout <<log( x / probabilitynn);*/

		}



}

int main(int argc, char **argv)

{
	int x;

	p4("InputOutput/p4_textModel.txt", "InputOutput/p4_sentence_a.txt", 3, 0.1);


	cin >> x;
	// Currently test the word model
	// For char model, uncomment char and comment string template definition 
	// in the beginning of the file
}