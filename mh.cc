#include <algorithm>
#include <cmath>
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

int MAX_VAL = 1000000;

struct Klass {
    int id, millores, prod;
};

void sortida(string output, int inici, int pen_max, const VI& solucio)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << pen_max << ' ' << temps << endl;
    for (auto x : solucio)
        out << x << " ";
    out << endl;
    out.close();
}

VI setinterval(int a, int b, int m, const VI& solparcial, const VI& ne)
{
    // funcio que copia un interval de la solparcial al vector interval
    // parametre m es la millora tractada
    int x = a, y = b;
    if (a < 0) {
        x = 0;
    }
    if (b == 0) {
        y = 1;
    }
    VI interval;
    int i = x, j = 0;
    while (i < b and j < ne[m]) {
        interval.push_back(solparcial[i]);
        ++i;
        ++j;
    }
    return interval;
}

int penalitzacions(int cotxes, const VI& solparcial, const VVB& estacions,
    const VI& ne, const VI& ce)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    int M = ne.size();
    int C = solparcial.size();

    // vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;

    // per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++) {
        int cotxes_millora = 0;
        // mirem si l'interval ne té penalitzacions
        interval = setinterval(cotxes - ne[m], cotxes, m, solparcial, ne);
        for (int k = 0; k < int(interval.size()); k++) {
            if (estacions[interval[k]][m]) {
                cotxes_millora++;
            }
        }

        // si el nombre de cotxes consecutius és major que el màxim permès
        if (cotxes_millora > ce[m]) {
            pen += max(cotxes_millora - ce[m], 0);
        }
    }
    return pen;
}

int f(const VI& produccio)
{
    int k = 0;
    int P = produccio.size();
    for (int i = 1; i < P; i++) {
        if (produccio[i] > produccio[k]) {
            k = i;
        }
    }
    return k;
}

int f_i(int f, int lambda, VI& penalitzacio, VI& solparcial, const VVB& estacions)
{
    // Funcio que calcula la nova funció objectiu
    int sum = 0;
    int M = penalitzacio.size();
    for (int i = 0; i < M; i++) {
        // solparcial[i] = una class 0,1,...,K
        sum += penalitzacio[i] * estacions[solparcial[i]][i];
    }
    // no se quin regularization factor (lambda) definir
    return f + lambda * sum;
}

VI generar_solucio(VI produccio)
{
    VI solinicial;
    for (int i = 0; i < produccio.size(); i++) {
        while (produccio[i] > 0) {
            solinicial.push_back(i);
            produccio[i]--;
        }
    }
    return solinicial;
}

VI localSearch(VI solparcial, int& pen, const int& cotxes, const VVB& estacions, const VI& ne, const VI& ce)
{
    VI neighbourhood = solparcial;
    int C = solparcial.size();
    int pen_n = 0;
    for (int i = 0; i < C; i++) {
        swap(neighbourhood[cotxes], neighbourhood[i]);

        for (int j = 0; j < C; j++) {
            pen_n += penalitzacions(j, neighbourhood, estacions, ne, ce);
        }
        if (pen_n < pen) {
            solparcial = neighbourhood;
            pen = pen_n;
        }
    }
    return solparcial;
}

void guided_local_search(int cotxes, VI& solparcial, VI& solucio, VI& produccio,
    int pen_act, int& pen_max, const VVB& estacions, const VI& ne, const VI& ce)
{
    solparcial = generar_solucio(produccio);
    int C = solparcial.size();
    for (int j = 0; j < C; j++) {
        pen_act += penalitzacions(j, solparcial, estacions, ne, ce);
    }
    // solparcial és la solucio actual i solucio és la millor fins al moment sobre f
    // M = numero de propietats diferents entre solucions
    VI penalitzacio(C, 0);
    int lambda = 0;
    pen_max = f_i(pen_act, lambda, penalitzacio, solparcial, estacions);
    while (cotxes < C) {
        // Millor solució fins al moment sobre la funció objectiu original
        VI s_f = localSearch(solparcial, pen_max, cotxes, estacions, ne, ce);
        // Busquem solparcial amb local search per optimitzar f_i
        solparcial = localSearch(solparcial, pen_act, cotxes, estacions, ne, ce);
        if (pen_max > pen_act) {
            pen_max = pen_act;
            solucio = solparcial;
        } else {
            solucio = s_f;
        }
        // Actualitzar vector de penalitzacions
        penalitzacio[cotxes] = penalitzacions(cotxes, solparcial, estacions, ne, ce);
        pen_act += penalitzacio[cotxes];
        ++cotxes;
    }
}

int main(int argc, char** argv)
{
    // llegim input de fitxers
    int inici = clock();
    string input = string(argv[1]);
    string output = string(argv[2]);
    ifstream f(input);

    int C, M, K;
    f >> C >> M >> K;
    // creacio vectors de millores, de la linia de produccio i de cada estacio
    VI ce = VI(M);
    VI ne = VI(M);
    VI produccio = VI(K);
    VVB estacions = VVB(K, VB(M, false));

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
        for (int j = 0; j < M; j++) {
            int aplica_millora;
            // millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora)
                estacions[i][j] = true;
            else
                estacions[i][j] = false;
        }
    }
    // definim solucio parcial i final que utlitzara la funcio de backtracking
    VI solparcial(C, 0);
    VI solucio(C);

    // inicialitzem el nombre de cotxes construits i de penalitzacions a 0
    int cotxes = 0, pen_act = 0, pen_max = MAX_VAL;
    guided_local_search(cotxes, solparcial, solucio, produccio, pen_act, pen_max,
        estacions, ne, ce);
    sortida(output, inici, pen_max, solucio);
    f.close();
}
