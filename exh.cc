#include <algorithm>
#include <ctime>
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

/*
Funció que escriu la solució final sobre el fitxer sortida.
Variables requerides: nom del fitxer sortda (output), algoritme d'inici (inici),
penalitzacio final (pen_max) i la solució final (solucio).
*/
void sortida(const string &output, const int &inici, const VI &solucio, const int &pen_max)
{
    ofstream out(output);
    out << pen_max << ' ' << (clock() - inici) / (double)CLOCKS_PER_SEC << endl;
    for (auto x : solucio)
        out << x << " ";
    out << endl;
    out.close();
}

/*
Funció que crea intervals de mida ne.
Paràmetres: inici i final de l'interval (a,b), la millora tractada (m), solució
parcial i vector de nombre màxim de millores ne.
*/
VI setinterval(int a, int b, const VI &solparcial)
{
    if (a < 0)
    {
        // En cas que el valor entrat sigui menor a 0 (intervals incomplets)
        a = 0;
    }
    VI interval;
    while (a < b)
    {
        interval.push_back(solparcial[a]);
        ++a;
    }
    return interval;
}

/*
Funció que calcula el nombre de penalitzacions.
*/
int penalitzacions(int cotxes, const VI &solparcial, const VI ne, const VI ce, const VVB estacions)
{
    // Nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    // Vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;
    // Per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < ne.size(); m++)
    {
        int cotxes_millora = 0;
        if (cotxes == solparcial.size())
        {
            for (int i = cotxes - ne[m]; i < cotxes; i++)
            {
                cotxes_millora = 0;
                // Mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, solparcial);
                for (int k = 0; k < int(interval.size()); k++)
                {
                    if (estacions[interval[k]][m])
                        cotxes_millora++;
                }

                // Nombre de cotxes consecutius és major que el màxim permès
                if (cotxes_millora > ce[m])
                    pen += cotxes_millora - ce[m];
            }
        }
        else
        {
            interval = setinterval(cotxes - ne[m], cotxes, solparcial);
            for (int k = 0; k < interval.size(); k++)
            {
                if (estacions[interval[k]][m])
                    cotxes_millora++;
            }
            if (cotxes_millora > ce[m])
                pen += cotxes_millora - ce[m];
        }
    }
    return pen;
}

void backtrack(const string output, const int inici, int cotxes, VI &solparcial, int pen_act, int &pen_max, VI produccio, const VI ce, const VI ne, const VVB estacions)
{
    // Si la penalització de la solució actual és major que la màxima no seguim
    if (pen_act >= pen_max)
        return;

    // Si el nombre de cotxes construits equival al total a construir
    if (cotxes == solparcial.size())
    {
        if (pen_act < pen_max)
        {
            pen_max = pen_act;
            sortida(output, inici, solparcial, pen_max);
        }
    }
    // Per a cada estacio afegir si la classe k te aquella millora
    else
    {
        // k és el punter que recorre totes les classes
        for (int k = 0; k < produccio.size(); k++)
        {
            // Si encara queden cotxes a construir d'aquella classe k
            if (produccio[k] > 0)
            {
                solparcial[cotxes] = k;
                pen_act += penalitzacions(cotxes + 1, solparcial, ne, ce, estacions);
                --produccio[k];
                backtrack(output, inici, cotxes + 1, solparcial, pen_act, pen_max, produccio, ce, ne, estacions);
                ++produccio[k];
                pen_act -= penalitzacions(cotxes + 1, solparcial, ne, ce, estacions);
            }
        }
    }
}

void llegir_dades(const string input, int &C, int &M, int &K, VI &ce, VI &ne, VI &produccio, VVB &estacions)
{
    ifstream f(input);
    f >> C >> M >> K;
    // Creacio vectors de millores, de la linia de produccio i de cada estacio
    ce.resize(M);
    ne.resize(M);
    produccio.resize(K);
    estacions.resize(K, VB(M, false));

    // Inicialitzacio d'estructures
    for (int i = 0; i < M; i++)
    {
        // Capacitat de l'estacio
        f >> ce[i];
    }
    for (int i = 0; i < M; i++)
    {
        // Conjunt de cotxes consecutius maxim de cada estacio
        f >> ne[i];
    }
    for (int i = 0; i < K; i++)
    {
        // Identificador i nombre de cotxes de cada classe k
        int classe;
        f >> classe >> produccio[classe];
        for (int j = 0; j < M; j++)
        {
            int aplica_millora;
            // Millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora)
                estacions[i][j] = true;
        }
    }
    f.close();
}

int main(int argc, char **argv)
{
    // Es llegeixen input de fitxers
    int inici = clock(), C, M, K;
    string input = string(argv[1]), output = string(argv[2]);
    VI ce, ne, produccio;
    VVB estacions;
    llegir_dades(input, C, M, K, ce, ne, produccio, estacions);
    // Es defineixen solucio parcial i final que utlitzara la funcio de backtracking
    VI solparcial(C);
    // S'inicialitzen el nombre de cotxes construits i de penalitzacions a 0
    int pen_max = INT_MAX;
    backtrack(output, inici, 0, solparcial, 0, pen_max, produccio, ce, ne, estacions);
}
