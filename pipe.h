#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <time.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;
using std::getline;

void openFile(string filename);
string checkText(char* allChars, string word, vector<string>&);
bool compareNoCase (string first, string second);
