#include"P.h"

void P2(string file1, string file2, int nGram, bool printCommon) {

	vector<string> words1, words2;

	read_tokens(file1, words1, 0); //EOS IS NOT TAKEN INTO CONSIDERATION
	read_tokens(file2, words2, 0); //EOS IS NOT TAKEN INTO CONSIDERATION
	
	vector<string> nGrams1(words1.size()-nGram), nGrams2(words2.size()-nGram);


	for (int i = 0; i < words1.size()-nGram; i++) {
		nGrams1[i] = "";
		for (int j = 0; j < nGram; j++) {
			nGrams1[i] += words1[i + j];
			nGrams1[i] += " ";
		}
	}

	for (int i = 0; i < words2.size() - nGram; i++) {
		nGrams2[i] = "";
		for (int j = 0; j < nGram; j++) {
			nGrams2[i] += words2[i + j];
			nGrams2[i] += " ";
		}
	}
	int similarities = 0;

	for (int i = 0; i < nGrams2.size(); i++) {
		for (int j = 0; j < nGrams1.size(); j++) {
			if (nGrams1[j].compare(nGrams2[i]) == 0) {
				similarities++;
				if (printCommon) cout << nGrams1[j] << endl;
			}
		}
	}

	cout << ((double)similarities / (double)nGrams2.size()) << endl;
	

}
