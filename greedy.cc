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

// Estructura amb informació sobre cada classe
struct Klass
{
    int id, millores, prod;
};

/*
Funció que escriu la solució final sobre el fitxer sortida.
Paràmetres: nom del fitxer sortda (output), algoritme d'inici (inici),
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
VI setinterval(int a, const int b, const VI &solucio)
{
    if (a < 0)
    {
        // En cas que el valor entrat sigui menor a 0 (intervals incomplets)
        a = 0;
    }
    VI interval;
    while (a < b)
    {
        interval.push_back(solucio[a]);
        ++a;
    }
    return interval;
}

/*
Funció que calcula el nombre de penalitzacions.
*/
int penalitzacions(const int &cotxes, const VVB &estacions, const VI &solucio, const VI &ce, const VI &ne)
{
    // Nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    // Vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;
    // Per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < ce.size(); m++)
    {
        int cotxes_millora = 0;
        if (cotxes == solucio.size())
        {
            for (int i = cotxes - ne[m]; i < cotxes; i++)
            {
                cotxes_millora = 0;
                // Mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, solucio);
                for (int k = 0; k < int(interval.size()); k++)
                {
                    if (estacions[interval[k]][m])
                    {
                        cotxes_millora++;
                    }
                }

                // Nombre de cotxes consecutius és major que el màxim permès
                if (cotxes_millora > ce[m])
                {
                    pen += cotxes_millora - ce[m];
                }
            }
        }
        else
        {
            interval = setinterval(cotxes - ne[m], cotxes, solucio);
            for (int k = 0; k < interval.size(); k++)
            {
                if (estacions[interval[k]][m])
                {
                    cotxes_millora++;
                }
            }
            if (cotxes_millora > ce[m])
            {
                pen += cotxes_millora - ce[m];
            }
        }
    }
    return pen;
}

/*
Funció utilizada per identificar la posició de la classe m_klass sobre el
vector de solució.
*/
int i_classe_anterior(const int sol, const vector<Klass> m_klass)
{
    int i = 0;
    while (m_klass[i].id != sol)
    {
        i++;
    }
    return i;
}

/*
Funció que escull la classe m_klass segons els criteris del greedy.
*/
int classe_escollida(const int &sol, const vector<Klass> m_klass)
{
    int max_prod = 0, escollida = 0;
    for (int i = 0; i < m_klass.size(); i++)
    {
        // per cada classe mirem si encara queden cotxes per produir
        if (m_klass[i].prod > 0)
        {
            if (m_klass[i].prod > max_prod)
            {
                // es canvien els valors de max_prod i escollida
                max_prod = m_klass[i].prod;
                escollida = m_klass[i].id;
            }
            else if (m_klass[i].prod == max_prod)
            {
                // si són els mateixos mirem les millores de la classe anterior
                if (m_klass[i_classe_anterior(sol, m_klass)].millores >= m_klass[i].millores && m_klass[i].millores < m_klass[escollida].millores)
                {
                    escollida = m_klass[i].id;
                }
            }
        }
    }
    return escollida;
}

/*Funció principal de l'algoritme greedy*/
void greedy(VI &solucio, int &pen_max, vector<Klass> m_klass, const VVB &estacions, const VI ce, const VI ne)
{
    for (int i = 0; i < solucio.size(); i++)
    {
        if (i == 0)
        {
            // Si estem col·locant el primer cotxe prenem per el cotxe amb més demanda
            for (int j = 0; j < m_klass.size() - 1; j++)
            {
                if (m_klass[j].prod > m_klass[j + 1].prod)
                {
                    solucio[i] = m_klass[j].id;
                }
                else
                {
                    solucio[i] = m_klass[j + 1].id;
                }
            }
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        }
        else
        {
            // Sino utilitzem les condicions definides per la funció classe_escollida() per trobar la classe
            solucio[i] = classe_escollida(solucio[i - 1], m_klass);
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        }
        // Actualitzem la penalització de la solució
        pen_max += penalitzacions(i + 1, estacions, solucio, ce, ne);
    }
}

/* Funció utilitzada per ordenar les classes segons el nombre de millores. */
bool SortMillores(const Klass &a, const Klass &b)
{
    return a.millores < b.millores;
}

void llegir_dades(string &input, int &C, int &M, int &K, VI &ce, VI &ne, VVB &estacions, vector<Klass> &m_klass)
{
    ifstream f(input);
    f >> C >> M >> K;
    // Creacio vectors de millores, matriu booleana de cada estacio i vector de
    // l'estructura Klass amb les millores de cada classe
    ce.resize(M);
    ne.resize(M);
    estacions.resize(K, VB(M, false));
    m_klass.resize(K);

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
        f >> classe >> m_klass[classe].prod;
        m_klass[classe].id = classe;
        for (int j = 0; j < M; j++)
        {
            int aplica_millora;
            // Millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora)
            {
                estacions[classe][j] = true;
                m_klass[classe].millores++;
            }
        }
    }
    f.close();
}

int main(int argc, char **argv)
{
    // Es llegeixen input de fitxers
    int inici = clock(), C, M, K;
    string input = string(argv[1]), output = string(argv[2]);
    VI ce, ne, solucio;
    VVB estacions;
    vector<Klass> m_klass;
    llegir_dades(input, C, M, K, ce, ne, estacions, m_klass);
    // Es defineix la solucio final i penalització actual
    solucio.resize(C, 0);

    // S'ordena el vector de millores en ordre descendent per nombre d'elles
    sort(m_klass.begin(), m_klass.end(), SortMillores);

    int pen_max = 0;
    greedy(solucio, pen_max, m_klass, estacions, ce, ne);
    // Es retorna una sola solució
    sortida(output, inici, solucio, pen_max);
}
