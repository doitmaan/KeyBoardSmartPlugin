#include"P.h"
#include <iterator>
#include"utilsToStudents.h"
#include "VectorHash.h"

void generate_nGrams(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, int n);
void wordProbs(unordered_map<string, int> & uniGrams, vector<string> & words, vector<string> & V, vector<double> & probs);

void P3(string filename, int n) {

	vector<string> words, V;
	vector<double> uniprobs;
	read_tokens(filename, words, true);
	double N = words.size(), maxCount = 0;

	vector<unordered_map<vector<string>, int>> nGramsList;
	unordered_map<string, int> uniGrams;

	wordProbs(uniGrams, words, V, uniprobs);
	generate_nGrams(nGramsList, words, n);


	srand(time(NULL));
	string nextWord = "";
	if (n == 1) {
		while (nextWord.compare(EOS) != 0) {
			int index = drawIndex(uniprobs);
			nextWord = V[index];
			cout << nextWord << " ";
		}
	}

	else {
		//GENERATE FIRST WORD
		vector<double> probs;
		vector<string> V_nGram;
		vector<vector<string>> V_nGrams;

		//CREATE BIGRAMS WITH EOS
		for (string word : V) {
			V_nGram.push_back(EOS); 
			V_nGram.push_back(word);
			V_nGrams.push_back(V_nGram);
			V_nGram.clear();
		}

		//GENERATE PROBS VECTOR
		for (vector<string> V_nG : V_nGrams) {
			if(nGramsList[0].count(V_nG) == 1) //0 is bigrams
				probs.push_back(double(nGramsList[0][V_nG]) / double(uniGrams[EOS]));
			else
				probs.push_back(0);
		}

		nextWord = V[drawIndex(probs)];
		cout << nextWord << " ";
		probs.clear();

		for (int i = 0; i < n - 1 && (nextWord.compare(EOS) != 0); i++) {
			V_nGrams.clear();
			V_nGram.push_back(nextWord);

			for (string word : V) {
				V_nGram.push_back(word);
				V_nGrams.push_back(V_nGram);
				V_nGram.pop_back();
			}

			for (vector<string> V_nG : V_nGrams) {
				if (nGramsList[i].count(V_nG) == 1) { //0 is bigrams
					if (i == 0)
						probs.push_back(double(nGramsList[i][V_nG]) / double(uniGrams[nextWord]));
					else
						probs.push_back(double(nGramsList[i][V_nG]) / double(nGramsList[i-1][V_nGram]));
				}
				else
					probs.push_back(0);
			}

			nextWord = V[drawIndex(probs)];
			cout << nextWord << " ";
			probs.clear();
		}
				
		while (nextWord.compare(EOS) != 0) {
			V_nGrams.clear();
			V_nGram.push_back(nextWord);

			if (V_nGram.size()>1) {
				for (int i = 0; i < V_nGram.size()-1; i++)
					V_nGram[i] = V_nGram[i + 1];
				V_nGram.pop_back();
			}

			for (string word : V) {
				V_nGram.push_back(word);
				V_nGrams.push_back(V_nGram);
				V_nGram.pop_back();
			}

			for (vector<string> V_nG : V_nGrams) {
				if (nGramsList[n - 2].count(V_nG) == 1) { //n-2 is the nth ngram since ngramsList starts with bigrams (no zero-grams/unigrams)
					if (n == 2)
						probs.push_back(double(nGramsList[n - 2][V_nG]) / double(uniGrams[nextWord]));
					else
						probs.push_back(double(nGramsList[n - 2][V_nG]) / double(nGramsList[n - 3][V_nGram]));
				}
				else
					probs.push_back(0);
			}

			nextWord = V[drawIndex(probs)];
			cout << nextWord << " ";
			probs.clear();
		}
	}
}

void wordProbs(unordered_map<string, int> & uniGrams, vector<string> & words, vector<string> & V, vector<double> & probs) {
	double N = words.size();

		for (int i = 0; i < words.size(); i++)
		{
			string uniGram = words[i];   // for temporarily storing tokens to go into next n-gram
			if (uniGrams.count(uniGram) == 0) // nGram is not in the database yet, insert it with count 1
				uniGrams[uniGram] = 1;
			else // nGram is already in the database, increase its count by 1
				uniGrams[uniGram] = uniGrams[uniGram] + 1;
		}

		for (auto uniGram = uniGrams.begin(); uniGram != uniGrams.end(); uniGram++) {
			probs.push_back(double(uniGram->second) / N);
			V.push_back(uniGram->first);
		}
}

void generate_nGrams(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, int n) {
	
	// Now create all n-grams from vector of tokens and insert them into the database 
	if (words.size() < n)
		cout << "\nInput file is too small to create any nGrams of size " << n;
	else
	{
		for (int k = 2; k <= n; k++) {
			unordered_map<vector<string>, int> nGrams;
			for (int i = 0; i <= words.size() - k; i++)
			{
				vector<string> nGram(k);   // for temporarily storing tokens to go into next n-gram

										    // Take next n tokens read from the input file 
				for (unsigned int j = 0; j < k; j++) // put next n tokens into vector temp
					nGram[j] = words[i + j];

				if (nGrams.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
					nGrams[nGram] = 1;
				else // nGram is already in the database, increase its count by 1
					nGrams[nGram] = nGrams[nGram] + 1;
			}
			nGramsList.push_back(nGrams);
		}
		
	}
}