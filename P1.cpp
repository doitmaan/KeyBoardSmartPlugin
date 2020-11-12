#include"P.h"

void P1(string filename, int k) {

	vector<string> words, wordcount;
	
	read_tokens(filename, words, 0); //EOS IS NOT TAKEN INTO CONSIDERATION
	vector<int> counts(size(words)), finalcounts;

	sort(words.begin(), words.end());

	for (int i = 0; i < words.size(); i++) {
		if (i == 0) {
			counts[i] = 1;
		}
		
		else if (words[i].compare(words[i - 1]) == 0) {
			counts[i] = counts[i - 1] + 1;
		}

		else counts[i] = 1;
	}

	for (int i = 0; i < words.size(); i++) {
		if (i < words.size()-1 && counts[i] < counts[i + 1]);
		else wordcount.push_back(to_string(counts[i])+words[i]);
	}
	
	sort(wordcount.begin(), wordcount.end());
	
	double subtotal = 0;
	for (int i = 0; i < k; i++) {
		subtotal += stoi(wordcount[wordcount.size() - i - 1].substr(0, 1));
		cout << wordcount[wordcount.size() - i - 1].substr(1) << ", " << stoi(wordcount[wordcount.size() - i - 1].substr(0, 1)) << endl;
		}
	cout << (subtotal / (double)words.size())*100<< "%" << endl;

}
