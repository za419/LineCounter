// LineCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

#pragma comment(lib, "User32.lib")

int main(int argc, char* argv[])
{
	WIN32_FIND_DATA	ffd;

	string dir;
	if (argc != 2)
	{
		dir = _getcwd(nullptr, 0);
		printf("Warning: No directory given, assuming %s\n", dir.c_str());
		printf("Standard usage: %s <directory path>\n\n", argv[0]);
	}
	else
		dir = argv[1];

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

	ifstream in;

	uintmax_t sum(0);

	do
	{
		if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			continue;


		in.open(ffd.cFileName, ios::in);
		size_t len(0);
		while (in.good())
		{
			getline(in, dir);
			++len;
		}
		in.clear();
		in.close();

		sum += len;

		cout << "\tFile " << ffd.cFileName << ": " << len << " lines.\n";
		cout << "\t\tRunning total: " << sum << endl << endl;
	} while (FindNextFile(find, &ffd));
	
	char msg[9001];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|80, NULL, GetLastError(), 0, msg, 9001, NULL);
	cout << "\n\n" << msg;

	cout << "\n\nTotal number of lines in files in the directory " << _getcwd(NULL,NULL) << ": " << sum << ".\n";
	return 0;
}
