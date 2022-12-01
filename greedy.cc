#include <algorithm>
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

struct class {
    int id, millores;
};

// variables globals de les dades inicials del problema
VI ce, ne, produccio;
VVB estacions;
int pen_max = 1000000, k, C, M, K;

void sortida(string output, int inici, int pen_max, const VI& solucio)
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
        ++i;
        ++j;
    }
    return interval;
}

int classe_escollida(VI millores_classe, int sol)
{
    // trobem si hi ha valors de produccio igual i si no hi ha un valor
    int max_prod = 0, escollida = 0, classe = 0;
    for (int i = 0; i < K; i++) {
        if (produccio[millores_classe[i].id] > max_prod) {
            max_prod = produccio[millores_classe[i].id];
            classe = millores_classe[i].id;
            escollida = classe;
        } else if (produccio[millores_classe[i].id] == max_prod) {
            escollida = classe;
            if (millores_classe[sol].millores >= millores_classe[classe].millores) {
                if (millores_classe[i].millores < millores_classe[classe].millores) {
                    escollida = millores_classe[i].id;
                }
            } else {
                if (millores_classe[i].millores > millores_classe[classe].millores) {
                    escollida = millores_classe[i].id;
                }
            }
        }
    }
    return escollida;
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

                for (int k = 0; k < int(interval.size()) - 1; k++) {
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
            for (int k = 0; k < int(interval.size()) - 1; k++) {
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

void greedy(VI millores_classe, VI& solucio, const int& inici, const string& output)
{
    for (int i = 0; i < C; i++) {
        cout << "i: " << i << endl;
        if (i == 0) {
            solucio[i] = millores_classe[0].id;
        }
        solucio[i] = classe_escollida(millores_classe, solucio[i - 1]);
    }
    sortida(output, inici, penalitzacions(solucio, C), solucio);
}

bool SortMillores(const class& a, const class& b)
{
    return a.millores > b.millores;
}

int main(int argc, char** argv)
{
    // llegim input de fitxers
    int inici = clock();
    string input = string(argv[1]);
    string output = string(argv[2]);
    ifstream f(input);
    // maybe s'ha de llegir aixi
    // ifstream inputFile(argv[1],ifstream::in);
    // ifstream solFile(argv[2],ifstream::in);

    f >> C >> M >> K;
    // creacio vectors de millores, de la linia de produccio i de cada estacio
    ce = VI(M);
    ne = VI(M);
    produccio = VI(K);
    estacions = VVB(K, VB(M, false));
    vector<class> millores_classe(K);

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
        millores_classe[i].id = classe;
        for (int j = 0; j < M; j++) {
            int aplica_millora;
            // millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora) {
                estacions[classe][j] = true;
                millores_classe[classe].millores++;

            } else
                estacions[classe][j] = false;
        }
    }
    // definim la solucio parcial que utlitzara la funcio de backtracking
    VI solucio(C, 0);
    sort(millores_classe.begin(), millores_classe.end(), SortMillores);
    // inicialitzem el nombre de cotxes construits i de penalitzacions a 0
    greedy(millores_classe, solucio, inici, output);
    f.close();
}
