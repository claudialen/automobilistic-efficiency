#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

ofstream out;

using VI = vector<int>;
using VVI = vector<VI>;
using VB = vector<bool>;
using VVB = vector<VB>;

int MAX_VAL = 1000000;

void sortida(string output, int inici, int pen_max, const VI &solucio)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << pen_max << ' ' << temps << endl;
    for (auto x : solucio)
        out << x << " ";
    out << endl;
    out.close();
}

int penalitzacions(int cotxes, const VI &solparcial, const VVB &estacions,
                   const VI &ne, const VI &ce)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    int M = ne.size();
    int C = solparcial.size();

    // vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;

    // per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++)
    {
        int cotxes_millora = 0;
        // mirem si l'interval ne té penalitzacions
        interval = setinterval(cotxes - ne[m], cotxes, m, solparcial, ne);
        for (int k = 0; k < int(interval.size()); k++)
        {
            if (estacions[interval[k]][m])
            {
                cotxes_millora++;
            }
        }

        // si el nombre de cotxes consecutius és major que el màxim permès
        if (cotxes_millora > ce[m])
        {
            pen += max(cotxes_millora - ce[m], 0);
        }
    }
    return pen;
}

bool features(int i, VI solparcial)
{
    return;
}

int f_i(int f, int phi, VI penalitzacio, VI solparcial)
{
    int sum = 0;
    for (int i = 0; i < penalitzacio.size(); i++)
    {
        sum += penalitzacio[i] * features(i, solparcial);
    }
    return f + phi * sum;
}

void guided_local_search(int cotxes, int C, VI solparcial, VI solucio)
{
    solucio = solparcial;
    f = ; // ns si hauriem d definir aqui f o utilitzar nomes la inicial
    // m = numero de propietats diferents entre solucions
    VI penalitzacio(m, 0);
    while (cotxes < C)
    {
        s_f = localSearch(solparcial, f_i(f, phi, penalitzacio, features), f);
        if (f(s_f) < f(solucio))
        {
            solucio = s_f;
        }
    }
}

int main(int argc, char **argv)
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
    for (int i = 0; i < M; i++)
    {
        // capacitat de l'estacio
        f >> ce[i];
    }
    for (int i = 0; i < M; i++)
    {
        // conjunt de cotxes consecutius maxim de cada estacio
        f >> ne[i];
    }
    for (int i = 0; i < K; i++)
    {
        // identificador i nombre de cotxes de cada classe k
        int classe;
        f >> classe >> produccio[classe];
        for (int j = 0; j < M; j++)
        {
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
    guided_local_search(cotxes, C, solparcial, solucio, pen_act, pen_max, produccio, ne, ce,
                        estacions);
    sortida(output, inici, pen_max, solucio);
    f.close();
}
