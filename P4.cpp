#include"P.h"

void generate_nGramsList(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, int n) {
	
	// Now create all n-grams from vector of tokens and insert them into the database (including unigrams)
	if (words.size() < n)
		cout << "\nInput file is too small to create any nGrams of size " << n;
	else
	{
		for (int k = 1; k <= n; k++) {
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


void P4(string training_data, string test_sentence, int n, double delta) {

	vector<string> words, sentence, V;
	read_tokens(training_data, words, false);
	read_tokens(test_sentence, sentence, false);

	double N = words.size(), N_test = sentence.size();

	vector<unordered_map<vector<string>, int>> nGramsList;
	generate_nGramsList(nGramsList, words, n);
	
	auto uniGrams = nGramsList[0];
	for (auto uniGram = uniGrams.begin(); uniGram != uniGrams.end(); uniGram++)
		V.push_back(uniGram->first[0]);

	double B = pow(V.size() + 1, n);
	double sentence_prob = 1;

	if (n > sentence.size() || n > words.size())
		cout << "n too large for texts given" << endl;
	
	else if (n == 1) {
		for (string word : sentence) {
			vector<string> unigram;
			unigram.push_back(word);
			if (nGramsList[0].count(unigram) == 1)
				sentence_prob *= (nGramsList[0][unigram] + delta) / (N + delta * B);
			else
				sentence_prob*= delta / (N + delta * B);
		}
	}

	else {
		for (int i = 0; i < sentence.size()-n+1; i++) {
			vector<string> ngram, n_1gram;
			for (int j = i; j < i + n; j++) {
				ngram.push_back(sentence[j]);
				if (j < i + n - 1)
					n_1gram.push_back(sentence[j]);
			}
			double ngramCount = 0, n_1gramCount = 0;
			if (nGramsList[n - 1].count(ngram) == 1)
				ngramCount = nGramsList[n - 1][ngram];

			if (nGramsList[n - 2].count(n_1gram) == 1)
				ngramCount = nGramsList[n - 1][n_1gram];

			if (i == 0)
				sentence_prob *= (ngramCount + delta) / (N + delta * B);
			else
				sentence_prob *= (ngramCount + delta) / (n_1gramCount + delta);
		}
	}
	cout << log(sentence_prob) << endl;
	
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\\
	//construct ngrams list
	//if(unigram) do word by word
	//else
	//	chain rule:
	//				- P(w1)p(w2|w1)p(w3|w1w2)...p(wk|wk-1....wk-n+1) 
	//				where k is the length of the word, n is the length of ngram model.
					

}