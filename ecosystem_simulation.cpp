#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

enum CellType { EMPTY, ROCK, RABBIT, FOX };

struct Animal {
    int x, y;  // x=columna, y=fila
    int proc_age;
    int hunger;
    CellType type;
    bool alive;
    Animal(int x_, int y_, CellType t) : x(x_), y(y_), proc_age(0), hunger(0), type(t), alive(true) {}
};

int GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES;
int N_GEN, R, C, N;

vector<vector<CellType>> grid;
vector<Animal> rabbits, foxes;
vector<pair<int,int>> rocks;

// Direcciones clockwise: Norte, Este, Sur, Oeste
int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};

bool isValid(int x, int y) {
    return x >= 0 && x < C && y >= 0 && y < R;
}

vector<pair<int,int>> getAvailableEmpty(int x, int y) {
    vector<pair<int,int>> result;
    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        if (isValid(nx, ny) && grid[ny][nx] == EMPTY) {
            result.push_back({nx, ny});
        }
    }
    return result;
}

vector<pair<int,int>> getAdjacentRabbits(int x, int y) {
    vector<pair<int,int>> result;
    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        if (isValid(nx, ny) && grid[ny][nx] == RABBIT) {
            result.push_back({nx, ny});
        }
    }
    return result;
}

void moveRabbits(int gen) {
    map<pair<int,int>, vector<int>> movements;
    vector<pair<int,int>> offspring_positions;
    
    // Planificar movimientos
    for (size_t i = 0; i < rabbits.size(); i++) {
        if (!rabbits[i].alive) continue;
        
        int x = rabbits[i].x;
        int y = rabbits[i].y;
        
        auto available = getAvailableEmpty(x, y);
        
        if (!available.empty()) {
            int P = available.size();
            int index = (gen + x + y) % P;
            movements[available[index]].push_back(i);
        } else {
            movements[{x, y}].push_back(i);
        }
    }
    
    // Marcar reproducciones
    map<int, bool> will_reproduce;
    for (size_t i = 0; i < rabbits.size(); i++) {
        will_reproduce[i] = false;
    }
    
    for (auto& m : movements) {
        int dest_x = m.first.first;
        int dest_y = m.first.second;
        
        for (int idx : m.second) {
            int old_x = rabbits[idx].x;
            int old_y = rabbits[idx].y;
            bool moved = (dest_x != old_x || dest_y != old_y);
            
            if (moved && rabbits[idx].proc_age >= GEN_PROC_RABBITS) {
                offspring_positions.push_back({old_x, old_y});
                will_reproduce[idx] = true;
            }
        }
    }
    
    // Limpiar grid
    for (auto& r : rabbits) {
        if (r.alive) grid[r.y][r.x] = EMPTY;
    }
    
    // Aplicar movimientos
    for (auto& m : movements) {
        int dest_x = m.first.first;
        int dest_y = m.first.second;
        auto& indices = m.second;
        
        if (indices.size() == 1) {
            int idx = indices[0];
            int old_x = rabbits[idx].x;
            int old_y = rabbits[idx].y;
            bool moved = (dest_x != old_x || dest_y != old_y);
            
            if (will_reproduce[idx]) {
                rabbits[idx].proc_age = 0;
            }
            
            rabbits[idx].x = dest_x;
            rabbits[idx].y = dest_y;
            if (!will_reproduce[idx]) rabbits[idx].proc_age++;
            grid[dest_y][dest_x] = RABBIT;
            
        } else {
            int winner = indices[0];
            for (int idx : indices) {
                if (rabbits[idx].proc_age > rabbits[winner].proc_age) {
                    winner = idx;
                }
            }
            
            for (int idx : indices) {
                if (idx != winner) rabbits[idx].alive = false;
            }
            
            int old_x = rabbits[winner].x;
            int old_y = rabbits[winner].y;
            bool moved = (dest_x != old_x || dest_y != old_y);
            
            if (will_reproduce[winner]) {
                rabbits[winner].proc_age = 0;
            }
            
            rabbits[winner].x = dest_x;
            rabbits[winner].y = dest_y;
            if (!will_reproduce[winner]) rabbits[winner].proc_age++;
            grid[dest_y][dest_x] = RABBIT;
        }
    }
    
    // Crear hijos
    for (auto& pos : offspring_positions) {
        if (grid[pos.second][pos.first] == EMPTY) {
            rabbits.push_back(Animal(pos.first, pos.second, RABBIT));
            grid[pos.second][pos.first] = RABBIT;
        }
    }
}

void moveFoxes(int gen) {
    vector<pair<int,int>> offspring_positions;
    
    // PASO 1: Incrementar hunger
    for (auto& f : foxes) {
        if (f.alive) {
            f.hunger++;
        }
    }
    
    // PASO 2: Verificar quiénes pueden salvarse comiendo rabbits
    map<int, bool> will_eat_rabbit;
    
    for (size_t i = 0; i < foxes.size(); i++) {
        if (!foxes[i].alive) continue;
        
        auto adjacent_rabbits = getAdjacentRabbits(foxes[i].x, foxes[i].y);
        will_eat_rabbit[i] = !adjacent_rabbits.empty();
    }
    
    // PASO 3: Matar foxes hambrientos que no pueden salvarse
    for (size_t i = 0; i < foxes.size(); i++) {
        if (!foxes[i].alive) continue;
        
        if (foxes[i].hunger >= GEN_FOOD_FOXES && !will_eat_rabbit[i]) {
            foxes[i].alive = false;
        }
    }
    
    // PASO 4: Reproducción
    map<int, bool> reproduced;
    for (size_t i = 0; i < foxes.size(); i++) {
        if (!foxes[i].alive) continue;
        
        if (foxes[i].proc_age >= GEN_PROC_FOXES) {
            offspring_positions.push_back({foxes[i].x, foxes[i].y});
            foxes[i].proc_age = 0;
            reproduced[i] = true;
        } else {
            reproduced[i] = false;
        }
    }
    
    // PASO 5: Planificar movimientos
    map<pair<int,int>, vector<int>> movements;
    
    for (size_t i = 0; i < foxes.size(); i++) {
        if (!foxes[i].alive) continue;
        
        int x = foxes[i].x;
        int y = foxes[i].y;
        
        auto adjacent_rabbits = getAdjacentRabbits(x, y);
        
        if (!adjacent_rabbits.empty()) {
            movements[adjacent_rabbits[0]].push_back(i);
        } else {
            auto available = getAvailableEmpty(x, y);
            if (!available.empty()) {
                int P = available.size();
                int index = (gen + x + y) % P;
                movements[available[index]].push_back(i);
            } else {
                movements[{x, y}].push_back(i);
            }
        }
    }
    
    // PASO 6: Limpiar grid
    for (auto& f : foxes) {
        grid[f.y][f.x] = EMPTY;
    }
    
    // PASO 7: Aplicar movimientos
    for (auto& m : movements) {
        int dest_x = m.first.first;
        int dest_y = m.first.second;
        auto& indices = m.second;
        
        // Verificar si hay rabbit en destino
        bool rabbit_at_dest = false;
        for (auto& r : rabbits) {
            if (r.alive && r.x == dest_x && r.y == dest_y) {
                r.alive = false;
                rabbit_at_dest = true;
                break;
            }
        }
        
        if (indices.size() == 1) {
            int idx = indices[0];
            int old_x = foxes[idx].x;
            int old_y = foxes[idx].y;
            bool moved = (dest_x != old_x || dest_y != old_y);
            
            if (rabbit_at_dest) {
                foxes[idx].hunger = 0;
            }
            
            foxes[idx].x = dest_x;
            foxes[idx].y = dest_y;
            if (!reproduced[idx]) foxes[idx].proc_age++;
            grid[dest_y][dest_x] = FOX;
            
        } else {
            int winner = indices[0];
            for (int idx : indices) {
                if (foxes[idx].proc_age > foxes[winner].proc_age ||
                    (foxes[idx].proc_age == foxes[winner].proc_age && foxes[idx].hunger < foxes[winner].hunger)) {
                    winner = idx;
                }
            }
            
            for (int idx : indices) {
                if (idx != winner) foxes[idx].alive = false;
            }
            
            int old_x = foxes[winner].x;
            int old_y = foxes[winner].y;
            bool moved = (dest_x != old_x || dest_y != old_y);
            
            if (rabbit_at_dest) {
                foxes[winner].hunger = 0;
            }
            
            foxes[winner].x = dest_x;
            foxes[winner].y = dest_y;
            if (!reproduced[winner]) foxes[winner].proc_age++;
            grid[dest_y][dest_x] = FOX;
        }
    }
    
    // PASO 8: Crear hijos
    for (auto& pos : offspring_positions) {
        if (grid[pos.second][pos.first] == EMPTY) {
            foxes.push_back(Animal(pos.first, pos.second, FOX));
            grid[pos.second][pos.first] = FOX;
        }
    }
}

void printGeneration(int gen) {
    cerr << "Generation " << gen << endl;
    cerr << "-------   ------- -------" << endl;
    
    for (int y = 0; y < R; y++) {
        // Primera matriz: tipos de objetos
        cerr << "|";
        for (int x = 0; x < C; x++) {
            if (grid[y][x] == ROCK) cerr << '*';
            else if (grid[y][x] == RABBIT) cerr << 'R';
            else if (grid[y][x] == FOX) cerr << 'F';
            else cerr << ' ';
        }
        cerr << "|   ";
        
        // Segunda matriz: proc_age
        cerr << "|";
        for (int x = 0; x < C; x++) {
            bool found = false;
            for (auto& r : rabbits) {
                if (r.alive && r.x == x && r.y == y) {
                    cerr << r.proc_age;
                    found = true;
                    break;
                }
            }
            if (!found) {
                for (auto& f : foxes) {
                    if (f.alive && f.x == x && f.y == y) {
                        cerr << f.proc_age;
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                if (grid[y][x] == ROCK) cerr << '*';
                else cerr << ' ';
            }
        }
        cerr << "| ";
        
        // Tercera matriz: hunger para foxes, mostrar tipo para otros
        cerr << "|";
        for (int x = 0; x < C; x++) {
            bool found = false;
            for (auto& f : foxes) {
                if (f.alive && f.x == x && f.y == y) {
                    cerr << f.hunger;
                    found = true;
                    break;
                }
            }
            if (!found) {
                for (auto& r : rabbits) {
                    if (r.alive && r.x == x && r.y == y) {
                        cerr << 'R';
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                if (grid[y][x] == ROCK) cerr << '*';
                else cerr << ' ';
            }
        }
        cerr << "|" << endl;
    }
    cerr << "-------   ------- -------" << endl;
}

int main() {
    cin >> GEN_PROC_RABBITS >> GEN_PROC_FOXES >> GEN_FOOD_FOXES;
    cin >> N_GEN >> R >> C >> N;
    
    grid.assign(R, vector<CellType>(C, EMPTY));
    
    for (int i = 0; i < N; i++) {
        string type;
        int fila, col;
        cin >> type >> fila >> col;
        
        if (type == "ROCK") {
            grid[fila][col] = ROCK;
            rocks.push_back({col, fila});
        } else if (type == "RABBIT") {
            grid[fila][col] = RABBIT;
            rabbits.push_back(Animal(col, fila, RABBIT));
        } else if (type == "FOX") {
            grid[fila][col] = FOX;
            foxes.push_back(Animal(col, fila, FOX));
        }
    }
    
    printGeneration(0);
    
    for (int gen = 0; gen < N_GEN; gen++) {
        cerr << endl;
        moveRabbits(gen);
        moveFoxes(gen);
        printGeneration(gen + 1);
    }
    
    // Output final
    int count = rocks.size();
    for (auto& r : rabbits) if (r.alive) count++;
    for (auto& f : foxes) if (f.alive) count++;
    
    cout << GEN_PROC_RABBITS << " " << GEN_PROC_FOXES << " " << GEN_FOOD_FOXES << " ";
    cout << "0 " << R << " " << C << " " << count << endl;
    
    for (auto& rock : rocks) cout << "ROCK " << rock.second << " " << rock.first << endl;
    for (auto& r : rabbits) if (r.alive) cout << "RABBIT " << r.y << " " << r.x << endl;
    for (auto& f : foxes) if (f.alive) cout << "FOX " << f.y << " " << f.x << endl;
    
    return 0;
}
