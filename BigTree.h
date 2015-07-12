#ifndef BIGTREE_H
#define	BIGTREE_H

/*
 * 
Properties of B-Tree
    1) All leaves are at same level.
    2) A B-Tree is defined by the term minimum degree ‘t’..
    3) Every node except root must contain at least t-1 keys. Root may contain minimum 1 key.
    4) All nodes (including root) may contain at MOST  2t – 1 keys.
    5) Number of children of a node is equal to the number of keys in it plus 1.
    6) All keys of a node are sorted in increasing order. The child between two keys k1 and k2 contains all keys in range from k1 and k2
    7) B-Tree grows and shrinks from root which is unlike  Binary Search Tree that grow downward and also shrink from downward.
    8) Like other balanced Binary Search Trees, time complexity to search, insert and delete is O(Logn).
Source [http://www.geeksforgeeks.org/b-tree-set-1-introduction-2/]
 * 
 */
#include<iostream>
using namespace std;

/* Clase que va a representar a un nodo del arbol */
class BTreeNode {
    
private:
    int * keys; // Arreglo de numeros en el nodo
    int degree; // Define el grado del nodo (este es compartido por todos los nodos del arbol)
    BTreeNode ** children; // Un arreglo de punters hacia los nodos hijos
    int number_keys; //numero actual de numeros (keys)  que se encuentran en el nodo
    bool leaf; // Es un nodo hoja o no
    
public:
    BTreeNode(int _t, bool _leaf); // Constructor

    //Una funcion para recorrer todos los nodos en un subarbol arraigado con este nodo
    void traverse();

    //Una funcion para buscar una llave en un subarbol arraigado en este nodo
    BTreeNode *search(int k); // returns NULL if k is not present.

    //Una funcion que retorna el indice de la primera llave que >= k
    int findKey(int k);

    /* Una funcion para insertar una nueva llave en el subarbol arraigado con este nodo.
     Se asume que el nodo no esta lleno cuando se llama la funcion*/
    void insertNonFull(int k);

    //Funcion para separar el hijo y de este nodo. i es el indice de y en el
    //array hijo C[]. El hijo c debe estar lleno cuando se llame esta funcion
    void splitChild(int i, BTreeNode *y);

    //Remueve la llave k en el subarbol arraigado en este nodo
    void remove(int k);

    //Remueve la llave en la posicion idk en este nodo, la cual es hoja
    void removeFromLeaf(int idx);

    //Una funcion para eliminar la llave en la posicion idx en este nodo, la cual NO es hoja
    void removeFromNonLeaf(int idx);

    //Obtiene el predecesor de la llave donde esta esta presente en la posicion idx del nodo
    int getPred(int idx);

    //Obtiene el sucesor de la llave donde esta esta presente en la posicion idx del nodo
    int getSucc(int idx);

    /* Llena el nodo hijo presente en la posicion idx del array C[] si ese hijo 
     tiene menos de t-1 llaves */
    void fill(int idx);

    //Se presta una llave del C[idx-1]nodo y lo pone en el C[idx] nodo
    void borrowFromPrev(int idx);

    //Se presta una llave del C[idx+1] nodo y lo pone en el C[idx] nodo
    void borrowFromNext(int idx);

    //Une el idx-hijo del nodo con el idx+1 hijo del nodo
    void merge(int idx);

    //Friendear el BTree para accesar las funciones privadas de esta clase
    friend class BigTree;
};

class BigTree {
 private:
    BTreeNode *root; // Puntero a la raiz
    int tree_degree; // Grado minimo

public:
   static void BigMenu();

    // Constructor

    BigTree(int _degree) {
        root = NULL;
        tree_degree = _degree;
    }

    void traverse() {
        if (root != NULL) root->traverse();
    }

    // Busca una llave en el arbol

    BTreeNode* search(int k) {
        return (root == NULL) ? NULL : root->search(k);
    }

    // Inserta una nueva llave en el arbol
    void insert(int k);

    // Elimina una llave del arbol
    void remove(int k);




};

#endif	/* BIGTREE_H */

