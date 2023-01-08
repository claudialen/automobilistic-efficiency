#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

ofstream out;

/*
Per a l'algortime de metaheurística hem decidit utilitzar el Guided Local
Search perquè la seva estructura era molt semblant a la del propi problema
plantejat, fet que ens permetia adaptar-lo facilment ja que només requeria
redefinir els parametres de la metaheurística pels propis del problema que ja
haviem definit pels anteriors algoritmes.
*/

using VI = vector<int>;
using VVI = vector<VI>;
using VB = vector<bool>;
using VVB = vector<VB>;

// estructura amb informació sobre cada classe
struct Klass
{
    int id, millores, prod;
};

/*
Funció que escriu la solució final sobre el fitxer sortida.
Paràmetres: nom del fitxer sortda (output), algoritme d'inici (inici),
penalitzacio final (pen_max) i la solució final (solucio).
*/
void sortida(const string output, const int &pen_max, const int inici, const VI solucio)
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
VI setinterval(int a, const int b, const VI &solparcial)
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
int penalitzacions(const int cotxes, const VI &solucio_actual, const VI ne, const VI ce, const VVB estacions)
{
    // Nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    // Vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;
    // Per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < ne.size(); m++)
    {
        int cotxes_millora = 0;
        if (cotxes == solucio_actual.size())
        {
            for (int i = cotxes - ne[m]; i < cotxes; i++)
            {
                cotxes_millora = 0;
                // Mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, solucio_actual);
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
            interval = setinterval(cotxes - ne[m], cotxes, solucio_actual);
            for (int k = 0; k < int(interval.size()); k++)
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
Funció que fa servir cerca local per calcula una solució.
*/
VI localSearch(VI &solparcial, int &pen, const int &cotxes, const VI ne, const VI ce, const VVB estacions)
{
    int pen_n = 0;
    for (int i = 0; i < solparcial.size(); i++)
    {
        if (solparcial[cotxes] != solparcial[i])
        {
            VI neighbourhood = solparcial;
            // Quan la posició de la solució en la que ens trobem i la posició 'i'
            // són diferents les intercanviem per crear una solució veïna
            swap(neighbourhood[cotxes], neighbourhood[i]);
            // Calculem la penalització de la nova solució
            for (int j = 0; j < solparcial.size(); j++)
            {
                pen_n += penalitzacions(j + 1, neighbourhood, ne, ce, estacions);
            }
            if (pen_n < pen)
            {
                // Si la nova solució és millor definim nova_sol i pen
                solparcial = neighbourhood;
                pen = pen_n;
            }
        }
    }
    return solparcial;
}

/*
Funció que calcula la nova funció objectiu.
*/
double f_i(const int f, const double lambda, const VI &penalitzacio, const VI &solparcial, const VVB estacions)
{
    int sum = 0;
    for (int i = 0; i < estacions.size(); i++)
    {
        sum += penalitzacio[i] * estacions[solparcial[i]][i];
    }
    return f + lambda * sum;
}

// ·················· Solució Greedy ··················//

/*
Funció utilizada per identificar la posició de la classe m_klass sobre el
vector de solució.
*/
int i_classe_anterior(int sol, const vector<Klass> m_klass)
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
int classe_escollida(const int &sol, vector<Klass> &m_klass)
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

/*
Funció principal de l'algoritme greedy
*/
void genera_solucio(int &pen_act, VI &solucio_1, vector<Klass> &m_klass, const VI ne, const VI ce, const VVB estacions)
{
    for (int i = 0; i < solucio_1.size(); i++)
    {
        if (i == 0)
        {
            // Si estem col·locant el primer cotxe prenem el cotxe amb més demanda
            for (int j = 0; j < m_klass.size() - 1; j++)
            {
                if (m_klass[j].prod > m_klass[j + 1].prod)
                {
                    solucio_1[i] = m_klass[j].id;
                }
                else
                {
                    solucio_1[i] = m_klass[j + 1].id;
                }
            }
        }
        else
        {
            // Sino utilitzem les condicions definides per la funció classe_escollida() per trobar la classe
            solucio_1[i] = classe_escollida(solucio_1[i - 1], m_klass);
        }
        // Actualitzem la penalització de la solució
        m_klass[i_classe_anterior(solucio_1[i], m_klass)].prod--;
        pen_act += penalitzacions(i + 1, solucio_1, ne, ce, estacions);
    }
}

/* Funció utilitzada per ordenar les classes segons el nombre de millores. */
bool SortMillores(const Klass &a, const Klass &b)
{
    return a.millores < b.millores;
}

/*
Funció que calcula solució òptima utilitzant Guided Local Search.
Paràmetres: nombre de cotxes afegits a la solució (cotxes), solució actual
(solparcial), penalitzacions actual (pen_act).
*/
void guided_local_search(const string output, const int inici, int cotxes, VI &solparcial, int pen_act, vector<Klass> m_klass, const VVB estacions, const VI ne, const VI ce)
{
    // generem la solució inicial
    genera_solucio(pen_act, solparcial, m_klass, ne, ce, estacions);
    // Calculem la penalització de la solució inicial
    for (int j = 0; j < solparcial.size(); j++)
    {
        pen_act += penalitzacions(j + 1, solparcial, ce, ne, estacions);
    }
    VI penalitzacio(solparcial.size(), 0), solucio, s_f;
    // Hem trobat que aquesta lambda era la que ajudava més a trobar la solució
    double lambda = 10;
    int pen_max = f_i(pen_act, lambda, penalitzacio, solparcial, estacions);
    while (cotxes < solparcial.size())
    {
        // Millor solució fins al moment sobre la funció objectiu original
        s_f = localSearch(solparcial, pen_max, cotxes, ne, ce, estacions);
        // Busquem solparcial amb local search per optimitzar f_i
        solparcial = localSearch(solparcial, pen_act, cotxes, ne, ce, estacions);
        if (pen_max > pen_act)
        {
            // Si la penalització de la solució s_f és major redefinim solucio i pen_max
            pen_max = pen_act;
            solucio = solparcial;
        }
        else
        {
            solucio = s_f;
        }
        // Actualitzem el vector de penalitzacions
        penalitzacio[cotxes] = penalitzacions(cotxes + 1, solparcial, ce, ne, estacions);
        pen_act += penalitzacio[cotxes];
        sortida(output, pen_max, inici, solucio);
        ++cotxes;
    }
}

void llegir_dades(const string input, int &C, int &M, int &K, VI &ce, VI &ne, VVB &estacions, vector<Klass> &m_klass)
{
    ifstream f(input);
    f >> C >> M >> K;
    // Creacio vectors de millores, matriu booleana de cada estacio i vector de
    // l'estructura Klass amb les millores de cada classe
    ce.resize(M), ne.resize(M);
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
    VI ce, ne;
    vector<Klass> m_klass;
    VVB estacions;
    llegir_dades(input, C, M, K, ce, ne, estacions, m_klass);
    // Es defineix la solucio final i parcial
    VI solparcial(C, 0);
    // S'ordena el vector de millores en ordre descendent per nombre d'elles
    sort(m_klass.begin(), m_klass.end(), SortMillores);
    guided_local_search(output, inici, 0, solparcial, 0, m_klass, estacions, ne, ce);
}
