#pragma once
#include <string>
#include<vector>
#include<algorithm>
#include<ctime>
#include<unordered_map>
#include<iostream>

#include"fileRead.h"



using namespace std;


void P1(string filename, int k);
void P2(string file1, string file2, int nGram, bool printCommon);
void P3(string filename, int n);
void P4(string training_data, string test_sentence, int n, double delta);