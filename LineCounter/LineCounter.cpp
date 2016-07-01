// LineCounter.cpp : Defines the entry point for the console application.

#include "stdafx.h"
using namespace std;

#pragma comment(lib, "User32.lib")

int linesInFile(WIN32_FIND_DATA); // Returns the number of lines in the file to which the argument is a handle
int linesInDirectory(string); // Returns if an error occurred, exactly like main
int linesInDirectoryRecursive(string); // Returns the number of lines in the directory tree with root directory given, or -1 if an error occurred

void help(const char*); // Prints the help message to cout
bool isHelpFlag(const char*); // Returns whether the passed string is a recognized help flag

int main(int argc, char* argv[])
{
	string dir;
	bool recursive(false); // Whether to use a recursive count
	if (argc == 1) // No arguments: Print warning and help message, then flat-count files in current working directory.
	{
		dir = _getcwd(nullptr, 0);
		fprintf(stderr, "Warning: No directory given, assuming %s\n", dir.c_str());
		help(argv[0]);
		cout << endl;
	}
	else if (argc == 2) // Only one argument given
	{
		if (isHelpFlag(argv[1])) // If its a help flag, print the help message and return
		{
			help(argv[0]);
			return 0;
		}
		else if (strcmp(argv[1], "/r")) // If its the recursive flag, print warning and help message, then recursive-count files in current working directory.
		{
			dir = _getcwd(nullptr, 0);
			recursive = true;
			fprintf(stderr, "Warning: No directory given, assuming %s\n", dir.c_str());
			help(argv[0]);
			cout << endl;
		}
		dir = argv[1]; // Else, assume that the argument is the directory to count
	}
	else if (argc==3) // Two arguments given
	{
		size_t dirpos(1);
		recursive = true; // If the program proceeds, it will be in recursive mode
		if (!strcmp(argv[dirpos], "/r")) // If the first argument is the recursive flag
			dirpos = 2; // The path will be in the second argument
		else if (strcmp(argv[2], "/r")) // If the second argument isn't the recursive flag...
		{
			fprintf(stderr, "Invalid arguments \"%s\", \"%s\"\n", argv[1], argv[2]); // Generate an error,
			help(argv[0]); // Print the help message,
			return 127; // And exit with an error.
		}
		dir = argv[dirpos]; // If we haven't exited, this will set dir to contain the path
	}
	else // If there are more than two arguments, print an error message with all arguments, print the help message, and exit with an error
	{
		cerr << "Invalid arguments \"" << argv[1] << "\"";
		for (int i = 2; i < argc; ++i)
			cerr << ", \"" << argv[i] << "\"";
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

bool isHelpFlag(const char* arg)
{
	return !strcmp(arg, "/h") || !strcmp(arg, "/help") || !strcmp(arg, "/?") || !strcmp(arg, "-h") || !strcmp(arg, "--help");
}

