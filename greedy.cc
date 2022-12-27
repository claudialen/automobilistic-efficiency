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
struct Klass {
    int id, millores, prod;
};

int MAX_VAL = 1000000;

/*
Funció que escriu la solució final sobre el fitxer sortida.
Paràmetres: nom del fitxer sortda (output), algoritme d'inici (inici),
penalitzacio final (pen_max) i la solució final (solucio).
*/
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

/*
Funció que crea intervals de mida ne.
Paràmetres: inici i final de l'interval (a,b), la millora tractada (m), solució
parcial i vector de nombre màxim de millores ne.
*/
VI setinterval(int a, int b, int m, const VI& solparcial, const VI& ne)
{
    if (a < 0) {
        // Si la posició inicial és menor que 0, redefinim x=0
        a = 0;
    }
    VI interval;
    while (a < b) {
        interval.push_back(solparcial[a]);
        ++a;
    }
    return interval;
}

/* Funció que calcula el nombre de penalitzacions. */
int penalitzacions(int cotxes, const VI& solparcial, const VVB& estacions,
    const VI& ne, const VI& ce)
{
    // Nombre de penalitzacions per afegir un nou cotxe a la solparcial
    int pen = 0, M = ne.size(), C = solparcial.size();

    // Vector de classes a comptar penalitzacions de la solucio solparcial
    VI interval;
    // Per cada millora m recorrem totes les seves classes k
    for (int m = 0; m < M; m++) {
        int cotxes_millora = 0;
        if (cotxes == C) {
            for (int i = cotxes - ne[m]; i < cotxes; i++) {
                cotxes_millora = 0;
                // Mirem si l'interval ne té penalitzacions
                interval = setinterval(i, cotxes, m, solparcial, ne);
                for (int k = 0; k < int(interval.size()); k++) {
                    if (estacions[interval[k]][m]) {
                        cotxes_millora++;
                    }
                }

                // Nombre de cotxes consecutius és major que el màxim permès
                if (cotxes_millora > ce[m]) {
                    pen += cotxes_millora - ce[m];
                }
            }
        } else {
            interval = setinterval(cotxes - ne[m], cotxes, m, solparcial, ne);
            for (int k = 0; k < int(interval.size()); k++) {
                if (estacions[interval[k]][m]) {
                    cotxes_millora++;
                }
            }
            if (cotxes_millora > ce[m]) {
                pen += cotxes_millora - ce[m];
            }
        }
    }
    return pen;
}

/* Funció que escull la classe m_klass segons els criteris del greedy. */
int classe_escollida(const vector<Klass>& m_klass, const int& sol)
{
    int K = m_klass.size(), max_prod = 0, escollida = 0, classe = 0;
    for (int i = 0; i < K; i++) {
        //per cada classe mirem si encara queden cotxes per produir
        if (m_klass[i].prod > 0) {
            if (m_klass[i].prod > max_prod) {
                //si són més del maxim trobat fins el moment es canvien els valors de max_prod i escollida
                max_prod = m_klass[i].prod;
                escollida = classe = m_klass[i].id;
            } else if (m_klass[i].prod == max_prod) {
                //si són els mateixos mirem si la classe del cotxe anterior requeria més millores
                if (m_klass[i_classe_anterior(sol, m_klass)].millores >= m_klass[classe].millores) {
                    //mirem si la classe que volem colocar té més millores que la i
                    if (m_klass[i].millores < m_klass[classe].millores) {
                        escollida = m_klass[i].id;
                    }
                } else {
                    //si no requeria més i té menys millores que la i actualitzem escollida
                    if (m_klass[i].millores > m_klass[classe].millores) {
                        escollida = m_klass[i].id;
                    }
                }
            }
        }
    }
    return escollida;
}

/* Funció utilizada per identificar la posició de la classe m_klass sobre el
vector de solució. */
int i_classe_anterior(int sol, const vector<Klass>& m_klass)
{
    int i = 0; //contador
    while (m_klass[i].id != sol) {
        i++;
    }
    return i;
}

/*Funció principal de l'algoritme greedy*/
void greedy(int& pen_act, vector<Klass>& m_klass, VI& solucio,
    const VVB& estacions, const VI& ne, const VI& ce)
{
    int C = solucio.size();
    for (int i = 0; i < C; i++) {
        if (i == 0) {
            // Si estem colocant el primer cotxe ens guiem per el cotxe amb més demanda
            for (int j = 0; j < m_klass.size() - 1; j++) {
                if (m_klass[j].prod > m_klass[j + 1].prod) {
                    solucio[i] = m_klass[j].id;
                } else {
                    solucio[i] = m_klass[j + 1].id;
                }
            }
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        } else {
            // Sino utilitzem les condicions definides per la funció classe_escollida() per trobar la classe
            solucio[i] = classe_escollida(m_klass, solucio[i - 1]);
            m_klass[i_classe_anterior(solucio[i], m_klass)].prod--;
        }
        // Actualitzem la penalització de la solució
        pen_act += penalitzacions(i + 1, solucio, estacions, ne, ce);
    }
}

/* Funció utilitzada per ordenar les classes segons el nombre de millores. */
bool SortMillores(const Klass& a, const Klass& b)
{
    return a.millores < b.millores;
}

int main(int argc, char** argv)
{
    // Es llegeixen input de fitxers
    int inici = clock();
    string input = string(argv[1]);
    string output = string(argv[2]);
    ifstream f(input);

    int C, M, K;
    f >> C >> M >> K;
    // Creacio vectors de millores, matriu booleana de cada estacio i vector de
    // l'estructura Klass amb les millores de cada classe
    VI ce = VI(M);
    VI ne = VI(M);
    VVB estacions = VVB(K, VB(M, false));
    vector<Klass> m_klass(K);

    // Inicialitzacio d'estructures
    for (int i = 0; i < M; i++) {
        // Capacitat de l'estacio
        f >> ce[i];
    }
    for (int i = 0; i < M; i++) {
        // Conjunt de cotxes consecutius maxim de cada estacio
        f >> ne[i];
    }
    for (int i = 0; i < K; i++) {
        // Identificador i nombre de cotxes de cada classe k
        int classe;
        f >> classe >> m_klass[classe].prod;
        m_klass[classe].id = classe;
        for (int j = 0; j < M; j++) {
            int aplica_millora;
            // Millores requerides per la classe k
            f >> aplica_millora;
            if (aplica_millora) {
                estacions[classe][j] = true;
                m_klass[classe].millores++;
            } else
                estacions[classe][j] = false;
        }
    }

    // Es defineix la solucio final i penalització actual
    VI solucio(C, 0);
    int pen_act = 0;

    // S'ordena el vector de millores en ordre descendent per nombre d'elles
    sort(m_klass.begin(), m_klass.end(), SortMillores);

    greedy(pen_act, m_klass, solucio, estacions, ne, ce);
    // Es retorna una sola solució
    sortida(output, inici, pen_act, solucio);
    f.close();
}
