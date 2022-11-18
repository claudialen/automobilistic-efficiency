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

// variables globals de les dades inicials del problema
VI ce, ne, produccio, solucio;
VVB estacions;
int pen_max, k, C, M, K;

void sortida(string output, int inici, int pen_max)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << pen_max << ' ' << temps << endl;
    for (auto x : solucio)
        out << solucio[x] << " ";
    out << endl;
    out.close();
}

VI setinterval(const int& a, const int& b, const VI& solparcial, const int& m)
{
    // funcio que copia un interval de la solparcial al vector interval
    VI interval;
    int i = a, j = 0;
    while (i < b and j < ne[m]) {
        interval[j] = solparcial[i];
        ++i, ++j;
    }
    return interval;
}

int penalitzacions(const VI& solparcial, const int& cotxes)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    // prenem interval per comptar penalitzacions de la solucio solparcial
    VI interval;
    // per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++) {
        int cotxes_millora = 0;
        // si el nombre de cotxes construits equival al total a construir
        if (cotxes == C) {
            // afegim les penalitzacions de l'interval ne incomplet al final
            for (int i = ne[m]; i > -1; i--) {
                interval = setinterval(cotxes - i, cotxes, solparcial, m);

                for (int k = 0; k < interval.size() - 1; k++) {
                    if (estacions[interval[k]][m]) {
                        cotxes_millora++;
                    }
                }
            }

        } else {
            // afegim les penalitzacions de l'interval ne incomplet a l'inici
            if (cotxes - ne[m] <= 0) {
                interval = solparcial;
            } else {
                interval = setinterval(cotxes - ne[m], cotxes, solparcial, m);
            }
            for (int k = 0; k < interval.size() - 1; k++) {
                if (estacions[interval[k]][m]) {
                    cotxes_millora++;
                }
            }
        }
        // si el nombre de cotxes consecutius és major que el màxim permès
        if (cotxes_millora > ce[m]) {
            pen += max(cotxes_millora - ce[m], 0);
        }
    }
    return pen;
}

void backtrack(int& cotxes, VI& solparcial, int& pen_act, const int& inici, const string& output)
{
    // si la penalització de la solució actual és major que la màxima no seguim
    if (pen_act >= pen_max)
        return;
    // si el nombre de cotxes construits equival al total a construir
    if (cotxes == solparcial.size()) {
        pen_max = penalitzacions(solparcial, cotxes);
        solucio = solparcial;
        sortida(output, inici, pen_max);
    }
    // per a cada estacio afegir si la classe k te aquella millora
    else {
        // k és el punter que recorre totes les classes
        for (int k = 0; k < K; k++) {
            // si encara queden cotxes a construir d'aquella classe k
            if (produccio[k] > 0) {
                produccio[k]--;

                solparcial[cotxes] = k;
                pen_act = penalitzacions(solparcial, cotxes);
                backtrack(++cotxes, solparcial, pen_act, inici, output);

                produccio[k]++;
            } else if (pen_act >= pen_max)
                backtrack(cotxes, solparcial, pen_act, inici, output);
        }
    }
}

int main(int argc, char** argv)
{
    // llegim input de fitxers
    int inici = clock();
    string input = string(argv[1]);
    string output = string(argv[2]);
    ifstream f(input);
    // ifstream inputFile(argv[1],ifstream::in);
    // ifstream solFile(argv[2],ifstream::in);

    f >> C >> M >> K;
    // creacio vectors de millores, de la linia de produccio i de cada estacio
    ce = VI(M);
    ne = VI(M);
    produccio = VI(K);
    estacions = VVB(M, VB(C, false));

    // inicialitzacio d'estructures
    for (int i = 0; i < M; i++) {
        // capacitat de l'estacio
        f >> ce[i];
    }
    for (int i = 0; i < M; i++) {
        // conjunt de cotxes consecutius maxim de cada estacio
        f >> ne[i];
    }
    for (int i = 0; i < K; i++) {
        // identificador i nombre de cotxes de cada classe k
        int classe;
        f >> classe >> produccio[classe];
        for (int j = 2; j < M + 2; j++) {
            int aplica_millora;
            // millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora)
                estacions[i][j] = true;
            else
                estacions[i][j] = false;
        }
    }
    // definim la solucio parcial que utlitzara la funcio de backtracking
    VI solparcial(C);
    // inicialitzem el nombre de cotxes construits i de penalitzacions a 0
    backtrack(0, solparcial, 0, inici, output);

    f.close();
}
