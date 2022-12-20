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

// estructura amb informació sobre cada classe
struct Klass
{
    int id, millores, prod;
};

int MAX_VAL = 1000000;

void sortida(string output, int inici, const int &pen_act, const VI &solucio)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << pen_act << ' ' << temps << endl;
    for (auto x : solucio)
        out << x << " ";
    out << endl;
    out.close();
}

VI setinterval(int a, int b, int m, const VI &solparcial, const VI &ne)
{
    // funcio que copia un interval de la solparcial al vector interval
    // parametre m es la millora tractada
    int x = a, y = b;
    if (a < 0)
    {
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

int i_classe_anterior(int sol, const vector<Klass> &m_klass)
{
    int i = 0;
    while (m_klass[i].id != sol)
    {
        i++;
    }
    return i;
}

int classe_escollida(const vector<Klass> &m_klass, const int &sol)
{
    int K = m_klass.size();
    // trobem si hi ha valors de produccio igual i si no hi ha un valor
    int max_prod = 0, escollida = 0, classe = 0;
    for (int i = 0; i < K; i++)
    {
        if (m_klass[i].prod != 0)
        {
            if (m_klass[i].prod > max_prod)
            {
                max_prod = m_klass[i].prod;
                classe = m_klass[i].id;
                escollida = classe;
            }
            else if (m_klass[i].prod == max_prod)
            {
                escollida = classe;
                if (m_klass[i_classe_anterior(sol, m_klass)].millores >= m_klass[classe].millores)
                {
                    if (m_klass[i].millores < m_klass[classe].millores)
                    {
                        escollida = m_klass[i].id;
                    }
                }
                else
                {
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

int penalitzacions(int cotxes, const VI &solucio, const VVB &estacions,
                   const VI &ne, const VI &ce)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    int M = ne.size();
    int C = solucio.size();

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

void genera_solucio(int &pen_act, vector<Klass> &m_klass, VI &solucio,
                    const VVB &estacions, const VI &ne, const VI &ce)
{
    int C = solucio.size();
    // comment
    for (int i = 0; i < C; i++)
    {
        if (i == 0)
        {
            int identificador;
            for (int j = 0; j < m_klass.size() - 1; j++)
            {
                if (m_klass[j].prod > m_klass[j + 1].prod)
                {
                    identificador = m_klass[j].id;
                }
                else if (m_klass[j].prod < m_klass[j + 1].prod)
                {
                    identificador = m_klass[j + 1].id;
                }
                else
                {
                    identificador = m_klass[0].id;
                }
            }

            solucio[i] = identificador;
            m_klass[i_classe_anterior(identificador, m_klass)].prod--;
        }
        else
        {
            solucio[i] = classe_escollida(m_klass, solucio[i - 1]);
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        }
        pen_act += penalitzacions(i + 1, solucio, estacions, ne, ce);
    }
}

bool SortMillores(const Klass &a, const Klass &b)
{
    return a.millores > b.millores;
}

int f(const VI &produccio)
{
    int k = 0;
    int P = produccio.size();
    for (int i = 1; i < P; i++)
    {
        if (produccio[i] > produccio[k])
        {
            k = i;
        }
    }
    return k;
}

double f_i(int f, double lambda, VI &penalitzacio, VI &solparcial, const VVB &estacions)
{
    // Funcio que calcula la nova funció objectiu
    int sum = 0;
    int M = penalitzacio.size();
    for (int i = 0; i < M; i++)
    {
        // solparcial[i] = una class 0,1,...,K
        sum += penalitzacio[i] * estacions[solparcial[i]][i];
    }
    // no se quin regularization factor (lambda) definir
    return f + lambda * sum;
}

/*VI generar_solucio(VI produccio)
{
    VI solinicial;
    for (int i = 0; i < produccio.size(); i++)
    {
        while (produccio[i] > 0)
        {
            solinicial.push_back(i);
            produccio[i]--;
        }
    }
    return solinicial;
}*/

VI localSearch(VI solparcial, int &pen, const int &cotxes, const VVB &estacions, const VI &ne, const VI &ce)
{
    VI neighbourhood = solparcial;
    int C = solparcial.size();
    int pen_n = 0;
    for (int i = 0; i < C; i++)
    {
        swap(neighbourhood[cotxes], neighbourhood[i]);

        for (int j = 0; j < C; j++)
        {
            pen_n += penalitzacions(j + 1, neighbourhood, estacions, ne, ce);
        }
        if (pen_n < pen)
        {
            solparcial = neighbourhood;
            pen = pen_n;
        }
    }
    return solparcial;
}

void guided_local_search(int cotxes, VI &solparcial, VI &solucio, vector<Klass> m_klass,
                         int pen_act, int &pen_max, const VVB &estacions, const VI &ne, const VI &ce, const string output, const int inici)
{
    genera_solucio(pen_act, m_klass, solparcial, estacions, ne, ce);
    int C = solparcial.size();
    for (int j = 0; j < C; j++)
    {
        pen_act += penalitzacions(j + 1, solparcial, estacions, ne, ce);
    }
    // solparcial és la solucio actual i solucio és la millor fins al moment sobre f
    // M = numero de propietats diferents entre solucions
    VI penalitzacio(C, 0);
    double lambda = 0.2;
    pen_max = f_i(pen_act, lambda, penalitzacio, solparcial, estacions);
    while (cotxes < C)
    {
        // Millor solució fins al moment sobre la funció objectiu original
        VI s_f = localSearch(solparcial, pen_max, cotxes, estacions, ne, ce);
        // Busquem solparcial amb local search per optimitzar f_i
        solparcial = localSearch(solparcial, pen_act, cotxes, estacions, ne, ce);
        if (pen_max > pen_act)
        {
            pen_max = pen_act;
            solucio = solparcial;
        }
        else
        {
            solucio = s_f;
        }
        // Actualitzar vector de penalitzacions
        penalitzacio[cotxes] = penalitzacions(cotxes + 1, solparcial, estacions, ne, ce);
        pen_act += penalitzacio[cotxes];
        sortida(output, inici, pen_max, solucio);
        ++cotxes;
    }
}

int main(int argc, char **argv)
{
    // llegim input de fitxers
    int inici = clock();
    string input = string(argv[1]);
    string output = string(argv[2]);
    ifstream f(input);
    // maybe s'ha de llegir aixi
    // ifstream inputFile(argv[1],ifstream::in);
    // ifstream solFile(argv[2],ifstream::in);

    int C, M, K;
    f >> C >> M >> K;
    // creacio vectors de millores, matriu booleana de cada estacio i vector de
    // l'estructura Klass amb les millores de cada classe
    VI ce = VI(M);
    VI ne = VI(M);
    VVB estacions = VVB(K, VB(M, false));
    vector<Klass> m_klass(K);

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
        f >> classe >> m_klass[classe].prod;
        m_klass[classe].id = classe;
        for (int j = 0; j < M; j++)
        {
            int aplica_millora;
            // millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora)
            {
                estacions[classe][j] = true;
                m_klass[classe].millores++;
            }
            else
                estacions[classe][j] = false;
        }
    }
    // definim la solucio parcial que utlitzara la funcio de backtracking
    VI solparcial(C, 0), solucio(C, 0);
    sort(m_klass.begin(), m_klass.end(), SortMillores);
    int cotxes = 0, pen_act = 0, pen_max = MAX_VAL;
    guided_local_search(cotxes, solparcial, solucio, m_klass, pen_act, pen_max,
                        estacions, ne, ce, output, inici);
    f.close();
}
