#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ofstream out;

void sortida(string output, int inici, int penalitzacio, ...)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << penalitzacio << ' ' << temps << endl;
    out.close();
}

int main(int argc, char **argv)
{
    int inici = clock();
    string input = string(argv[1]);
    ifstream f(input);
    int C, M, K;
    f >> C >> M >> K;
    for (int i = 0; i < M; i++)
    {
        // ns si haurem de fer un vector o crear una struct d millores (crec q vector)
        f >> ce;
    }
    for (int i = 0; i < M; i++)
    {
        // lo mismo q amb ce
        f >> ne;
    }
    for (int i = 0; i < K; i++)
    {
        // lo mismo q amb ce
        f >> id >> cotxes;
        for (int i = 0; i < M; i++)
        {
            f >> millora;
        }
    }

    f.close();
}
