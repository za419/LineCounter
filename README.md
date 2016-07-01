# LineCounter
A small, very limited, line counting program for windows

Counts all the newlines in all files in a specified directory in a very brute-force manner

Usage: LineCounter.exe [directory_path] [/r]
    directory_path: The path to the directory in which to count lines
        If omitted, will print a warning message and count files in the current working directory
    /r: Recursive mode
        By default, LineCounter will only count files directly in the directory given: That is, it will produce a flat count.
        Passing this flag will instruct it to count files in all subdirectories as well: A recursive count.
