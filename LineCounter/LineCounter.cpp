// LineCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

#pragma comment(lib, "User32.lib")

int linesInFile(WIN32_FIND_DATA);
int linesInDirectory(const char*);
int linesInDirectoryRecursive(const char*);

int main(int argc, char* argv[])
{
	string dir;
	if (argc != 2)
	{
		dir = _getcwd(nullptr, 0);
		printf("Warning: No directory given, assuming %s\n", dir.c_str());
		printf("Standard usage: %s <directory path>\n\n", argv[0]);
	}
	else
		dir = argv[1];
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

