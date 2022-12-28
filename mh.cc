#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

ofstream out;

/* Per a l'algortime de metaheurística hem decidit utilitzar el Guided Local
Search perquè la seva estructura era molt semblant a la del propi problema plantejat, el que ens permetia adaptar-lo facilment ja
que només requeria redefinir els parametres de la metaheurística pels propis del problema que ja haviem definit pels anteriors algoritmes.
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

int MAX_VAL = 1000000;
int C, M, K, pen_max = MAX_VAL, inici;
VI ce, ne, solucio;
VVB estacions;
vector<Klass> m_klass;
string input, output;

/*
Funció que escriu la solució final sobre el fitxer sortida.
Paràmetres: nom del fitxer sortda (output), algoritme d'inici (inici),
penalitzacio final (pen_max) i la solució final (solucio).
*/
void sortida()
{
    ofstream out(output);
    out << pen_max << ' ' << (clock() - inici) / (double)CLOCKS_PER_SEC << endl;
    for (auto x : solucio)
        out << x << " ";
    out << endl;
    out.close();
}

/*
Funció que defineix un interval de la solució donada de llargada ne
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
int penalitzacions(int cotxes, const VI &solucio_actual)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    // vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;
    // per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++)
    {
        int cotxes_millora = 0;
        if (cotxes == C)
        {
            for (int i = cotxes - ne[m]; i < cotxes; i++)
            {
                cotxes_millora = 0;
                // mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, solucio_actual);
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
            // mirem si l'interval ne té penalitzacions
            interval = setinterval(cotxes - ne[m], cotxes, solucio_actual);
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

/*
Funció que fa servir cerca local per calcula una solució.
*/
VI localSearch(VI solparcial, int &pen, const int &cotxes)
{
    int pen_n = 0;
    for (int i = 0; i < C; i++)
    {
        if (solparcial[cotxes] != solparcial[i])
        {
            VI neighbourhood = solparcial;
            // Quan la posició de la solució en la que ens trobem i la posició i són diferents les intercanviem per crear una solució veïna
            swap(neighbourhood[cotxes], neighbourhood[i]);
            // Calculem la penalització de la nova solució
            for (int j = 0; j < C; j++)
            {
                pen_n += penalitzacions(j + 1, neighbourhood);
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
double f_i(int f, double lambda, VI &penalitzacio, VI &solparcial)
{
    int sum = 0;
    for (int i = 0; i < M; i++)
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
int i_classe_anterior(int sol)
{
    int i = 0;
    while (m_klass[i].id != sol)
    {
        i++;
    }
    return i;
}

/* Funció que escull la classe m_klass segons els criteris del greedy. */
int classe_escollida(const int &sol)
{
    int max_prod = 0, escollida = 0, classe = 0;
    for (int i = 0; i < K; i++)
    {
        // per cada classe mirem si encara queden cotxes per produir
        if (m_klass[i].prod > 0)
        {
            if (m_klass[i].prod > max_prod)
            {
                // si són més del maxim trobat fins el moment es canvien els valors de max_prod i escollida
                max_prod = m_klass[i].prod;
                escollida = classe = m_klass[i].id;
            }
            else if (m_klass[i].prod == max_prod)
            {
                // si són els mateixos mirem si la classe del cotxe anterior requeria més millores
                if (m_klass[i_classe_anterior(sol)].millores >= m_klass[classe].millores)
                {
                    // mirem si la classe que volem colocar té més millores que la i
                    if (m_klass[i].millores < m_klass[classe].millores)
                    {
                        escollida = m_klass[i].id;
                    }
                }
                else
                {
                    // si no requeria més i té menys millores que la i actualitzem escollida
                    if (m_klass[i].millores > m_klass[classe].millores)
                    {
                        escollida = m_klass[i].id;
                    }
                }
            }
        }
    }
    return escollida;
}

/*
Funció principal de l'algoritme greedy
*/
void genera_solucio(int &pen_act, VI &solucio_1)
{
    for (int i = 0; i < C; i++)
    {
        if (i == 0)
        {
            // Si estem colocant el primer cotxe ens guiem per el cotxe amb més demanda
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
            solucio_1[i] = classe_escollida(solucio_1[i - 1]);
        }
        // Actualitzem la penalització de la solució
        m_klass[i_classe_anterior(solucio_1[i])].prod--;
        pen_act += penalitzacions(i + 1, solucio_1);
    }
}
// ···················································//

/* Funció utilitzada per ordenar les classes segons el nombre de millores. */
bool SortMillores(const Klass &a, const Klass &b)
{
    return a.millores < b.millores;
}

/*
Funció que calcula solució òptima utilitzant Guided Local Search.
Paràmetres: nombre de cotxes afegits a la solució (cotxes), solució actual
(solparcial), millor solució fins al moment sobre f (solucio), vector de
classes (m_klass), penalitzacions actual i millor (pen_act, pen_max), vectors
d'estacions, ne, ce i variables de sortida i inici.
*/
void guided_local_search(int cotxes, VI &solparcial, int pen_act)
{
    // generem la solució inicial
    genera_solucio(pen_act, solparcial);
    // Calculem la penalització de la solució inicial
    for (int j = 0; j < C; j++)
    {
        pen_act += penalitzacions(j + 1, solparcial);
    }
    VI penalitzacio(C, 0);
    double lambda = 0.1; // Hem trobat que aquesta lambda era la que ajudava més a trobar la solució
    pen_max = f_i(pen_act, lambda, penalitzacio, solparcial);
    while (cotxes < C)
    {
        // Millor solució fins al moment sobre la funció objectiu original
        VI s_f = localSearch(solparcial, pen_max, cotxes);
        // Busquem solparcial amb local search per optimitzar f_i
        solparcial = localSearch(solparcial, pen_act, cotxes);
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
        penalitzacio[cotxes] = penalitzacions(cotxes + 1, solparcial);
        pen_act += penalitzacio[cotxes];
        sortida();
        ++cotxes;
    }
}

void llegir_dades()
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
    inici = clock();
    input = string(argv[1]), output = string(argv[2]);
    llegir_dades();
    // Es defineix la solucio final i parcial
    VI solparcial(C, 0);
    solucio.resize(C, 0);
    // S'ordena el vector de millores en ordre descendent per nombre d'elles
    sort(m_klass.begin(), m_klass.end(), SortMillores);
    guided_local_search(0, solparcial, 0);
}
