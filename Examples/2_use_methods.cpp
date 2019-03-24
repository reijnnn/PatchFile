#include <iostream>
#include "../PatchFile.h"

int main(int argc, char* argv[]) {

	PatchFile *patch = new PatchFile();

	// -create old_file.txt new_file.txt patch_file.txt
	patch->createPatch("old_file.txt", "new_file.txt", "patch_file.txt");

	// -merge old_file.txt patch_file.txt new_patch_file.txt
	patch->mergePatch("old_file.txt", "patch_file.txt", "new_patched_file.txt");

	return 0;
}
