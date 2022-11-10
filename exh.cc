#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ofstream out;

void sortida(string output, ...)
{
    ofstream out(output);
    out << ... << endl;
    out << ... << endl;
    out.close();
}

int main(int argc, char **argv)
{
    string input = string(argv[1]);
    ifstream f(input);
    f >> ;
    f.close();
}
