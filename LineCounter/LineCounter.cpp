// LineCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

#pragma comment(lib, "User32.lib")

int linesInFile(WIN32_FIND_DATA);
int linesInDirectory(const char*);
int linesInDirectoryRecursive(const char*);

void help(const char* prog);


int main(int argc, char* argv[])
{
	string dir;
	bool recursive(false);
	if (argc == 1)
	{
		dir = _getcwd(nullptr, 0);
		printf("Warning: No directory given, assuming %s\n", dir.c_str());
		help(argv[0]);
		cout << endl;
	}
	else if (argc == 2)
		dir = argv[1];
	else if (argc==3)
	{
		size_t dirpos(1);
		recursive = true;
		if (!strcmp(argv[dirpos], "/r"))
			dirpos = 2;
		else if (strcmp(argv[2], "/r"))
		{
			printf("Invalid arguments \"%s\", \"%s\"\n", argv[1], argv[2]);
			help(argv[0]);
			return 127;
		}
		dir = argv[dirpos];
	}
	else
	{
		cout << "Invalid arguments \"" << argv[1] << "\"";
		for (int i = 2; i < argc; ++i)
			cout << ", \"" << argv[i] << "\"";
		help(argv[0]);
		return 127;
	}
	if (recursive)
		return linesInDirectoryRecursive(dir) == -1;
	else
		return linesInDirectory(dir);
}

int linesInDirectory(string dir)
{
	WIN32_FIND_DATA	ffd;

	cout << "Target directory is " << dir << endl;

	_chdir(dir.c_str());
	dir += "\\*";

	HANDLE find = INVALID_HANDLE_VALUE;
	find = FindFirstFile(dir.c_str(), &ffd);

	if (find == INVALID_HANDLE_VALUE)
	{
		cerr << "No files found!\n";
		return 1;
	}

	uintmax_t sum(0);
	size_t len(0);

	do
	{
		len = linesInFile(ffd);
		sum += len;
		cout << "\tFile " << ffd.cFileName << ": " << len << " lines.\n";
		cout << "\t\tRunning total: " << sum << endl << endl;
	} while (FindNextFile(find, &ffd));

	char msg[9001];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | 80, NULL, GetLastError(), 0, msg, 9001, NULL);
	cout << "\n\n" << msg;

	cout << "\n\nTotal number of lines in files in the directory " << _getcwd(NULL, NULL) << ": " << sum << ".\n";
	return 0;
}

int linesInDirectoryRecursive(string dir)
{
	WIN32_FIND_DATA	ffd;

	cout << "Target directory is " << dir << endl;

	_chdir(dir.c_str());
	dir += "\\*";

	HANDLE find = INVALID_HANDLE_VALUE;
	find = FindFirstFile(dir.c_str(), &ffd);

	if (find == INVALID_HANDLE_VALUE)
	{
		cerr << "No files found!\n";
		return -1;
	}

	uintmax_t sum(0);
	size_t len(0);

	do
	{
		if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			cout << "\n\nFound directory " << ffd.cFileName << ".\n";
			cout << "Counting lines in directory...\n\n";
			len = linesInDirectoryRecursive(dir);
			if (len != -1)
				sum += len;
			cout << "Done with subdirectory " << ffd.cFileName << ".\n";
			cout << "\tRunning total of lines in " << dir << ": " << sum << ".\n\n";
		}
		else
		{
			len = linesInFile(ffd);
			sum += len;
			cout << "\tFile " << ffd.cFileName << ": " << len << " lines.\n";
			cout << "\t\tRunning total: " << sum << endl << endl;
		}
	} while (FindNextFile(find, &ffd));

	char msg[9001];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | 80, NULL, GetLastError(), 0, msg, 9001, NULL);
	cout << "\n\n" << msg;

	cout << "\n\nTotal number of lines in files in the directory " << _getcwd(NULL, NULL) << ": " << sum << ".\n";
	return sum;
}

int linesInFile(WIN32_FIND_DATA ffd)
{
	ifstream in;

	if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		return 0;


	in.open(ffd.cFileName, ios::in);
	size_t len(0);
	string line;
	while (in.good())
	{
		getline(in, line);
		++len;
	}
	in.clear();
	in.close();

	return len;
}

void help(const char* prog)
{
	cout << "LineCounter: Counts the number of lines in all files of the chosen directory.\n";
	cout << "Usage: " << prog << " [directory_path] [/r]\n";
	cout << "    directory_path: The path to the directory in which to count lines\n";
	cout << "        If omitted, will print a warning message and count files in the current\n";
	cout << "        working directory.\n";
	cout << "    /r: Recursive mode\n";
	cout << "        By default, LineCounter will only count files directly in the directory\n";
	cout << "        given: That is, it will produce a flat count.\n";
	cout << "        Passing this flag will instruct it to count files in all subdirectories\n";
	cout << "        as well: A recursive count.\n";
}

