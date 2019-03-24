#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PatchFile.h"

using namespace std;

void PatchFile::printHelp() {
	cerr << "Options:\n"
		<< "\t-h,--help\tShow help message\n"
		<< "\t-create <old file name> <new file name> <patch file name>\t Create patch using 'old' and 'new' files\n"
		<< "\t-merge <old file name> <patch file name> <new file name>\t Create new version of file using 'old' file and patch"
 		<< endl;
}

void PatchFile::printUseHelp() {
	cerr << "Use --help command to display available options" << endl;
}

bool PatchFile::fileExists(const string& filename) {
	ifstream ifile(filename.c_str());
	return (bool)ifile;
}

long long PatchFile::calcStringHash(string &str) {
	const int MOD = 1e9 + 7;
	long long res = 0;
	for(int i = 0; i < str.size(); i++) {
		res += ((res * 31LL) % MOD + (str[i] - 'a' + 1)) % MOD;
	}
	return res;
}

PatchFile::PatchFile() {
}

int PatchFile::startPatchUseCMD(int argc, char* argv[]) {
	if (argc == 2 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")) {
		printHelp();
		return 0;
	} else if (argc != 5) {
		printUseHelp();
		return 1;
	} else {

		if(!(string(argv[1]) == "-create" || string(argv[1]) == "-merge")) {
			cerr << "-create or -merge expected but " << argv[1] << " found" << endl;
			printUseHelp();
			return 1;
		}

		if(!fileExists(string(argv[2]))) {
			cerr << "File '" << argv[2] << "' does not exist" << endl;
			return 1;
		}

		if(!fileExists(string(argv[3]))) {
			cerr << "File '" << argv[3] << "' does not exist" << endl;
			return 1;
		}

		if(string(argv[1]) == "-create") {
			if(createPatch(string(argv[2]), string(argv[3]), string(argv[4]))) {
				cout << "Patch-file successfully created: " << string(argv[4]) << endl;
			} else {
				cerr << "An error occurred while creating the patch-file" << endl;
				return 1;
			}
		}

		if(string(argv[1]) == "-merge") {
			if(mergePatch(string(argv[2]), string(argv[3]), string(argv[4]))) {
				cout << "New version of file successfully created: " << string(argv[4]) << endl;
			} else {
				cerr << "An error occurred while creating the new version of file" << endl;
				return 1;
			}
		}
	}
}

bool PatchFile::createPatch(const string& fileNameOld, const string& fileNameNew, const string& fileNamePatch) {

	string line;

	vector<string> vecFileOld;
	vector<long long> vecHashOld;

	ifstream fileOld;
	fileOld.open(fileNameOld.c_str());

	if(!fileOld.is_open()) {
		cerr << "Can not create or open file " << fileNameOld << endl;
		return false;
	}

	// Loading old file, calculating for each row a hash value
	int cntRowFileOld = 0;
	while(getline(fileOld, line)) {
		vecFileOld.push_back(line);
		vecHashOld.push_back(calcStringHash(line));
		cntRowFileOld++;
	}
	fileOld.close();

	vector<string> vecFileNew;
	vector<long long> vecHashNew;

	ifstream fileNew;
	fileNew.open(fileNameNew.c_str());

	if(!fileNew.is_open()) {
		cerr << "Can not create or open file " << fileNameNew << endl;
		return false;
	}

	// Loading new file, calculating for each row a hash value
	int cntRowFileNew = 0;
	while(getline(fileNew, line)) {
		vecFileNew.push_back(line);
		vecHashNew.push_back(calcStringHash(line));
		cntRowFileNew++;
	}
	fileNew.close();

	// Initializing structures for path and calculation
	pair<int, int> **parent = new pair<int, int>*[cntRowFileOld + 1];
	for(int i = 0; i <= cntRowFileOld; i++) {
		parent[i] = new pair<int, int>[cntRowFileNew + 1];
	}

	int **dp = new int*[cntRowFileOld + 1];
	for(int i = 0; i <= cntRowFileOld; i++) {
		dp[i] = new int[cntRowFileNew + 1];
	}

	for(int i = 0; i <= cntRowFileOld; i++) {
		dp[i][0] = i;
	}
	for(int i = 0; i <= cntRowFileNew; i++) {
		dp[0][i] = i;
	}

	// Searching minimum difference between two files
	for(int i = 1; i <= cntRowFileOld; i++) {
		for(int j = 1; j <= cntRowFileNew; j++) {

			dp[i][j] = dp[i - 1][j] + 1;
			parent[i][j] = make_pair(i - 1, j);

			if(dp[i][j - 1] + 1 < dp[i][j]) {
				dp[i][j] = dp[i][j - 1] + 1;
				parent[i][j] = make_pair(i, j - 1);
			}

			int isEqualRows = (vecHashOld[i - 1] == vecHashNew[j - 1]);
			if(dp[i][j] > dp[i - 1][j - 1] + !isEqualRows) {
				dp[i][j] = dp[i - 1][j - 1] + !isEqualRows;
				parent[i][j] = make_pair(i - 1, j - 1);
			}
		}
	}

	// Recovering the shortest path between two files
	vector<pair<int, int> > vecPath;

	int indx = cntRowFileOld;
	int indy = cntRowFileNew;

	while(indx >= 0 && indy >= 0) {

		vecPath.push_back(make_pair(indx, indy));

		if(indx == 0) {
			indy--;
			continue;
		}

		if(indy == 0) {
			indx--;
			continue;
		}

		int indxNew = parent[indx][indy].first;
		int indyNew = parent[indx][indy].second;

		indx = indxNew;
		indy = indyNew;
	}

	ofstream filePatch;
	filePatch.open(fileNamePatch.c_str());
	if(!filePatch.is_open()) {
		cerr << "Can not create or open file " << fileNamePatch << endl;
		return false;
	}

	// Building patch-file
	for(int i = (int)vecPath.size() - 1; i > 0; i--) {
		int x = vecPath[i].first;
		int y = vecPath[i].second;

		int dx = x - vecPath[i - 1].first;
		int dy = y - vecPath[i - 1].second;

		if(dx && dy) {
			if(vecHashOld[x] != vecHashNew[y]) {
				filePatch << "^ (" << x << "):" << vecFileNew[y] << endl;
			}
		} else {
			if(dx) {
				filePatch << "- (" << x << "):" << vecFileOld[x] << endl;
			} else if(dy) {
				filePatch << "+ (" << y << "):" << vecFileNew[y] << endl;
			}
		}
	}

	filePatch.close();

	return true;
}

bool PatchFile::mergePatch(const string& fileNameOld, const string& fileNamePatch, const string& fileNameNew) {

	string line;

	vector<string> vecFileOld;

	// Loading old file
	ifstream fileOld;
	fileOld.open(fileNameOld.c_str());

	if(!fileOld.is_open()) {
		cerr << "Can not create or open file " << fileNameOld << endl;
		return false;
	}

	int cntRowFileOld = 0;
	while(getline(fileOld, line)) {
		vecFileOld.push_back(line);
		cntRowFileOld++;
	}
	fileOld.close();

	vector<string> vecFilePatch;

	// Loading patch-file
	ifstream filePatch;
	filePatch.open(fileNamePatch.c_str());

	if(!filePatch.is_open()) {
		cerr << "Can not create or open file " << fileNamePatch << endl;
		return false;
	}

	int cntRowFilePatch = 0;
	while(getline(filePatch, line)) {
		vecFilePatch.push_back(line);
		cntRowFilePatch++;
	}
	filePatch.close();

	ofstream fileNew;
	fileNew.open(fileNameNew.c_str());
	if(!fileNew.is_open()) {
		cerr << "Can not create or open file " << fileNameNew << endl;
		return false;
	}

	// Building new file version
	vector<string> vecFileNew;

	int indFileOld       = 0;
	int indFilePatch     = 0;
	int indFilePatchPred = -1;

	int    filePatchRowNum;
	char   filePatchRowOperType;
	string filePatchRowData;

	while(indFileOld < cntRowFileOld) {

		if(indFilePatch != indFilePatchPred) {
			if(indFilePatch == cntRowFilePatch) {
				filePatchRowOperType = 'X';
			} else {
				sscanf(vecFilePatch[indFilePatch].c_str(), "%*s (%d):", &filePatchRowNum);
				filePatchRowData     = vecFilePatch[indFilePatch].substr(vecFilePatch[indFilePatch].find(":") + 1, -1);
				filePatchRowOperType = vecFilePatch[indFilePatch][0];
			}
			indFilePatchPred = indFilePatch;
		}

		if(filePatchRowOperType == '+') {
			if((int)vecFileNew.size() == filePatchRowNum) {
				vecFileNew.push_back(filePatchRowData);
				indFilePatch++;
				continue;
			}
		} else if(filePatchRowOperType == '-') {
			if(indFileOld == filePatchRowNum) {
				indFileOld++;
				indFilePatch++;
				continue;
			}
		} else if(filePatchRowOperType == '^') {
			if(indFileOld == filePatchRowNum) {
				vecFileNew.push_back(filePatchRowData);
				indFileOld++;
				indFilePatch++;
				continue;
			}
		}

		vecFileNew.push_back(vecFileOld[indFileOld]);
		indFileOld++;
	}

	while(indFilePatch < cntRowFilePatch) {
		sscanf(vecFilePatch[indFilePatch].c_str(), "%*s (%d):", &filePatchRowNum);
		filePatchRowData     = vecFilePatch[indFilePatch].substr(vecFilePatch[indFilePatch].find(":") + 1, -1);
		filePatchRowOperType = vecFilePatch[indFilePatch][0];

		if(filePatchRowOperType == '+') {
			vecFileNew.push_back(filePatchRowData);
		}

		indFilePatch++;
	}

	for(int i = 0; i < (int)vecFileNew.size(); i++) {
		fileNew << vecFileNew[i] << endl;
	}

	fileNew.close();

	return true;
}
