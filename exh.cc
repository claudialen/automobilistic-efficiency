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
int penalitzacio_act;

void sortida(string output, int inici, ...)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << penalitzacio << ' ' << temps << endl;
    out.close();
}

void backtrack(int& cotxes_constr, vector<int>& produccio, vector<int>& solparcial, vector<int>& millores, int& penalitzacio_max)
{
    if (penalitzacio_act > penalitzacio_max)
        return;
    if (cotxes_constr == solparcial.size()) {
        penalitzacio_max = penalitzacio_act;
        sortida();
    } // sortida()
    // per a cada estacio afegir si la classe te aquella millora
    else {
        int M = ce.size();
        for (int m = 0; m < M; ++m) {
            // aixo esta malament, el vector no va be
            if (estacions[klasse][m])
                ++millores[m];
            if (millores[m] >= ne[m]) {
                // aquest cotxes_constr hauria de ser cotxes consecutius
                penalitzacio_act += max(cotxes_constr - ce[m], 0);
            }
        }
        // classe es el punter que recorre totes les classes
        for (int classe = 0; classe < produccio.size(); classe++) {
            if (penalitzacio_act > penalitzacio_max)
                backtrack(cotxes_constr, produccio, solparcial, millores, penalitzacio_max);
            else if (produccio[classe] > 0) {
                produccio[classe]--;
                solparcial[cotxes_constr] = classe;
                backtrack(++cotxes_constr, produccio, solparcial, millores, penalitzacio_max);
                produccio[classe]++;
            }
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
    // creacio vectors de millores, de la linia de produccio i de cada estacio
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
        f >> classe >> produccio[classe];
        for (int j = 2; j < M + 2; j++) {
            // millores requerides per la classe
            f >> estacions[i][j];
        }
    }

    f.close();
}
