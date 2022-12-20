#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

ofstream out;

using VI = vector<int>;
using VVI = vector<VI>;
using VB = vector<bool>;
using VVB = vector<VB>;

int MAX_VAL = 1000000;

/*Funtion that writes the final solution on the output file.
Required variables: file name (output), algorithm start (inici), final penalty (pen_max) and final solution (solucio).*/
void sortida(string output, int inici, int pen_max, const VI &solucio)
{
    ofstream out(output);                                      // creates the output file
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC; // calculates the time the algorithm spent finding the last solution
    out << pen_max << ' ' << temps << endl;                    // writes the penalty
    for (auto x : solucio)
        out << x << " "; // writes the solution on the file
    out << endl;
    out.close(); // closes the file
}

/* Function that creates intervals of ne size */
VI setinterval(int a, int b, int m, const VI &solparcial, const VI &ne)
{
    int x = a;
    if (a < 0)
    {
        // if the initial position is smaller than 0, we redefine it as 0
        x = 0;
    }
    VI interval;
    int i = x;
    while (i < b)
    {
        interval.push_back(solparcial[i]);
        ++i;
    }
    return interval;
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
        if (cotxes == C)
        {
            for (int i = cotxes - ne[m]; i < cotxes; i++)
            {
                int cotxes_millora = 0;
                // mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, m, solparcial, ne);
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
                    pen += cotxes_millora - ce[m];
                }
            }
        }
        else
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
                pen += cotxes_millora - ce[m];
            }
        }
    }
    return pen;
}

void backtrack(int cotxes, VI &solparcial, VI &solucio, int pen_act, int &pen_max,
               VI &produccio, const VI &ne, const VI &ce, const VVB &estacions, const string output, const int inici)
{
    int K = produccio.size();
    int C = solparcial.size();
    // si la penalització de la solució actual és major que la màxima no seguim
    if (pen_act >= pen_max)
    {
        return;
    }
    // si el nombre de cotxes construits equival al total a construir
    if (cotxes == C)
    {
        // pen_act += penalitzacions(cotxes, solparcial, estacions, ne, ce);
        if (pen_act < pen_max)
        {
            pen_max = pen_act;
            solucio = solparcial;
            sortida(output, inici, pen_max, solucio);
        }
    }
    // per a cada estacio afegir si la classe k te aquella millora
    else
    {
        // k és el punter que recorre totes les classes
        for (int k = 0; k < K; k++)
        {
            // si encara queden cotxes a construir d'aquella classe k
            if (produccio[k] > 0)
            {
                solparcial[cotxes] = k;
                pen_act += penalitzacions(cotxes + 1, solparcial, estacions, ne, ce);
                --produccio[k];
                backtrack(cotxes + 1, solparcial, solucio, pen_act, pen_max,
                          produccio, ne, ce, estacions, output, inici);
                ++produccio[k];
                pen_act -= penalitzacions(cotxes + 1, solparcial, estacions, ne, ce);
            }
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
    VI solparcial(C);
    VI solucio(C);

    // inicialitzem el nombre de cotxes construits i de penalitzacions a 0
    int cotxes = 0, pen_act = 0, pen_max = MAX_VAL;
    backtrack(cotxes, solparcial, solucio, pen_act, pen_max, produccio, ne, ce,
              estacions, output, inici);
    f.close();
}
