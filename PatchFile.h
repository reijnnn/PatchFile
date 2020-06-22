#ifndef PATCHFILE_H
#define PATCHFILE_H

#include <vector>
#include <string>

class PatchFile
{
private:

   std::vector<std::string> vecFileOld, vecFileNew;
   std::vector<long long> vecHashOld, vecHashNew;

   void printHelp();
   void printUseHelp();
   bool fileExists(const std::string& filename);
   long long calcStringHash(std::string &str);
   std::string prepareFileRow(char operation, int row, std::string text);
   std::vector<std::string> calcMinPrescription(int stOldPos, int enOldPos, int stNewPos, int enNewPos);
   std::vector<int> calcLastRowDistanceInverse(int stOldPos, int enOldPos, int stNewPos, int enNewPos);
   std::vector<int> calcLastRowDistance(int stOldPos, int enOldPos, int stNewPos, int enNewPos);

public:

   PatchFile();
   int startPatchUseCMD(int argc, char* argv[]);
   bool createPatch(const std::string& fileNameOld, const std::string& fileNameNew, const std::string& fileNamePatch);
   bool mergePatch(const std::string& fileNameOld, const std::string& fileNamePatch, const std::string& fileNameNew);
};

#endif
