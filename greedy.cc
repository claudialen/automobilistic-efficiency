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
struct Klass {
    int id, millores, prod;
};

int MAX_VAL = 1000000;

void sortida(string output, int inici, const int& pen_act, const VI& solucio)
{
    ofstream out(output);
    double temps = (clock() - inici) / (double)CLOCKS_PER_SEC;
    out << pen_act << ' ' << temps << endl;
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
    VI interval;
    int i = x;
    while (i < b) {
        interval.push_back(solparcial[i]);
        ++i;
    }
    return interval;
}

int i_classe_anterior(int sol, const vector<Klass>& m_klass)
{
    int i = 0;
    while (m_klass[i].id != sol) {
        i++;
    }
    return i;
}

int classe_escollida(const vector<Klass>& m_klass, const int& sol)
{
    int K = m_klass.size();
    // trobem si hi ha valors de produccio igual i si no hi ha un valor
    int max_prod = 0, escollida = 0, classe = 0;
    for (int i = 0; i < K; i++) {
        if (m_klass[i].prod != 0) {
            if (m_klass[i].prod > max_prod) {
                max_prod = m_klass[i].prod;
                classe = m_klass[i].id;
                escollida = classe;
            } else if (m_klass[i].prod == max_prod) {
                escollida = classe;
                if (m_klass[i_classe_anterior(sol, m_klass)].millores >= m_klass[classe].millores) {
                    if (m_klass[i].millores < m_klass[classe].millores) {
                        escollida = m_klass[i].id;
                    }
                } else {
                    if (m_klass[i].millores > m_klass[classe].millores) {
                        escollida = m_klass[i].id;
                    }
                }
            }
        }
    }
    return escollida;
}

int penalitzacions(int cotxes, const VI& solucio, const VVB& estacions,
    const VI& ne, const VI& ce)
{
    // nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0;
    int M = ne.size();
    int C = solucio.size();

    // vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;

    // per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++) {
        int cotxes_millora = 0;
        // afegim les penalitzacions de l'interval ne incomplet a l'inici
        interval = setinterval(cotxes - ne[m], cotxes, m, solucio, ne);
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

void greedy(int& pen_act, vector<Klass>& m_klass, VI& solucio,
    const VVB& estacions, const VI& ne, const VI& ce)
{
    int C = solucio.size();
    // comment
    for (int i = 0; i < C; i++) {
        if (i == 0) {
            int identificador;
            for (int j = 0; j < m_klass.size() - 1; j++) {
                if (m_klass[j].prod > m_klass[j + 1].prod) {
                    identificador = m_klass[j].id;
                } else if (m_klass[j].prod < m_klass[j + 1].prod) {
                    identificador = m_klass[j + 1].id;
                } else {
                    identificador = m_klass[0].id;
                }
            }

            solucio[i] = identificador;
            m_klass[i_classe_anterior(identificador, m_klass)].prod--;

        } else {
            solucio[i] = classe_escollida(m_klass, solucio[i - 1]);
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        }
        pen_act += penalitzacions(i + 1, solucio, estacions, ne, ce);
    }
}

bool SortMillores(const Klass& a, const Klass& b)
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

    int C, M, K;
    f >> C >> M >> K;
    // creacio vectors de millores, matriu booleana de cada estacio i vector de
    // l'estructura Klass amb les millores de cada classe
    VI ce = VI(M);
    VI ne = VI(M);
    VVB estacions = VVB(K, VB(M, false));
    vector<Klass> m_klass(K);

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
        f >> classe >> m_klass[classe].prod;
        m_klass[classe].id = classe;
        for (int j = 0; j < M; j++) {
            int aplica_millora;
            // millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora) {
                estacions[classe][j] = true;
                m_klass[classe].millores++;
            } else
                estacions[classe][j] = false;
        }
    }
    // definim la solucio parcial que utlitzara la funcio de backtracking
    VI solucio(C, 0);
    int pen_act = 0;
    sort(m_klass.begin(), m_klass.end(), SortMillores);
    // inicialitzem el nombre de cotxes construits i de penalitzacions a 0
    greedy(pen_act, m_klass, solucio, estacions, ne, ce);
    sortida(output, inici, pen_act, solucio);
    f.close();
}
