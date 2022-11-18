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

VI ce, ne, produccio, solucio;
VVB estacions;
int penalitzacio_max;
int classe;

int penalitzacions(const VI& solparcial, const int& cotxes_constr)
{
    int p = 0;
    for (int m = 0; m < estacions[0].size(); m++) {
        int cotxes_millora = 0;
        if (cotxes_constr == solparcial.size()) {
            for (int interval = ne[m]; interval > -1; interval--) {
                VI finestra(interval) = solparcial [cotxes_constr - interval:cotxes_constr];
                for (int classe = 0; classe < finestra.size() - 1; classe++) {
                    if (estacions[finestra[classe]][m]) {
                        cotxes_millora++;
                    }
                }
            }

        } else {
            if (cotxes_constr - ne[m] <= 0) {
                VI finestra(cotxes_constr) = solparcial;
            } else {
                VI finestra(ne[m]) = solparcial [cotxes_constr - ne[m]:cotxes_constr];
            }
            for (int classe = 0; classe < finestra.size() - 1; classe++) {
                if (estacions[finestra[classe]][m]) {
                    cotxes_millora++;
                }
            }
        }

        if (millora > ce[m]) {
            p += max(millora - ce[m], 0);
        }
    }
    return p;
}

void sortida(string output, int inici, int penalitzacio_max)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << penalitzacio_max << ' ' << temps << endl;
    out.close();
}

void backtrack(int& cotxes_constr, vector<int>& produccio,
    vector<int>& solparcial, vector<int>& millores,
    int& penalitzacio_act)
{
    if (penalitzacio_act >= penalitzacio_max)
        return;
    if (cotxes_constr == solparcial.size()) {
        penalitzacio_max = penalitzacions(solparcial, cotxes_constr);
        solucio = solparcial;
        sortida(output, inici, penalitzacio_max);
    }
    // per a cada estacio afegir si la classe te aquella millora
    else {
        int M = ce.size();
        // classe es el punter que recorre totes les classes
        for (int classe = 0; classe < produccio.size(); classe++) {
            if (produccio[classe] > 0) {
                produccio[classe]--;
                solparcial[cotxes_constr] = classe;
                penalitzacio_act = penalitzacions(solparcial, cotxes_constr);
                backtrack(++cotxes_constr, produccio, solparcial, millores, penalitzacio_max);
                produccio[classe]++;
            } else if (penalitzacio_act >= penalitzacio_max)
                backtrack(cotxes_constr, produccio, solparcial, millores, penalitzacio_max);
        }
    }
}

int main(int argc, char** argv)
{
    int inici = clock();
    string input = string(argv[1]);
    string output = string(arv[2]);
    ifstream f(input);

    int C, M, K;
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
        // identificador i nombre de cotxes de cada classe
        f >> classe >> produccio[classe];
        for (int j = 2; j < M + 2; j++) {
            int aplica_millora;
            // millores requerides per la classe
            f >> aplica_millora;
            if (aplica_millora)
                estacions[i][j] = true;
            else
                estacions[i][j] = false;
        }
    }

    f.close();
}
