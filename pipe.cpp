#include "pipe.h"

//The reason these are global is that they are used in every function minus the main
int* matchingLineNum;
char* everyChar;
vector<string> textInFile;
int fileLen;
int matchedLen;

void openFile (string filename) {
	vector<string> containsWord;
	
	string line;
	ifstream file;
	
	//Opening file and placing the contents into a vector
	file.open(filename.c_str());
	while(getline(file, line))
		textInFile.push_back(line);
	file.close();
	
	//Converting the vector to a string
	string s;
	for(int i=0; i<textInFile.size(); i++){
		s += textInFile.at(i);
	}
	
	fileLen = s.length();
	
	matchingLineNum = new int[fileLen];
	everyChar = new char[fileLen];
	
	int track = 0;
	int lineNum = 0;
	
	//This sets the line number at the index of every character
	file.open(filename.c_str());
	while(getline(file, line)){
		int temp = 0;
		int orig = track;
		for(int i=orig; i<orig+(line.length()); i++){
			matchingLineNum[i] = lineNum;
			++track;
		}
		++lineNum;
	}
	file.close();
	
	//This places every charecter of the file into an array
	int counter = 0;
	for(int i=0; i<textInFile.size(); i++){
		string lne = textInFile.at(i);
		for(int j=0; j<lne.length(); j++){
			everyChar[counter] = lne.at(j);
			++counter;
		}
	}
	
	checkText(everyChar, "world", containsWord);
	delete matchingLineNum;
	delete everyChar;
}

string checkText (char *everyChar, string word, vector<string>& matches) {
	int fd1[2];
	int fd2[2];
	char* matchingLine = (char*)calloc(8501, sizeof(char));
	
	pid_t p;

	//Pipe failed
	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		cout << "PIPE FAILED" << endl;
		return "PIPE FAILED";
	}

	p = fork();
	
	//Fork failed
	if (p < 0) {
		cout << "FORK FAILED" << endl;
		return "FORK FAILED";
	}
	
	//Parent Proccess 
	else if (p > 0) {
		close(fd1[0]);
		//Sends everyChar to the Child
		write(fd1[1], everyChar, fileLen);
		close(fd1[1]);
		//Waits for the child to exit 
		wait(NULL);
		close(fd2[1]);
		
		//Reads matchingLine from child
		read(fd2[0], matchingLine, 8502);
		
		//Prints the contents
		cout << endl;
		for(int i=0; i<8501; i++)
			cout << matchingLine[i];
		cout << endl;
		delete matchingLine;
		close(fd2[0]);
	}
	
	//Child Proccess
	else {
		close(fd1[1]);
		
		//Reads everyChar from parent 
		read(fd1[0], everyChar, fileLen);
		close(fd1[0]);
		close(fd2[0]);
		
		vector<int> lines;
		vector<string> matches;
		
		//This checks for the word by looping through the charecter array
		for(int i=0; i<fileLen; i++){
			if(everyChar[i] == word[0]){
				if(!(find(lines.begin(), lines.end(), matchingLineNum[i]) != lines.end())){
					
					//It has matched the first char and now loops through to find if it is the correct word
					for(int j=0; j<word.length(); j++){
						if(j == word.length()-1 && i < fileLen-1){
							if(!isalpha(everyChar[i+1])){
								int lt = matchingLineNum[i];
								lines.push_back(lt);
								matches.push_back(textInFile[lt]);
							}
						}
						else if(word.at(j) != everyChar[i])
							j = word.length();
						++i;
					}
				}
			}
		}
		
		//Some lines start with two spaces or a ' or ".
		//This removes them for sorting purposes
		for(int i=0; i<matches.size(); i++){
			if(matches[i].at(0) == ' '&&matches[i].at(1) == ' ')
				matches[i] = matches[i].substr(2);
			if(matches[i].at(0) == '"'|| matches[i].at(0) == '\'')
				matches[i] = matches[i].substr(1);
		}
		
		sort(matches.begin(), matches.end(), compareNoCase);
		
		//Converts the vector to a string
		string ret;
		for(int i=0; i<matches.size(); i++)
			ret += matches[i] + "\n";
			
		matchedLen = ret.length();
		
		//Converts the string to a char array
		matchingLine = const_cast<char*>(ret.c_str());
		
		//Writes back to the parent
		write(fd2[1], matchingLine, 8502);
		close(fd2[1]);
		
		exit(0);
	}
}

bool compareNoCase (string first, string second){
  int i=0;
  
  //Loops throught the strings and checks to make sure they are in the correct order
  while((i < first.length()) && (i < second.length())){
	if(tolower(first[i]) < tolower(second[i])) 
		return true;
	else if(tolower(first[i]) > tolower(second[i])) 
		return false;
	i++;
  }

  //If they are teh same up to a certain point but one is longer than the other, return respectively
  if(first.length() < second.length()) 
	return true;
  else 
	return false;
}

int main(){
	openFile("test.txt");
	return 0;
}
