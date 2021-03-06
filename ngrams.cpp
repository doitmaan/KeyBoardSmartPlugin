// AI_A3.cpp : Defines the entry point for the console application.
//

#include <string>
#include<algorithm>
#include<ctime>
#include<unordered_map>
#include<iostream>

#include"fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"

using namespace std;

void generate_nGramsList(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, int n);
void generate_nGramsList(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, vector<int> & maxCountList, int n);
void generate_nGramsListChar(vector<unordered_map<vector<char>, int>> & nGramsList, vector<char> & words, int n);

void P4(string training_data, string test_sentence, int n, double delta);
void P5(string training_data, string test_sentence, int n, int threshold);
void P6(int n, double delta, int senLength);
void P7(string train_text, string text_check, string dictionary, int n, int threshold, double delta, int model);

int main()
{
	P5("Texts/Novels/KafkaTrial.txt", "Texts/Novels/testFile.txt", 1, 1);
	P5("Texts/Novels/KafkaTrial.txt", "Texts/Novels/testFile.txt", 2, 5);
	P5("Texts/Novels/KafkaTrial.txt", "Texts/Novels/testFile.txt", 3, 5);

	//P6(2, 0.05, 50);

	//P7("InputOutput/hugeTrain.txt", "InputOutput/textCheck.txt", "InputOutput/dictionary.txt", 2, 2, 0.01, 1);
	
	system("PAUSE");

	return 0;
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

	double B = pow(V.size() + 1, n), B_1 = pow(V.size() + 1, n-1);
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
				sentence_prob *= delta / (N + delta * B);
		}
	}

	else {
		for (int i = 0; i < sentence.size() - n + 1; i++) {
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
				n_1gramCount = nGramsList[n - 2][n_1gram];

			if (i == 0)
				sentence_prob *= (ngramCount + delta) / (N + delta * B);
			else
				sentence_prob *= ((ngramCount + delta) / (N + delta * B))/((n_1gramCount + delta) / (N + delta * B_1));
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

void P5(string training_data, string test_sentence, int n, int threshold) {
	
	vector<string> words, sentence, V;
	vector<int> maxCountList;
	read_tokens(training_data, words, false);
	read_tokens(test_sentence, sentence, false);

	double N = words.size(), N_test = sentence.size();

	vector<unordered_map<vector<string>, int>> nGramsList;
	generate_nGramsList(nGramsList, words, maxCountList, n);

	auto uniGrams = nGramsList[0];
	for (auto uniGram = uniGrams.begin(); uniGram != uniGrams.end(); uniGram++)
		V.push_back(uniGram->first[0]);

	double B = pow(V.size() + 1, n), B_1 = pow(V.size() + 1, n - 1);


	vector<double> NrList(maxCountList[n-1]+1, 0.0), Nr_1List(maxCountList[n - 2]+1, 0.0);

	//compute Nr counts and store in a vector
	for (auto ngram = nGramsList[n - 1].begin(); ngram != nGramsList[n - 1].end(); ngram++)
		NrList[ngram->second] += 1;
	
	//compute (N-1)r counts and store in a vector
	for (auto ngram = nGramsList[n - 2].begin(); ngram != nGramsList[n - 2].end(); ngram++)
		Nr_1List[ngram->second] += 1;

	//compute N0 and (N-1)0 counts and store in element zero of each r vector respectively
	double nSum = 0, n_1Sum = 0;
	for (int i = 1; i < NrList.size(); i++)
		nSum += NrList[i];
	NrList[0] = B - nSum;
	
	for (int i = 1; i < Nr_1List.size(); i++)
		n_1Sum += Nr_1List[i];
	Nr_1List[0] = B_1 - n_1Sum;

	vector<double> NrProbsList(maxCountList[n - 1] + 1, 0.0), Nr_1ProbsList(maxCountList[n - 2] + 1, 0.0);


	//turn Nr and (N-1)r lists into GT/MLE probability lists (NOTE ASSUMING THRESHOLD IS LESS THAN MAX R).
	
	for (int i = 0; i < NrList.size(); i++) {
		if (i < threshold && NrList[i] != 0)
			NrProbsList[i] = ((i + 1)*NrList[i + 1]) / (N*NrList[i]);
		else
			NrProbsList[i] = i / N;
	}

	for (int i = 0; i < Nr_1List.size(); i++) {
		if (i < threshold && Nr_1List[i] != 0)
			Nr_1ProbsList[i] = ((i + 1)*Nr_1List[i + 1]) / (N*Nr_1List[i]);
		else
			Nr_1ProbsList[i] = i / N;
	}

	//normalize probabilities for seen ngrams:

	//sum of probabilities of seen ngrams and n_1grams:
	double npSum = 0, n_1pSum = 0;
	for (int i = 1; i < NrList.size(); i++)
		npSum += NrList[i]*NrProbsList[i];

	for (int i = 1; i < Nr_1List.size(); i++)
		n_1pSum += Nr_1List[i]*Nr_1ProbsList[i];

	//multiply each probability by normailzing factor
	for (int i = 1; i < NrList.size(); i++)
		NrProbsList[i] *= (1 - NrList[0] * NrProbsList[0]) / npSum;

	for (int i = 1; i < Nr_1List.size(); i++)
		Nr_1ProbsList[i] *= (1 - Nr_1List[0] * Nr_1ProbsList[0]) / n_1pSum;

	//npSum = 0, n_1pSum = 0;
	//for (int i = 0; i < NrList.size(); i++)
	//	npSum += NrList[i] * NrProbsList[i];

	//for (int i = 0; i < Nr_1List.size(); i++)
	//	n_1pSum += Nr_1List[i] * Nr_1ProbsList[i];

	
	if (n > sentence.size() || n > words.size())
		cout << "n too large for texts given" << endl;

	else {
		double log_sentence_prob = 0;

		for (int i = 0; i < sentence.size() - n + 1; i++) {
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
				n_1gramCount = nGramsList[n - 2][n_1gram];

			if (i == 0)
				log_sentence_prob += log(NrProbsList[ngramCount]);
			else
				log_sentence_prob += log(NrProbsList[ngramCount]) - log(Nr_1ProbsList[n_1gramCount]);
		}
		cout << log_sentence_prob << endl;
	}



	//1. generate the ngramList 
	//2. generate list of probabilities of ngrams & n_1grams with rates 1-r
	//3. substitute probabilities for counts in the database
	//4. for each ngram in sentence calculate prob. devide by n_1gram prob
}

void P6(int n, double delta, int senLength) {

	//LANGUAGE INDEXING:
	//0. DANISH
	//1. ENGLISH
	//2. FRENCH
	//3. ITALIAN
	//4. LATIN
	//5. SWEDISH

	vector<vector<char>> training_corpus(6), test_corpus(6);//generate a training corpus of size 6 (6 languages)
	vector<string> training_texts, test_texts;
	training_texts.push_back("Texts/Languages/danish1.txt");
	training_texts.push_back("Texts/Languages/english1.txt");
	training_texts.push_back("Texts/Languages/french1.txt");
	training_texts.push_back("Texts/Languages/italian1.txt");
	training_texts.push_back("Texts/Languages/latin1.txt");
	training_texts.push_back("Texts/Languages/sweedish1.txt");

	test_texts.push_back("Texts/Languages/danish2.txt");
	test_texts.push_back("Texts/Languages/english2.txt");
	test_texts.push_back("Texts/Languages/french2.txt");
	test_texts.push_back("Texts/Languages/italian2.txt");
	test_texts.push_back("Texts/Languages/latin2.txt");
	test_texts.push_back("Texts/Languages/sweedish2.txt");

	int V_n = 256, B = pow(V_n + 1, n), B_1 = pow(V_n + 1, n - 1);
	
	for (int i = 0; i < training_corpus.size(); i++)
		read_tokens(training_texts[i], training_corpus[i], false);

	for (int i = 0; i < test_corpus.size(); i++)
		read_tokens(test_texts[i], test_corpus[i], false);
	
	//creates a matrix arranged by [language #][sentence #] who's entrys are sentences(vectors of chars).
	vector<vector<vector<char>>> test_sentences(6);

	for (int i = 0; i < test_corpus.size(); i++) { //FOR LANGUAGE : TEST_CORPUS
		int size = test_corpus[i].size() / senLength;
		if (test_corpus[i].size() % senLength)
			size--;
 		for (int j = 0; j < size; j++) { //FOR SENTENCE : LANGUAGE
			vector<char> sentence;
			for (int k = j * senLength; k < (j + 1)*senLength; k++) //FOR WORD : SENTENCE
				sentence.push_back(test_corpus[i][k]);
			test_sentences[i].push_back(sentence);
		}
	}

	//create a database indexed by [language][n] in which each entry contains the unordered map of ngrams for a given language
	vector<vector<unordered_map<vector<char>, int>>> nGramsLanguageDB(6);
	
	for (int i = 0; i < nGramsLanguageDB.size(); i++)
		generate_nGramsListChar(nGramsLanguageDB[i], training_corpus[i], n);

	double CONFmatrix[6][6] = { 0 }, error_count = 0;

	if (n > senLength)
		cout << "n too large for texts given" << endl;

	else if (n == 1) {
		for (int i = 0; i < test_sentences.size(); i++) { //FOR LANGUAGE : TEST SENTENCES
			for (int j = 0; j < test_sentences[i].size(); j++) { //FOR SENTENCE : LANGUAGE
				vector<double> log_sentence_probs(6, 0.0);
				for (int k = 0; k < test_sentences[i][j].size(); k++) { //FOR CHAR : SENTENCE
					vector<char> unigram;
					unigram.push_back(test_sentences[i][j][k]);
					for (int a = 0; a < nGramsLanguageDB.size(); a++) { //FOR LANGUAGE : DB
						if (nGramsLanguageDB[a][0].count(unigram) == 1)
							log_sentence_probs[a] += log((nGramsLanguageDB[a][0][unigram] + delta) / (double(training_corpus[a].size()) + delta * B));
						else
							log_sentence_probs[a] += log(delta / (double(training_corpus[a].size()) + delta * B));
					}
				}
					double min_log_prob; int min_index;
					for (int b = 0; b < log_sentence_probs.size(); b++) {
						if (b == 0) {
							min_log_prob = log_sentence_probs[b];
							min_index = b;
						}
						if (log_sentence_probs[b] > min_log_prob) {
							min_log_prob = log_sentence_probs[b];
							min_index = b;
						}
					}
					CONFmatrix[i][min_index] += 1;
					if (i != min_index)  error_count += 1;
			}
		}
	}

	else {
		for (int i = 0; i < test_sentences.size(); i++) { //FOR LANGUAGE : TEST SENTENCES
			for (int j = 0; j < test_sentences[i].size(); j++) { //FOR SENTENCE : LANGUAGE
				vector<double> log_sentence_probs(6, 0.0);
				for (int k = 0; k < test_sentences[i][j].size() - n + 1; k++) { //FOR CHAR : SENTENCE
					vector<char> ngram, n_1gram;
					for (int l = k; l < k + n; l++) {
						ngram.push_back(test_sentences[i][j][l]);
						if (l < k + n - 1)
							n_1gram.push_back(test_sentences[i][j][l]);
					}
					for (int a = 0; a < nGramsLanguageDB.size(); a++) { //FOR LANGUAGE : DB
						double ngramCount = 0, n_1gramCount = 0;

						if (nGramsLanguageDB[a][n - 1].count(ngram) == 1)
							ngramCount = nGramsLanguageDB[a][n - 1][ngram];

						if (nGramsLanguageDB[a][n - 2].count(n_1gram) == 1)
							n_1gramCount = nGramsLanguageDB[a][n - 2][n_1gram];

						if (k == 0)
							log_sentence_probs[a] += log((ngramCount + delta) / double(training_corpus[a].size() + delta * B));
						else
							log_sentence_probs[a] += log((ngramCount + delta) / double(training_corpus[a].size() + delta * B)) - log((n_1gramCount + delta) / double(training_corpus[a].size() + delta * B_1));
					}
				}
				double min_log_prob; int min_index;
				for (int b = 0; b < log_sentence_probs.size(); b++) {
					if (b == 0) {
						min_log_prob = log_sentence_probs[b];
						min_index = b;
					}
					if (log_sentence_probs[b] > min_log_prob) {
						min_log_prob = log_sentence_probs[b];
						min_index = b;
					}
				}
				CONFmatrix[i][min_index] += 1;
				if (i != min_index)  error_count += 1;
			}
		}
	}
	double total = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			total += CONFmatrix[i][j];
			cout << CONFmatrix[i][j] << "\t";
		}
		cout << endl;
	}

	cout << "ERROR RATE: " << (error_count / total)*100 << endl;

}

void P7(string train_text, string text_check, string dictionary, int n, int threshold, double delta, int model) {
	vector<string> words, sentences, V;
	vector<int> maxCountList;

	read_tokens(train_text, words, false);
	read_tokens(dictionary, V, false);
	read_tokens(text_check, sentences, true);

	double N = words.size(), N_test = sentences.size();

	vector<vector<string>> sentence_list;
	vector<string> sentence;
	for (int i = 0; i < sentences.size(); i++) {
		if (sentences[i].compare(EOS) != 0)
			sentence.push_back(sentences[i]);
		else {
			sentence_list.push_back(sentence);
			sentence.clear();
		}
	}
	vector<unordered_map<vector<string>, int>> nGramsList;
	generate_nGramsList(nGramsList, words, maxCountList, n);

	double B = pow(V.size() + 1, n), B_1 = pow(V.size() + 1, n - 1);

	//MIN EDIT DISTANCE BS
	vector<vector<vector<string>>> alternate_sentence_table;
	for (int i = 0; i < sentence_list.size(); i++) { //FOR SENTENCE : SENTENCES
		vector<vector<string>> alternate_sentence_list;
		alternate_sentence_list.push_back(sentence_list[i]);
		for (int j = 0; j < sentence_list[i].size(); j++) { //FOR WORD : SENTENCE
			for (int k = 0; k < V.size(); k++) { //FOR WORD : DICTIONARY.
				if (uiLevenshteinDistance(sentence_list[i][j], V[k]) <= 1) {
					vector<string> alternate_sentence(sentence_list[i]);
					alternate_sentence[j] = V[k];
					alternate_sentence_list.push_back(alternate_sentence);
				}
			}
		}
		alternate_sentence_table.push_back(alternate_sentence_list);
	}

	if (model == 1) { //ADD-DELTA

		for (int i = 0; i < alternate_sentence_table.size(); i++) {//FOR SENTENCE : ALTERNATE SENTENCE TABLE
			
			vector<double> alternate_sentence_probs(alternate_sentence_table[i].size(), 0.0); //LOG PROBS.

			for (int j = 0; j < alternate_sentence_table[i].size(); j++) { // FOR ALTERNATE SENTENCE : SENTENCE
				
				if (n > alternate_sentence_table[i][j].size())
					cout << "n too large for sentence given" << endl;

				else if (n == 1) {
					for (string word : alternate_sentence_table[i][j]) {
						vector<string> unigram;
						unigram.push_back(word);
						if (nGramsList[0].count(unigram) == 1)
							alternate_sentence_probs[j] += log((nGramsList[0][unigram] + delta) / (N + delta * B));
						else
							alternate_sentence_probs[j] += log(delta / (N + delta * B));
					}
				}

				else {
					for (int k = 0; k < alternate_sentence_table[i][j].size() - n + 1; k++) {
						vector<string> ngram, n_1gram;
						for (int l = k; l < k + n; l++) {
							ngram.push_back(alternate_sentence_table[i][j][l]);
							if (l < k + n - 1)
								n_1gram.push_back(alternate_sentence_table[i][j][l]);
						}
						double ngramCount = 0, n_1gramCount = 0;

						if (nGramsList[n - 1].count(ngram) == 1)
							ngramCount = nGramsList[n - 1][ngram];

						if (nGramsList[n - 2].count(n_1gram) == 1)
							n_1gramCount = nGramsList[n - 2][n_1gram];

						if (i == 0)
							alternate_sentence_probs[j] += log((ngramCount + delta) / (N + delta * B));
						else
							alternate_sentence_probs[j] += log((ngramCount + delta) / (N + delta * B)) - log((n_1gramCount + delta) / (N + delta * B_1));
					}
				}
			}
			double min_log_prob; int min_index;
			for (int b = 0; b < alternate_sentence_probs.size(); b++) {
				if (b == 0) {
					min_log_prob = alternate_sentence_probs[b];
					min_index = b;
				}
				if (alternate_sentence_probs[b] > min_log_prob) {
					min_log_prob = alternate_sentence_probs[b];
					min_index = b;
				}
			}
			auto output_sentence = alternate_sentence_table[i][min_index];
			for (string word : output_sentence)
				cout << word << " ";
			cout << endl;
		}
	}
	if (model == 0) { //GT MODEL
		vector<double> NrList(maxCountList[n - 1] + 1, 0.0), Nr_1List(maxCountList[n - 2] + 1, 0.0);

		//compute Nr counts and store in a vector
		for (auto ngram = nGramsList[n - 1].begin(); ngram != nGramsList[n - 1].end(); ngram++)
			NrList[ngram->second] += 1;

		//compute (N-1)r counts and store in a vector
		for (auto ngram = nGramsList[n - 2].begin(); ngram != nGramsList[n - 2].end(); ngram++)
			Nr_1List[ngram->second] += 1;

		//compute N0 and (N-1)0 counts and store in element zero of each r vector respectively
		double nSum = 0, n_1Sum = 0;
		for (int i = 1; i < NrList.size(); i++)
			nSum += NrList[i];
		NrList[0] = B - nSum;

		for (int i = 1; i < Nr_1List.size(); i++)
			n_1Sum += Nr_1List[i];
		Nr_1List[0] = B_1 - n_1Sum;

		vector<double> NrProbsList(maxCountList[n - 1] + 1, 0.0), Nr_1ProbsList(maxCountList[n - 2] + 1, 0.0);


		//turn Nr and (N-1)r lists into GT/MLE probability lists (NOTE ASSUMING THRESHOLD IS LESS THAN MAX R).

		for (int i = 0; i < NrList.size(); i++) {
			if (i < threshold && NrList[i] != 0)
				NrProbsList[i] = ((i + 1)*NrList[i + 1]) / (N*NrList[i]);
			else
				NrProbsList[i] = i / N;
		}

		for (int i = 0; i < Nr_1List.size(); i++) {
			if (i < threshold && Nr_1List[i] != 0)
				Nr_1ProbsList[i] = ((i + 1)*Nr_1List[i + 1]) / (N*Nr_1List[i]);
			else
				Nr_1ProbsList[i] = i / N;
		}

		//normalize probabilities for seen ngrams:

		//sum of probabilities of seen ngrams and n_1grams:
		double npSum = 0, n_1pSum = 0;
		for (int i = 1; i < NrList.size(); i++)
			npSum += NrList[i] * NrProbsList[i];

		for (int i = 1; i < Nr_1List.size(); i++)
			n_1pSum += Nr_1List[i] * Nr_1ProbsList[i];

		//multiply each probability by normailzing factor
		for (int i = 1; i < NrList.size(); i++)
			NrProbsList[i] *= (1 - NrList[0] * NrProbsList[0]) / npSum;

		for (int i = 1; i < Nr_1List.size(); i++)
			Nr_1ProbsList[i] *= (1 - Nr_1List[0] * Nr_1ProbsList[0]) / n_1pSum;

		//npSum = 0, n_1pSum = 0;
		//for (int i = 0; i < NrList.size(); i++)
		//	npSum += NrList[i] * NrProbsList[i];

		//for (int i = 0; i < Nr_1List.size(); i++)
		//	n_1pSum += Nr_1List[i] * Nr_1ProbsList[i];


		for (int i = 0; i < alternate_sentence_table.size(); i++) {//FOR SENTENCE : ALTERNATE SENTENCE TABLE

			vector<double> alternate_sentence_probs(alternate_sentence_table[i].size(), 0.0); //LOG PROBS.

			for (int j = 0; j < alternate_sentence_table[i].size(); j++) { // FOR ALTERNATE SENTENCE : SENTENCE

				if (n > alternate_sentence_table[i][j].size())
					cout << "n too large for sentence given" << endl;

				else {
					for (int k = 0; k < alternate_sentence_table[i][j].size() - n + 1; k++) { //FOR WORD : ALTERNATE SENTENCE
						vector<string> ngram, n_1gram;
						for (int l = k; l < k + n; l++) {
							ngram.push_back(alternate_sentence_table[i][j][l]);
							if (l < i + n - 1)
								n_1gram.push_back(alternate_sentence_table[i][j][l]);
						}
						double ngramCount = 0, n_1gramCount = 0;

						if (nGramsList[n - 1].count(ngram) == 1)
							ngramCount = nGramsList[n - 1][ngram];

						if (nGramsList[n - 2].count(n_1gram) == 1)
							n_1gramCount = nGramsList[n - 2][n_1gram];

						if (i == 0)
							alternate_sentence_probs[j] += log(NrProbsList[ngramCount]);
						else
							alternate_sentence_probs[j] += log(NrProbsList[ngramCount]) - log(Nr_1ProbsList[n_1gramCount]);
					}
				}
			}
			double min_log_prob; int min_index;
			for (int b = 0; b < alternate_sentence_probs.size(); b++) {
				if (b == 0) {
					min_log_prob = alternate_sentence_probs[b];
					min_index = b;
				}
				if (alternate_sentence_probs[b] > min_log_prob) {
					min_log_prob = alternate_sentence_probs[b];
					min_index = b;
				}
			}
			auto output_sentence = alternate_sentence_table[i][min_index];
			for (string word : output_sentence)
				cout << word << " ";
			cout << endl;
		}
	}

	//DELTA MODEL BS
	//OR
	//GT MODEL BS


}

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

void generate_nGramsListChar(vector<unordered_map<vector<char>, int>> & nGramsList, vector<char> & words, int n) {

	// Now create all n-grams from vector of tokens and insert them into the database (including unigrams)
	if (words.size() < n)
		cout << "\nInput file is too small to create any nGrams of size " << n;
	else
	{
		for (int k = 1; k <= n; k++) {
			unordered_map<vector<char>, int> nGrams;
			for (int i = 0; i <= words.size() - k; i++)
			{
				vector<char> nGram(k);   // for temporarily storing tokens to go into next n-gram

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

void generate_nGramsList(vector<unordered_map<vector<string>, int>> & nGramsList, vector<string> & words, vector<int> & maxCountList, int n) {

	// Now create all n-grams from vector of tokens and insert them into the database (including unigrams)
	if (words.size() < n)
		cout << "\nInput file is too small to create any nGrams of size " << n;
	else
	{
		for (int k = 1; k <= n; k++) {
			unordered_map<vector<string>, int> nGrams;
			int maxCount = 0;
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

				if (maxCount < nGrams[nGram])   // update maximum nGram count, if needed 
					maxCount = nGrams[nGram];
			}
			nGramsList.push_back(nGrams);
			maxCountList.push_back(maxCount);
		}

	}
}