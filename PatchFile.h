#ifndef PATCHFILE_H
#define PATCHFILE_H

class PatchFile
{
private:

	void printHelp();
	void printUseHelp();
	bool fileExists(const std::string& filename);
	long long calcStringHash(std::string &str);

public:

	PatchFile();
	int startPatchUseCMD(int argc, char* argv[]);
	bool createPatch(const std::string& fileNameOld, const std::string& fileNameNew, const std::string& fileNamePatch);
	bool mergePatch(const std::string& fileNameOld, const std::string& fileNamePatch, const std::string& fileNameNew);
};

#endif
