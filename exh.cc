#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

ofstream out;

using VI = vector<int>;
using VVI = vector<VI>;
using VB = vector<bool>;
using VVB = vector<VB>;

VI ce, ne;
VVI linia;
VVB estacions;
int penalitzacio;

void sortida(string output, int inici, ...)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << penalitzacio << ' ' << temps << endl;
    out.close();
}

void backtrack(int& k, vector<int>& solparcial)
{
    int M = ce.size();
    int id = solparcial[k];
    // per a cada estacio afegir si la classe te aquella millora
    for (int m = 0; m < M; ++m) {
        // fila: id de la classe, columna: les M millores
        if (linia[id][m] == 1) {
            estacions[m][k] = true;
        }
        if (k + 1 >= ne[m]) {
            int cotxes_cons = 0;
            for (int j = k + 1 - ne[m]; j < k + 1; ++j)
                if (estacions[m][j])
                    ++cotxes_cons;
            penalitzacio += max(cotxes_cons - ce[m], 0);
        }
    }
}

int main(int argc, char** argv)
{
    int inici = clock();
    string input = string(argv[1]);
    ifstream f(input);

    int C, M, K;
    f >> C >> M >> K;
    // creacio de vectors de millores, de la linia de produccio i de cada estacio
    ce = VI(M);
    ne = VI(M);
    linia = VVI(K, VI(M + 2, -1));
    estacions = VVB(M, VB(C, false));

    // inicialitzacio d'estructures
    for (int i = 0; i < M; i++) {
        // capacitat de l'estacio
        f >> ce[i];
    }
    for (int i = 0; i < M; i++) {
        // conjunt de cotxes consecutius de l'estacio
        f >> ne[i];
    }
    for (int i = 0; i < K; i++) {
        // identificador i nombre de cotxes de la classe
        f >> linia[i][0] >> linia[i][1];
        for (int j = 2; j < M + 2; j++) {
            // millores requerides per la classe
            f >> linia[i][j];
        }
    }

    f.close();
}
