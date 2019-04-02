#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "../PatchFile.h"

using namespace std;

int main(int argc, char* argv[]) {

	srand (time(NULL));

	const string TEST_NAME = "test_1";
	int TEST_COUNT    = 10;
	int MAX_FILE_ROWS = 3000;

	if(argc >= 2) {
		TEST_COUNT = atoi(argv[1]);
	}
	if(argc >= 3) {
		MAX_FILE_ROWS = atoi(argv[2]);
	}

	const string OLD_FILE_NAME   	   = TEST_NAME + "_old_file.txt";
	const string NEW_FILE_NAME   	   = TEST_NAME + "_new_file.txt";
	const string PATCH_FILE_NAME 	   = TEST_NAME + "_patch_file.txt";
	const string NEW_PATCHED_FILE_NAME = TEST_NAME + "_new_patched_file.txt";

	// Generating test data
	while(TEST_COUNT--) {

		int ROW_FILE_COUNT = rand() % MAX_FILE_ROWS + 1;

		fstream fileOld;
		fileOld.open(OLD_FILE_NAME.c_str(), fstream::out);
		if(!fileOld.is_open()) {
			cerr << "Can not create or open file " << OLD_FILE_NAME << endl;
			return 1;
		}

		fstream fileNew;
		fileNew.open(NEW_FILE_NAME.c_str(), fstream::out);
		if(!fileNew.is_open()) {
			cerr << "Can not create or open file " << NEW_FILE_NAME << endl;
			return 1;
		}

		for(int i = 0; i < ROW_FILE_COUNT; i++) {

			int operType = rand()  % 16 + 1;
			int rowData	 = (rand() % 10000000 + 10);

			if(operType == 1) {
				// delete row
			} else if(operType == 2) {
				// edit row
				fileNew << rowData << " " << "edit" << endl;
			} else if(operType == 3) {
				// add row
				fileNew << rowData << " " << "add" << endl;
			} else {
				fileNew << rowData << endl;
			}

			fileOld << rowData << endl;
		}

		fileOld.close();
		fileNew.close();

		// Emulating on test data
		PatchFile *patch = new PatchFile();

		cout << "Starting new test with param: " << "ROW_FILE_COUNT = " << ROW_FILE_COUNT << endl;

		clock_t time_start = clock();

		patch->createPatch(OLD_FILE_NAME, NEW_FILE_NAME, PATCH_FILE_NAME);
		patch->mergePatch(OLD_FILE_NAME, PATCH_FILE_NAME, NEW_PATCHED_FILE_NAME);

		clock_t time_end = clock();

		cout << "Finished. Time elapsed: " << double(time_end - time_start) / CLOCKS_PER_SEC << endl;

		// Validating result
		fileNew.open(NEW_FILE_NAME.c_str(), fstream::in);
		if(!fileNew.is_open()) {
			cerr << "Can not create or open file " << NEW_FILE_NAME << endl;
			return 1;
		}

		vector<string> vecFileNew;
		string line;
		while(getline(fileNew, line)) {
		   vecFileNew.push_back(line);
		}
		fileNew.close();

		fstream fileNewPatched;
		fileNewPatched.open(NEW_PATCHED_FILE_NAME.c_str(), fstream::in);
		if(!fileNewPatched.is_open()) {
			cerr << "Can not create or open file " << NEW_PATCHED_FILE_NAME << endl;
			return 1;
		}

		vector<string> vecFileNewPatched;
		while(getline(fileNewPatched, line)) {
		   vecFileNewPatched.push_back(line);
		}
		fileNewPatched.close();

		if((int)vecFileNewPatched.size() != (int)vecFileNew.size()) {
			cerr << NEW_FILE_NAME << " and " << NEW_PATCHED_FILE_NAME << " are having different size" << endl;
			return 1;
		}

		for(int i = 0; i < (int)vecFileNew.size(); i++) {
			if(vecFileNew[i] != vecFileNewPatched[i]) {
				cerr << NEW_FILE_NAME << " and " << NEW_PATCHED_FILE_NAME << " are having difference at line " << i + 1 << endl;
				return 1;
			}
		}
	}

	return 0;
}
