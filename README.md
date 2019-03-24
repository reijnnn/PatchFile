# PatchFile

Create a patch-file using old file version and new file version.

Merge a patch-file and old file version for building new file version.

## Getting Started

To use functions in your project you need to include PatchFile.h.

You may use an app with command line arguments, for example:

```
patchfile -create old_file.txt new_file.txt patch_file.txt
patchfile -merge old_file.txt patch_file.txt new_patched_file.txt
```

and call:

```
PatchFile *patch = new PatchFile();
patch->startPatchUseCMD(argc, argv);
```

You may call functions directly:

```
PatchFile *patch = new PatchFile();
patch->createPatch("old_file.txt", "new_file.txt", "patch_file.txt");
patch->mergePatch("old_file.txt", "patch_file.txt", "new_patched_file.txt");
```

See Examples/

## Running the tests

To check algorithms work use tests.

See Tests/
