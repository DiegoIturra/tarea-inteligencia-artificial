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

struct Matriz{
    Puzzle estado;
    Puzzle parent;
    int f;
    int h;
    int g;

    //para poder insertar dentro de un set ordenado con menor f
    bool operator<(const Matriz& m) const{
      return (this->f < m.f);
    }
};


//comparador
struct myComp{
    constexpr bool operator()(pair<Puzzle,int> const& a, pair<Puzzle,int> const& b) const noexcept{
            return a.second > b.second;
    }
};

//funcion que imprime un tablero
void imprimirTablero(Puzzle& matriz,int size){
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
bool testObjetivo(Puzzle goal ,Puzzle& current , const int& size){
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
bool enFrontera(queue<Puzzle>& frontera,Puzzle& estadoActual){
    Puzzle topFrontera;
    bool onTop = false;
    //si el stack no está vacio
    if(!frontera.empty()){
        topFrontera = frontera.front();
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

//este metodo retorna true si el estado esta en el conjunto de explorados
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

/*lLgoritmo DFS , busqueda no informada
  parametros(EstadoActual , meta , dimension tablero)
*/
bool depth_first_search(Puzzle& estadoInicial , Puzzle& estadoFinal,int& size){
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
            if(!fue_explorado(explorado,*it)){
                frontera.push(*it);
                imprimirTablero(*it,size);
            }
        }
    }
    return false;
}


/*Algoritmo BFS , busqueda no informada*/
bool breadth_first_search(Puzzle& estadoInicial , Puzzle& estadoFinal ,const int& size){
    queue<Puzzle> frontera;
    frontera.push(estadoInicial);
    set<Puzzle> explorados;

    while(!frontera.empty()){
        Puzzle state = frontera.front();
        frontera.pop();
        explorados.insert(state);

        if(testObjetivo(estadoFinal,state,size)){
            imprimirTablero(estadoFinal,size);
            return true;
        }

        vector<Puzzle> vecinos = generarVecinos(state);
        vector<Puzzle>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            if(!fue_explorado(explorados,*it)){
                frontera.push(*it);
                explorados.insert(*it);
                imprimirTablero(*it,size);
            }
        }
    }
    return false;
}

/*calcula el la heuristica como la distancia manhattan entre una casilla a su posicion correcta
recibe el estadoActual y un mapa que tiene como clave el elemento actual y su valor es la posicion en su
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

/*Genera un puzzle con un orden aleatorio */
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

//generar un mapa con el valor y la posicion en el estadp final del puzzle
//el cual sirve para calcular las heuristicas con respecto a las posiciones correctas
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
bool greedy(Puzzle& estadoActual , Puzzle& estadoFinal , map<int,Posicion>& goalPos ,vector<Puzzle>& camino){
    int size = estadoFinal.size();
    priority_queue<pair<Puzzle,int>,vector<pair<Puzzle,int>>,myComp > frontera;
    int h = heuristic(estadoActual,goalPos,size);
    frontera.push(make_pair(estadoActual,h));
    set<Puzzle> explorados;


    while(!frontera.empty()){
        Puzzle state = frontera.top().first; //tomamos el estado con la menor heuristica
        frontera.pop(); //lo sacamos de la PQ
        explorados.insert(state); //estado ya ha sido explorado

        camino.push_back(state); //agrega el estado al camino

        if(testObjetivo(state,estadoFinal,size)){
            return true;
        }

        //insertar los vecinos del estado actual
        vector<Puzzle> vecinos = generarVecinos(state);

        vector<Puzzle>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            /*para cada vecino del estado actual que no haya sido explorado y que no este en la enFrontera
            calcular su heuristica y agregarlo a la frontera y marcarlo como explorado*/
            if(!fue_explorado(explorados,*it)){
                h = heuristic(*it,goalPos,size);
                frontera.push(make_pair(*it,h));
                explorados.insert(*it);
            }
        }
    }
    return false;
}


//devuelve el estado con la menor funcion f(n)
Matriz findMin_funcion(set<Matriz> my_set){
    Matriz min_element;
    if (!my_set.empty()){
        min_element = *my_set.begin();
    }
    return min_element;
}


/*ALgoritmo A y algunas funciones auxiliares*/
bool estaEnConjunto(set<Matriz>& conjunto , Puzzle& current){
    set<Matriz>::iterator it;
    //recorremos el conjunto y si el test objetivo es igual al nodo que buscamos retorna true
    for(it = conjunto.begin() ; it != conjunto.end() ; it++){
        if(testObjetivo(it->estado,current,current.size())){
            return true;
        }
    } //caso contrario retorna false
    return false;
}

//A* NO FUNCIONA DE FORMA ADECUADA (MALA IMPLEMENTACION)
bool A_Star(Puzzle& start , Puzzle& goal , map<int,Posicion>& goalPos){
    int size = goal.size();
    set<Matriz> open_list;
    set<Matriz> closed_list;

    //matriz con la info de el nodo de partida
    Matriz infoStart;
    infoStart.estado = start;
    infoStart.g = 0;
    infoStart.h = heuristic(start,goalPos,size);
    infoStart.f = infoStart.g + infoStart.h;

    //insertamos start en open_list
    open_list.insert(infoStart);

    while(!open_list.empty()){
        Matriz current = findMin_funcion(open_list); //obtenemos el estado con menor f(n)

        if(testObjetivo(current.estado , goal , size)){
            imprimirTablero(current.estado,size);
            return true;
        }

        open_list.erase(current); //remover de open list
        closed_list.insert(current);  //agregar a closed list

        vector<Puzzle> vecinos = generarVecinos(current.estado); //generar vecinos del estado actual
        vector<Puzzle>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            Puzzle vecino = *it;
            //convertimos un estado a su informacion total
            Matriz info;
            info.estado = vecino;
            info.g = 1;
            info.h = heuristic(vecino,goalPos,size);
            info.f = info.h + info.g;

            if(estaEnConjunto(open_list,vecino)){
                continue;
            }

            int costo = current.g + 1;

            if(estaEnConjunto(open_list,vecino) && costo < info.g){
                open_list.erase(info);
            }
            if(estaEnConjunto(closed_list,vecino) && costo < info.g){
                closed_list.erase(info);
            }
            if(!estaEnConjunto(open_list,vecino) && !estaEnConjunto(closed_list,vecino)){
                open_list.insert(info);
                info.g = costo;
                info.h = heuristic(info.estado,goalPos,size);
                info.f = info.g + info.h;
            }
        }
    }
    return false;
}



void ejecutarDFS(Puzzle& start,Puzzle& goal){
    int size = goal.size();
    bool solucion = depth_first_search(start,goal,size);
    if(!solucion){
        puts("Puzzle no tiene solucion");
    }
}

void ejecutarBFS(Puzzle& start , Puzzle& goal){
    int size = goal.size();
    bool solucion = breadth_first_search(start,goal,size);
    if(!solucion){
        puts("Puzzle no tiene solucion");
    }
}

void ejecutarGreedy(Puzzle& start , Puzzle& goal , map<int,Posicion>& goalPos ,vector<Puzzle>& camino){
    int size = goal.size();
    bool solucion = greedy(start,goal,goalPos,camino);
    if(solucion){
        vector<Puzzle>::iterator it;
        for(it = camino.begin() ; it != camino.end() ; it++){
            imprimirTablero(*it,size);
        }
    }else{
        puts("Puzzle no tiene solucion");
    }
}

int main(){

    //definir la dimension del tablero
    int dimension;
    string algoritmo;
    puts("ingrese dimension (2,3 o 4)");
    cin >> dimension;
    puts("probar algoritmo: dfs,bfs,greedy");
    cin >> algoritmo;

    //generar el estado objetivo
    Puzzle goal = getObjetivo(dimension);

    //generar un estado random desde el cual partir
    Puzzle current = generateRandomPuzzle(dimension);

    //mapa que guarda elemento y su posicion en el puzzle objetivo
    //sera usado para calcular la heuristica con respecto a las posiciones originales
    map<int,Posicion> goalPos;
    setPos_EstadoFinal(goalPos,dimension);

    vector<Puzzle> camino; //vector que guarda el camino con menor heuristica para algoritmo greedy

    //ejecucion de algoritmos
    if(algoritmo == "dfs"){
        ejecutarDFS(current,goal);
    }else if(algoritmo == "bfs"){
        ejecutarBFS(current,goal);
    }else if(algoritmo == "greedy"){
        ejecutarGreedy(current,goal,goalPos,camino);
    }

    return 0;
}
