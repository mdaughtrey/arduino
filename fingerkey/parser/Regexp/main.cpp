#include <iostream>
#include <fstream>
#include <string>
#include "Regexp.h"
#include "parser.h"

using namespace std;

int main(int argc, char ** argv)
{
    if (2 != argc)
    {
        cerr << "Usage: exec filename" << endl;
        return 1;
    }
    ifstream infile(argv[1]);
    if (!infile.is_open())
    {
        cerr << "Cannot open file " << argv[1] << endl;
        return 1;
    }
    string line;
    while (getline(infile, line))
    {
        cout << "Input: " << line << endl;
    }
    infile.close();
    cout << "Done" << endl;
    return 0;
}
