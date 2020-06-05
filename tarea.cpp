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
using namespace std;

//mapea un estado con su respectiva heuristica
map< vector<vector<int>> , int > heuristicasTablero;

//estructura que representa un nodo
struct Tree{
    vector<vector<int>> estadoActual; //puzzle actual
};

//funcion que imprime un tablero
void imprimirTablero(vector<vector<int>>& matriz,int& size){
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            cout << matriz[i][j] << " ";
        }
        puts("");
    }
    puts("");
}

//asigna memoria a un nuevo nodo y retorna la referencia
Tree* newNode(Tree* node , int size){
    node = new Tree(); //creamos un nodo
    node->estadoActual.resize(size,vector<int>(size)); //asignamos memoria para estado estadoActual
    return node;
}

//retorna la posicion en la cual se halla en numero 0
pair<int,int> findZero(vector<vector<int>>& puzzle,int size){
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
vector<vector<int>> moveUp(vector<vector<int>>& estadoActual , int& size ,pair<int,int>& posCero){
      vector<vector<int>> vecino = estadoActual; //vecino es el puzzle actual pero con un movimiento de casilla
      int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
      vecino[posCero.first][posCero.second] = vecino[posCero.first - 1][posCero.second]; // el vecino guarda la posicion de arriba
      vecino[posCero.first-1][posCero.second] = temp;
      return vecino;
}

//crea un estado vecino en el cual mueve la celda hacia la posicion inferior
vector<vector<int>> moveDown(vector<vector<int>>& estadoActual , int& size ,pair<int,int>& posCero){
    vector<vector<int>> vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first+1][posCero.second];
    vecino[posCero.first+1][posCero.second] = temp;
    return vecino;
}

//crea un estado vecino en el cual mueve una casilla a la derecha
vector<vector<int>> moveRight(vector<vector<int>>& estadoActual , int& size ,pair<int,int>& posCero){
    vector<vector<int>> vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first][posCero.second+1];
    vecino[posCero.first][posCero.second+1] = temp;
    return vecino;
}

//crea un estado vecino en el cual mueve una casilla a la izquierda
vector<vector<int>> moveLeft(vector<vector<int>>& estadoActual , int& size ,pair<int,int>& posCero){
    vector<vector<int>> vecino = estadoActual;
    int temp = estadoActual[posCero.first][posCero.second]; //guarda el valor cero
    vecino[posCero.first][posCero.second] = vecino[posCero.first][posCero.second-1];
    vecino[posCero.first][posCero.second-1] = temp;
    return vecino;
}

//esta funcion comprueba si el estado actual es igual al estado al cual queremos llegar
bool testObjetivo(vector<vector<int>>& goal ,vector<vector<int>>& current , const int& size){
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
vector<vector<int>> getObjetivo(const int& size){
    vector<vector<int>> goal;
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
bool enFrontera(stack<vector<vector<int>>>& frontera,vector<vector<int>>& estadoActual){
    vector<vector<int>> topFrontera;
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

//este metodo retorna true si el estado esta en el conjunto de explorados , en otro caso retorna false
bool fue_explorado(set<vector<vector<int>>>& nodosExplorados , vector<vector<int>>& estadoActual){
    if(nodosExplorados.find(estadoActual) != nodosExplorados.end()){
        return true; //elemento esta en el set
    }else{
        return false; //no esta en el set
    }
}

//este metodo genera los vecinos del puzzle actual
vector<vector<vector<int>>> generarVecinos(vector<vector<int>>& nodoActual,int size){
    vector<vector<vector<int>>> vecinos;

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


bool dfs(vector<vector<int>>& estadoInicial , vector<vector<int>>& estadoFinal,int& size){
    stack<vector<vector<int>>> frontera; //nodos en la frontera
    set<vector<vector<int>>> explorado; //nodos que ya han sido explorados

    frontera.push(estadoInicial); //agregar el estado inicial a la frontera

    while(!frontera.empty()){
        vector<vector<int>> state = frontera.top(); //obtenemos el nodo actual
        frontera.pop(); //lo sacamos del stack
        explorado.insert(state); //el estado actual ya ha sido expolorado

        //si el estado que analizamos es igual al objetivo , entonces retorna exito
        if(testObjetivo(state,estadoFinal,size)){
            return true;
        }

        //insertar los vecinos del estado actual
        vector<vector<vector<int>>> vecinos = generarVecinos(state,size);

        vector<vector<vector<int>>>::iterator it;
        for(it = vecinos.begin() ; it != vecinos.end() ; it++){
            if(!enFrontera(frontera,*it) && !fue_explorado(explorado,*it)){
                frontera.push(*it);
                imprimirTablero(*it,size);
            }
        }
    }
    return false;
}



int main(){

    //dimension del tablero
    int dimension;
    puts("ingrese dimension");
    cin >> dimension;

    //crear arbol de estados vacio
    Tree* root = NULL;

    //crear un nodo raiz
    root = newNode(root,dimension);

    //creamos un estado inicial de prueba
    int contador = (dimension*dimension)-1;
    for(int i=0 ; i<dimension ; i++){
        for(int j=0 ; j<dimension ; j++){
            root->estadoActual[i][j] = contador;
            contador--;
        }
    }
    //imprimir
    imprimirTablero(root->estadoActual,dimension);

    vector<vector<int>> goal = getObjetivo(dimension);
    bool f = dfs(root->estadoActual,goal,dimension);
    cout << f << endl;

    delete root;

    return 0;
}
