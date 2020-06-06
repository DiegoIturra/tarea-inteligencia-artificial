#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <set>
#include <iterator>
#include <stack>
#include <map>
#include <queue>
#include <utility>
#include <numeric>
using namespace std;

typedef vector<vector<int>> Puzzle;

//denota la posicion de un elemento en el puzzle
struct Posicion{
    int x;
    int y;
};

//comparador
struct myComp{
    constexpr bool operator()(
        pair<Puzzle,int> const& a,
        pair<Puzzle,int> const& b)
        const noexcept{
            return a.second > b.second;
        }
};

//funcion que imprime un tablero
void imprimirTablero(Puzzle& matriz,int& size){
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            cout << matriz[i][j] << " ";
        }
        puts("");
    }
    puts("");
}

//retorna la posicion en la cual se halla en numero 0
pair<int,int> findZero(Puzzle& puzzle,int size){
    pair<int,int> posicion;
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            if(puzzle[i][j] == 0){
                posicion = make_pair(i,j);
            }
        }
    }
    return posicion;
}

//crea un estado vecino en el cual mueve la celda vacia hacia la posicion superior
Puzzle moveUp(Puzzle& estadoActual , int& size ,pair<int,int>& posCero){
      Puzzle vecino = estadoActual; //vecino es el puzzle actual pero con un movimiento de casilla
      int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
      vecino[posCero.first][posCero.second] = vecino[posCero.first - 1][posCero.second]; // el vecino guarda la posicion de arriba
      vecino[posCero.first-1][posCero.second] = temp;
      return vecino;
}

//crea un estado vecino en el cual mueve la celda hacia la posicion inferior
Puzzle moveDown(Puzzle& estadoActual , int& size ,pair<int,int>& posCero){
    Puzzle vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first+1][posCero.second];
    vecino[posCero.first+1][posCero.second] = temp;
    return vecino;
}

//crea un estado vecino en el cual mueve una casilla a la derecha
Puzzle moveRight(Puzzle& estadoActual , int& size ,pair<int,int>& posCero){
    Puzzle vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first][posCero.second+1];
    vecino[posCero.first][posCero.second+1] = temp;
    return vecino;
}

//crea un estado vecino en el cual mueve una casilla a la izquierda
Puzzle moveLeft(Puzzle& estadoActual , int& size ,pair<int,int>& posCero){
    Puzzle vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first][posCero.second-1];
    vecino[posCero.first][posCero.second-1] = temp;
    return vecino;
}

//esta funcion comprueba si el estado actual es igual al estado al cual queremos llegar
bool testObjetivo(Puzzle& goal ,Puzzle& current , const int& size){
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            if(goal[i][j] != current[i][j]){
                return false;
            }
        }
    }
    return true;
}

//genera el puzzle resuelto , segun la dimension dada
Puzzle getObjetivo(const int& size){
    Puzzle goal;
    switch (size) {
      case 2:
        goal = {{1,2},{3,0}};
        break;
      case 3:
        goal = {{1,2,3},{4,5,6},{7,8,0}};
        break;
      case 4:
        goal = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}};
        break;
    }
    return goal;
}

//este metodo retorna true si el top del stack es igual al nodo actual,en otro caso retorna false
bool enFrontera(stack<Puzzle>& frontera,Puzzle& estadoActual){
    Puzzle topFrontera;
    bool onTop = false;
    //si el stack no está vacio
    if(!frontera.empty()){
        topFrontera = frontera.top();
        onTop = true;
    }
    //si hay un elemento en el top,comprobamos si es igual al estad actual
    if(onTop){
        if(testObjetivo(topFrontera,estadoActual,estadoActual.size())){
            return true;
        }
    }
    return false;
}

//este metodo retorna true si el front de la pq es igual al nodo actual , en otro caso retorna falso
bool isFront(priority_queue<pair<Puzzle,int>,vector<pair<Puzzle,int>>,myComp > frontera,Puzzle& estadoActual){
    Puzzle topFrontera;
    bool onTop = false;

    if(!frontera.empty()){
        topFrontera = frontera.top().first;
        onTop = true;
    }
    if(onTop){
        if(testObjetivo(topFrontera,estadoActual,estadoActual.size())){
            return true;
        }
    }
    return false;
}

//este metodo retorna true si el estado esta en el conjunto de explorados , en otro caso retorna false
bool fue_explorado(set<Puzzle>& nodosExplorados , Puzzle& estadoActual){
    if(nodosExplorados.find(estadoActual) != nodosExplorados.end()){
        return true; //elemento esta en el set
    }else{
        return false; //no esta en el set
    }
}

//este metodo genera los vecinos del puzzle actual
vector<Puzzle> generarVecinos(Puzzle& nodoActual){
    vector<Puzzle> vecinos;
    int size = nodoActual.size();

    /*existen 4 posibles movimientos para el puzzle actual*/
    pair<int,int> posVacia = findZero(nodoActual,size);
    //movimiento hacia arriba
    if(posVacia.first-1 >= 0){
        vecinos.push_back(moveUp(nodoActual,size,posVacia));
    }
    //movimiento hacia abajo
    if(posVacia.first+1 < size){
        vecinos.push_back(moveDown(nodoActual,size,posVacia));
    }
    //movimiento hacia la derecha
    if(posVacia.second+1 < size){
        vecinos.push_back(moveRight(nodoActual,size,posVacia));
    }
    //movimiento hacia la izquierda
    if(posVacia.second-1 >= 0){
        vecinos.push_back(moveLeft(nodoActual,size,posVacia));
    }
    return vecinos;
}


bool dfs(Puzzle& estadoInicial , Puzzle& estadoFinal,int& size){
    stack<Puzzle> frontera; //nodos en la frontera
    set<Puzzle> explorado; //nodos que ya han sido explorados

    frontera.push(estadoInicial); //agregar el estado inicial a la frontera

    while(!frontera.empty()){
        Puzzle state = frontera.top(); //obtenemos el nodo actual
        frontera.pop(); //lo sacamos del stack
        explorado.insert(state); //el estado actual ya ha sido expolorado

        //si el estado que analizamos es igual al objetivo , entonces retorna exito
        if(testObjetivo(state,estadoFinal,size)){
            return true;
        }

        //insertar los vecinos del estado actual
        vector<Puzzle> vecinos = generarVecinos(state);

        vector<Puzzle>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            if(!enFrontera(frontera,*it) && !fue_explorado(explorado,*it)){
                frontera.push(*it);
                imprimirTablero(*it,size);
            }
        }
    }
    return false;
}

/*calcula el la heuristica como la distancia manhattan entre una casilla a su posicion correcta
recibe el estadoActual y un mapa que tiene como clave el elemento actual y su valor la posicion en su
forma correcta
*/
int heuristic(Puzzle& estadoActual , map<int,Posicion>& goalPos, int size){
    int h = 0;
    map<int,Posicion>::iterator it;
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            it = goalPos.find(estadoActual[i][j]);
            h += abs(i - it->second.x)  + abs(j - it->second.y);
        }
    }
    return h;
}

Puzzle generateRandomPuzzle(const int& size){
    vector<int> numeros(size*size);
    Puzzle puzzle(size,vector<int>(size));
    iota(numeros.begin(),numeros.end(),0); //genera vector de 0 a n-1
    random_shuffle(numeros.begin(),numeros.end()); //randomiza los elementos del vector

    //rellenar puzzle con los valores aleatorios
    int contador = 0;
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            puzzle[i][j] = numeros[contador];
            contador++;
        }
    }
    return puzzle;
}

//generar un map con el valor y la posicion en el estadp final del puzzle
void setPos_EstadoFinal(map<int,Posicion>& goalPos,const int& dimension){
    //recorrer el goal
    int contador = 1;
    Posicion pos;
    for(int i=0 ; i<dimension ; i++){
        for(int j=0 ; j<dimension ; j++){
            pos.x = i;
            pos.y = j;

            //agrega el 0 en la ultima casilla y no en la primera
            if(i == dimension-1 && j == dimension-1){
                goalPos[0] = pos;
                break;
            }
            goalPos[contador] = pos;
            contador++;
        }
    }
}


//gredy(inicio,fin,posicionesGoal,mapHeuristicas)
bool greedy(Puzzle& estadoActual , Puzzle& estadoFinal , map<int,Posicion>& goalPos ,map<Puzzle,int>& tableroHeuristicas){
    int size = estadoFinal.size();
    priority_queue<pair<Puzzle,int>,vector<pair<Puzzle,int>>,myComp > frontera;
    int h = heuristic(estadoActual,goalPos,size);
    tableroHeuristicas[estadoActual] = h;
    frontera.push(make_pair(estadoActual,h));
    set<Puzzle> explorados;

    while(!frontera.empty()){
        Puzzle state = frontera.top().first; //tomamos el estado con la menor heuristica
        frontera.pop(); //lo sacamos de la PQ
        explorados.insert(state); //estado ya ha sido explorado

        if(testObjetivo(state,estadoFinal,size)){
            return true;
        }

        //insertar los vecinos del estado actual
        vector<Puzzle> vecinos = generarVecinos(state);

        vector<Puzzle>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            if(!isFront(frontera,*it) && !fue_explorado(explorados,*it)){
                h = heuristic(*it,goalPos,size);
                tableroHeuristicas[*it] = h;
                frontera.push(make_pair(*it,h));
                explorados.insert(*it);
                imprimirTablero(*it,size);
            }
        }
    }
    return false;
}


int main(){

    //definir la dimension del tablero
    int dimension;
    puts("ingrese dimension");
    cin >> dimension;

    //generar el estado objetivo
    Puzzle goal = getObjetivo(dimension);

    //generar un estado random desde el cual partir
    Puzzle current = generateRandomPuzzle(dimension);

    //mapa que guarda elemento y su posicion en el puzzle objetivo
    //sera usado para calcular la heuristica con respecto a las posiciones originales
    map<int,Posicion> goalPos;
    setPos_EstadoFinal(goalPos,dimension);

    //mapa de un estado con su respectiva heuristica
    //map <puzzle, heuristica>
    map< Puzzle , int > heuristicasTablero;

    bool f = greedy(current,goal,goalPos,heuristicasTablero);
    cout << f << endl;

    return 0;
}
