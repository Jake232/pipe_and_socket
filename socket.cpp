#include "socket.h"

//The reason these are global is that they are used in every function minus the main
int* matchingLineNumSocket;
char* everyCharSocket;
vector<string> textInFileSocket;
int fileLength;
int matchedLength;
void openFile (string filename) {
	vector<string> matchedLines;
	
	string line;
	ifstream file;
	
	//Opening file and placing the contents into a vector
	file.open(filename.c_str());
	while(getline(file, line))
		textInFileSocket.push_back(line);
	file.close();
	
	//Converting the vector to a string
	string s;
	for(int i=0; i<textInFileSocket.size(); i++){
		s += textInFileSocket.at(i);
	}
	fileLength = s.length();
	matchingLineNumSocket = new int[fileLength];
	everyCharSocket = new char[fileLength];
	
	//This sets the line number at the index of every character
	int track = 0;
	int lineNum = 0;
	file.open(filename.c_str());
	while(getline(file, line)){
		int temp = 0;
		int orig = track;
		for(int i=orig; i<orig+(line.length()); i++){
			matchingLineNumSocket[i] = lineNum;
			++track;
		}
		++lineNum;
	}
	file.close();
	
	//This places every charecter of the file into an array
	int count = 0;
	for(int i=0; i<textInFileSocket.size(); i++){
		string lne = textInFileSocket.at(i);
		for(int j=0; j<lne.length(); j++){
			everyCharSocket[count] = lne.at(j);
			++count;
		}
	}
	checkText(everyCharSocket, filename, "world", matchedLines);
	delete matchingLineNumSocket;
	delete everyCharSocket;
}

string checkText (char *everyCharSocket, string filename, string word, vector<string>& matches) {
	int fd[2];
	char* matchingLines = (char*)calloc(8501, sizeof(char));
	
	pid_t p;

	//Socket Failed
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1) {
		cout << "SOCKET FAILED" << endl;
		return "SOCKET FAILED";
	}

	p = fork();

	//Fork Failed
	if (p < 0) {
		cout << "FORK FAILED" << endl;
		return "FORK FAILED";
	}
	
	//Parent
	else if (p > 0) {
		//Write to child
		write(fd[0], everyCharSocket, fileLength);
		
		//Wait for child to exit 
		wait(NULL);
		
		//Read from Child
		read(fd[0], matchingLines, 8502);
		
		//Print the result
		cout << endl;
		for(int i=0; i<8501; i++)
			cout << matchingLines[i];
		cout << endl;
		delete matchingLines;
		close(fd[0]);
	}
	else {
		//Read from Parent
		read(fd[1], everyCharSocket, fileLength);
		vector<int> lines;
		vector<string> matches;
		
		//This checks for the word by looping through the charecter array
		for(int i=0; i<fileLength; i++){
			if(everyCharSocket[i] == word[0]){
				if(!(find(lines.begin(), lines.end(), matchingLineNumSocket[i]) != lines.end())){
					
					//It has matched the first char and now loops through to find if it is the correct word
					for(int j=0; j<word.length(); j++){
						if(j == word.length()-1 && i < fileLength-1){
							if(!isalpha(everyCharSocket[i+1])){
								int lt = matchingLineNumSocket[i];
								lines.push_back(lt);
								matches.push_back(textInFileSocket[lt]);
							}
						}
						else if(word.at(j) != everyCharSocket[i])
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
		sort (matches.begin(), matches.end(), compareNoCase);
		
		//Converts the vector to a string
		string ret;
		for(int i=0; i<matches.size(); i++)
			ret += matches[i] + "\n";
		matchedLength = ret.length();
		
		//Converts the string to a char array
		matchingLines = const_cast<char*>(ret.c_str());
		
		//Writes back to the parent
		write(fd[1], matchingLines, 8502);
		close(fd[1]);
		
		exit(0);
	}
}

	bool compareNoCase (string first, string second){
	  int i=0;
	  while ((i < first.length()) && (i < second.length())){
		if (tolower (first[i]) < tolower (second[i])) return true;
		else if (tolower (first[i]) > tolower (second[i])) return false;
		i++;
	  }

	  if (first.length() < second.length()) return true;
	  else return false;
}

int main(){
	openFile("test.txt");
	return 0;
}
