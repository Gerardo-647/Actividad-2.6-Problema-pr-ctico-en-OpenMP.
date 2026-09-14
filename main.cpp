/* Integrantes:
 * - Cancelada de la O Gerardo Alexander
 * - Hernandez Miranda David Alejandro
 */

#include <iostream>
#include <cstring>
#include <cmath>
#include <cctype>
#include <omp.h>

using namespace std;

const char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int ALPHABET_SIZE = 36;

class BruteForceSolver {
private:
    char* target_key;
    int key_length;
    unsigned long long total_combinations;

    bool found_flag;
    int winner_thread;
    char* found_key;

    void indexToKey(unsigned long long index, char* out_str) const {
        for (int i = key_length - 1; i >= 0; i--) {
            out_str[i] = ALPHABET[index % ALPHABET_SIZE];
            index /= ALPHABET_SIZE;
        }
        out_str[key_length] = '\0';
    }

public:
    BruteForceSolver(const char* key) {
        key_length = strlen(key);
        target_key = new char[key_length + 1];
        strcpy(target_key, key);

        total_combinations = 1;
        for (int i = 0; i < key_length; i++) {
            total_combinations *= ALPHABET_SIZE;
        }

        found_flag = false;
        winner_thread = -1;
        found_key = new char[key_length + 1];
        found_key[0] = '\0';
    }

    ~BruteForceSolver() {
        delete[] target_key;
        delete[] found_key;
    }

    static bool isValidKey(const char* key) {
        int len = strlen(key);
        if (len == 0) return false;
        for (int i = 0; i < len; i++) {
            char c = toupper(key[i]);
            bool valid = false;
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                if (c == ALPHABET[j]) {
                    valid = true;
                    break;
                }
            }
            if (!valid) return false;
        }
        return true;
    }

    unsigned long long getTotalCombinations() const {
        return total_combinations;
    }

    void solveSequential(double &out_time, unsigned long long &out_checked) {
        char* current_candidate = new char[key_length + 1];
        out_checked = 0;

        double start_time = omp_get_wtime();

        for (unsigned long long i = 0; i < total_combinations; i++) {
            indexToKey(i, current_candidate);
            out_checked++;

            if (strcmp(current_candidate, target_key) == 0) {
                break;
            }
        }

        double end_time = omp_get_wtime();
        out_time = end_time - start_time;
        delete[] current_candidate;
    }

    void solveParallel(int num_threads, double &out_time, unsigned long long* thread_checked) {
        found_flag = false;
        winner_thread = -1;
        found_key[0] = '\0';

        unsigned long long* start_range = new unsigned long long[num_threads];
        unsigned long long* end_range = new unsigned long long[num_threads];
        unsigned long long* count_assigned = new unsigned long long[num_threads];
        bool* thread_found = new bool[num_threads];

        unsigned long long chunk_size = total_combinations / num_threads;
        unsigned long long remainder = total_combinations % num_threads;

        unsigned long long current_start = 0;
        for (int i = 0; i < num_threads; i++) {
            start_range[i] = current_start;
            unsigned long long assigned = chunk_size + (i < remainder ? 1 : 0);
            count_assigned[i] = assigned;
            end_range[i] = current_start + assigned - 1;
            current_start += assigned;
            thread_checked[i] = 0;
            thread_found[i] = false;
        }

        cout << "\n=======================================================\n";
        cout << " DISTRIBUCION DE TRABAJO ENTRE HILOS\n";
        cout << "=======================================================\n";
        char* temp_start = new char[key_length + 1];
        char* temp_end = new char[key_length + 1];

        for (int i = 0; i < num_threads; i++) {
            indexToKey(start_range[i], temp_start);
            indexToKey(end_range[i], temp_end);
            cout << "Hilo " << i
                 << " -> Inicio: " << temp_start
                 << " -> Fin: " << temp_end
                 << " -> Cantidad: " << count_assigned[i] << endl;
        }
        delete[] temp_start;
        delete[] temp_end;
        cout << "-------------------------------------------------------\n";

        double start_time = omp_get_wtime();

        #pragma omp parallel num_threads(num_threads) shared(found_flag, winner_thread)
        {
            int tid = omp_get_thread_num();
            char* local_candidate = new char[key_length + 1];
            unsigned long long local_count = 0;

            #pragma omp critical
            {
                cout << "[Inicio] Hilo " << tid << " comenzando busqueda en rango ["
                     << start_range[tid] << " - " << end_range[tid] << "]\n";
            }

            for (unsigned long long i = start_range[tid]; i <= end_range[tid]; i++) {
                #pragma omp flush(found_flag)
                if (found_flag) {
                    break;
                }

                indexToKey(i, local_candidate);
                local_count++;

                if (strcmp(local_candidate, target_key) == 0) {
                    #pragma omp critical
                    {
                        if (!found_flag) {
                            found_flag = true;
                            winner_thread = tid;
                            strcpy(found_key, local_candidate);
                            thread_found[tid] = true;
                            #pragma omp flush(found_flag)
                        }
                    }
                    break;
                }
            }

            thread_checked[tid] = local_count;

            #pragma omp critical
            {
                cout << "[Finalizacion] Hilo " << tid << " termino. Reviso "
                     << local_count << " combinaciones.\n";
            }

            delete[] local_candidate;
        }

        double end_time = omp_get_wtime();
        out_time = end_time - start_time;

        cout << "\n-------------------------------------------------------\n";
        cout << " ESTADO FINAL DE LOS HILOS\n";
        cout << "-------------------------------------------------------\n";
        for (int i = 0; i < num_threads; i++) {
            cout << "Hilo " << i << ": "
                 << (thread_found[i] ? "ENCONTRADA (Ganador)" : (found_flag ? "DETENIDO" : "NO ENCONTRADA"))
                 << " | Evaluados: " << thread_checked[i] << endl;
        }

        if (winner_thread != -1) {
            cout << "\n>>> CLAVE ENCONTRADA POR EL HILO: " << winner_thread << " <<<\n";
            cout << ">>> CLAVE OBTENIDA: " << found_key << " <<<\n";
        }

        delete[] start_range;
        delete[] end_range;
        delete[] count_assigned;
        delete[] thread_found;
    }

    int getWinnerThread() const { return winner_thread; }
};

int main() {
    char input_key[64];
    int num_threads = omp_get_max_threads();
    std::cout << "Integrantes: \n - Cancelada de la O Gerardo Alexander \n - Hernandez Miranda David Alejandro \n\n";

    cout << "=============================================\n";
    cout << "  BUSQUEDA EXHAUSTIVA EN PARALELO (OMP)\n";
    cout << "=============================================\n";
    cout << " Hilos de hardware disponibles: " << num_threads << endl;

    while (true) {
        cout << "\nIngrese la clave de prueba (caracteres permitidos: A-Z, 0-9): ";
        cin >> input_key;

        for (int i = 0; input_key[i]; i++) {
            input_key[i] = toupper(input_key[i]);
        }

        if (BruteForceSolver::isValidKey(input_key)) {
            break;
        } else {
            cout << "[ERROR] Clave invalida. Asegurese de no incluir espacios, caracteres especiales ni estar vacia.\n";
        }
    }

    BruteForceSolver solver(input_key);

    cout << "\n[OK] Clave valida: " << input_key << endl;
    cout << "[INFO] Longitud de clave: " << strlen(input_key) << endl;
    cout << "[INFO] Tamano del espacio de busqueda (36^N): " << solver.getTotalCombinations() << " combinaciones.\n";

    cout << "\n>>> Ejecutando busqueda secuencial... <<<\n";
    double seq_time = 0.0;
    unsigned long long seq_checked = 0;
    solver.solveSequential(seq_time, seq_checked);
    cout << " [Secuencial] Tiempo: " << seq_time << " s | Combinaciones revisadas: " << seq_checked << endl;

    cout << "\n>>> Ejecutando busqueda paralela con OpenMP (" << num_threads << " hilos)... <<<\n";
    double par_time = 0.0;
    unsigned long long* thread_checked = new unsigned long long[num_threads];
    solver.solveParallel(num_threads, par_time, thread_checked);

    cout << "\n=======================================================\n";
    cout << " COMPARATIVA DE RENDIMIENTO\n";
    cout << "=======================================================\n";
    cout << " Tiempo secuencial: " << seq_time << " s\n";
    cout << " Tiempo paralelo:   " << par_time << " s\n";
    cout << " Hilos utilizados:  " << num_threads << "\n";

    if (par_time > 0.0) {
        double speedup = seq_time / par_time;
        cout << " Speedup obtenido:  " << speedup << "x\n";
    }
    cout << "=======================================================\n";

    delete[] thread_checked;
    std::cout << "\n\nIntegrantes: \n - Cancelada de la O Gerardo Alexander \n - Hernandez Miranda David Alejandro \n";
    return 0;
}
