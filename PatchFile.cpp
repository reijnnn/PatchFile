#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

vector<int> PatchFile::calcLastRowDistance(int stOldPos, int enOldPos, int stNewPos, int enNewPos) {

   const int N_ROWS = 2;

   int sizeOld = enOldPos - stOldPos + 1;
   int sizeNew = enNewPos - stNewPos + 1;

   int **dp = new int*[N_ROWS];
   for(int i = 0; i < N_ROWS; i++) {
      dp[i] = new int[sizeNew + 1];
   }

   int activeRow = 0;
   for(int j = 0; j <= sizeNew; j++) {
      dp[activeRow][j] = j;
   }

   for(int i = 1; i <= sizeOld; i++) {

      activeRow = 1 - activeRow;

      dp[activeRow][0] = i;
      dp[1 - activeRow][0] = i - 1;

      for(int j = 1; j <= sizeNew; j++) {
         dp[activeRow][j] = min(dp[1 - activeRow][j] + 1, dp[activeRow][j - 1] + 1);
         dp[activeRow][j] = min(dp[activeRow][j], dp[1 - activeRow][j - 1] + (vecHashOld[stOldPos + i - 1] != vecHashNew[stNewPos + j - 1]));
      }
   }

   vector<int> vecLastRow;
   for(int j = 1; j <= sizeNew; j++) {
      vecLastRow.push_back(dp[activeRow][j]);
   }

   return vecLastRow;
}

vector<int> PatchFile::calcLastRowDistanceInverse(int stOldPos, int enOldPos, int stNewPos, int enNewPos) {

   const int N_ROWS = 2;

   int sizeOld = enOldPos - stOldPos + 1;
   int sizeNew = enNewPos - stNewPos + 1;

   int **dp = new int*[N_ROWS];
   for(int i = 0; i < N_ROWS; i++) {
      dp[i] = new int[sizeNew + 1];
   }

   int activeRow = 0;
   for(int j = 0; j <= sizeNew; j++) {
      dp[activeRow][j] = sizeNew - j;
   }

   for(int i = sizeOld - 1; i >= 0; i--) {

      activeRow = 1 - activeRow;

      dp[activeRow][sizeNew] = sizeOld - i;
      dp[1 - activeRow][sizeNew] = sizeOld - i - 1;

      for(int j = sizeNew - 1; j >= 0; j--) {
         dp[activeRow][j] = min(dp[1 - activeRow][j] + 1, dp[activeRow][j + 1] + 1);
         dp[activeRow][j] = min(dp[activeRow][j], dp[1 - activeRow][j + 1] + (vecHashOld[i + stOldPos] != vecHashNew[j + stNewPos]));
      }
   }

   vector<int> vecLastRow;
   for(int j = 0; j < sizeNew; j++) {
      vecLastRow.push_back(dp[activeRow][j]);
   }

   return vecLastRow;
}

string PatchFile::prepareFileRow(char operation, int row, string text) {
   stringstream ss;
   ss << operation << " (" << row << "):" << text;
   return ss.str();
}

vector<string> PatchFile::calcMinPrescription(int stOldPos, int enOldPos, int stNewPos, int enNewPos) {

   int sizeOld = enOldPos - stOldPos + 1;
   int sizeNew = enNewPos - stNewPos + 1;

   vector<string> vecPrescription;

   if(stOldPos > enOldPos) {
      for(int i = stNewPos; i <= enNewPos; i++) {
         vecPrescription.push_back(prepareFileRow('+', i, vecFileNew[i]));
      }
      return vecPrescription;
   }

   if(stNewPos > enNewPos) {
      for(int i = stOldPos; i <= enOldPos; i++) {
         vecPrescription.push_back(prepareFileRow('-', i, vecFileOld[i]));
      }
      return vecPrescription;
   }

   if(sizeNew == 1 || sizeOld == 1) {
      if(sizeOld > 1) {
         int indEqual = enOldPos;
         for(int i = stOldPos; i <= enOldPos; i++) {
            if(vecHashOld[i] == vecHashNew[stNewPos]) {
               indEqual = i;
               break;
            }
         }
         for(int i = stOldPos; i <= enOldPos; i++) {
            if(i == indEqual) {
               if(vecHashOld[i] != vecHashNew[stNewPos]) {
                  vecPrescription.push_back(prepareFileRow('^', i, vecFileNew[stNewPos]));
               }
            } else {
               vecPrescription.push_back(prepareFileRow('-', i, vecFileOld[i]));
            }
         }
      } else if(sizeNew > 1) {
         int indEqual = enNewPos;
         for(int i = stNewPos; i <= enNewPos; i++) {
            if(vecHashOld[stOldPos] == vecHashNew[i]) {
               indEqual = i;
               break;
            }
         }
         for(int i = stNewPos; i <= enNewPos; i++) {
            if(i == indEqual) {
               if(vecHashOld[stOldPos] != vecHashNew[i]) {
                  vecPrescription.push_back(prepareFileRow('^', stOldPos, vecFileNew[i]));
               }
            } else {
               vecPrescription.push_back(prepareFileRow('+', i, vecFileNew[i]));
            }
         }
      } else {
         if(vecHashOld[stOldPos] != vecHashNew[stNewPos]) {
            vecPrescription.push_back(prepareFileRow('^', stOldPos, vecFileNew[stNewPos]));
         }
      }
      return vecPrescription;
   }

   vector<string> vecLePrescription, vecRiPrescription;

   if(sizeOld > sizeNew) {
      int midOldPos = (enOldPos + stOldPos) / 2;

      int leOldSize = midOldPos - stOldPos + 1;
      int riOldSize = sizeOld - leOldSize;

      vector<int> vecLastRowDist = calcLastRowDistance(stOldPos, midOldPos, stNewPos, enNewPos);
      vector<int> vecLastRowDistInverse = calcLastRowDistanceInverse(midOldPos + 1, enOldPos, stNewPos, enNewPos);

      int minInd;
      int minVal;
      for(int i = -1; i < sizeNew; i++) {
         if(i == -1) {
            minInd = i;
            minVal = vecLastRowDistInverse[i + 1] + leOldSize;
         } else if(i == sizeNew - 1) {
            if(vecLastRowDist[i] + riOldSize < minVal) {
               minInd = i;
               minVal = vecLastRowDist[i] + riOldSize;
            }
         } else {
            if(vecLastRowDist[i] + vecLastRowDistInverse[i + 1] < minVal) {
               minInd = i;
               minVal = vecLastRowDist[i] + vecLastRowDistInverse[i + 1];
            }
         }
      }

      vecLePrescription = calcMinPrescription(stOldPos, midOldPos, stNewPos, stNewPos + minInd);
      vecRiPrescription = calcMinPrescription(midOldPos + 1, enOldPos, stNewPos + minInd + 1, enNewPos);

   } else {
      int midNewPos = (enNewPos + stNewPos) / 2;

      int leNewSize = midNewPos - stNewPos + 1;
      int riNewSize = sizeNew - leNewSize;

      vector<int> vecLastRowDist = calcLastRowDistance(stNewPos, midNewPos, stOldPos, enOldPos);
      vector<int> vecLastRowDistInverse = calcLastRowDistanceInverse(midNewPos + 1, enNewPos,  stOldPos, enOldPos);

      int minInd;
      int minVal;
      for(int i = -1; i < sizeOld; i++) {
         if(i == -1) {
            minInd = i;
            minVal = vecLastRowDistInverse[i + 1] + leNewSize;
         } else if(i == sizeOld - 1) {
            if(vecLastRowDist[i] + riNewSize < minVal) {
               minInd = i;
               minVal = vecLastRowDist[i] + riNewSize;
            }
         } else {
            if(vecLastRowDist[i] + vecLastRowDistInverse[i + 1] < minVal) {
               minInd = i;
               minVal = vecLastRowDist[i] + vecLastRowDistInverse[i + 1];
            }
         }
      }

      vecLePrescription = calcMinPrescription(stOldPos, stOldPos + minInd, stNewPos, midNewPos);
      vecRiPrescription = calcMinPrescription(stOldPos + minInd + 1, enOldPos, midNewPos + 1, enNewPos);
   }

   vecLePrescription.insert(vecLePrescription.end(), vecRiPrescription.begin(), vecRiPrescription.end());

   return vecLePrescription;
}

bool PatchFile::createPatch(const string& fileNameOld, const string& fileNameNew, const string& fileNamePatch) {

   string line;

   vecFileOld.clear();
   vecHashOld.clear();

   ifstream fileOld;
   fileOld.open(fileNameOld.c_str());

   if(!fileOld.is_open()) {
      cerr << "Can not create or open file " << fileNameOld << endl;
      return false;
   }

   int cntRowFileOld = 0;
   while(getline(fileOld, line)) {
      vecFileOld.push_back(line);
      vecHashOld.push_back(calcStringHash(line));
      cntRowFileOld++;
   }
   fileOld.close();

   vecFileNew.clear();
   vecHashNew.clear();

   ifstream fileNew;
   fileNew.open(fileNameNew.c_str());

   if(!fileNew.is_open()) {
      cerr << "Can not create or open file " << fileNameNew << endl;
      return false;
   }

   int cntRowFileNew = 0;
   while(getline(fileNew, line)) {
      vecFileNew.push_back(line);
      vecHashNew.push_back(calcStringHash(line));
      cntRowFileNew++;
   }
   fileNew.close();

   vector<string> vecPatch = calcMinPrescription(0, cntRowFileOld - 1, 0, cntRowFileNew - 1);

   ofstream filePatch;
   filePatch.open(fileNamePatch.c_str());
   if(!filePatch.is_open()) {
      cerr << "Can not create or open file " << fileNamePatch << endl;
      return false;
   }

   for(int i = 0; i < (int)vecPatch.size(); i++) {
      filePatch << vecPatch[i] << endl;
   }

   filePatch.close();

   return true;
}

bool PatchFile::mergePatch(const string& fileNameOld, const string& fileNamePatch, const string& fileNameNew) {

   string line;

   vector<string> vecFileOld;

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

   vector<string> vecFileNew;

   int indFileOld = 0;
   int indFilePatch = 0;
   int indFilePatchPred = -1;

   int filePatchRowNum;
   char filePatchRowOperType;
   string filePatchRowData;

   while(indFileOld < cntRowFileOld) {

      if(indFilePatch != indFilePatchPred) {
         if(indFilePatch == cntRowFilePatch) {
            filePatchRowOperType = 'X';
         } else {
            sscanf(vecFilePatch[indFilePatch].c_str(), "%*s (%d):", &filePatchRowNum);
            filePatchRowData = vecFilePatch[indFilePatch].substr(vecFilePatch[indFilePatch].find(":") + 1, -1);
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
      filePatchRowData = vecFilePatch[indFilePatch].substr(vecFilePatch[indFilePatch].find(":") + 1, -1);
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
