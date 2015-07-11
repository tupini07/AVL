#include "BigTree.h"

/* Constructor para la clase del nodo del Big Tree */
BTreeNode::BTreeNode(int _degree, bool _leaf) {
    
    // Asignamos los valores a las variables del objeto
    degree = _degree;
    leaf = _leaf;

    //Reservamos la maxima cantidad de memoria para el arreglo de las llavs y para el de los hijos
    keys = new int[2 * degree - 1]; //El valor maximo de llaves es 2*grado-1
    children = new BTreeNode *[2 * degree]; //El valor maximo de hijos es de 2*grado

    // inicializamos el numero actual de llaves en 0, ya que no hay llaves al crearse el nodo
    number_keys = 0;
}


/* Esta funcion se encarga de atravesar el arbol, desplegando cada numero en orden de menor a mayor,
 * podria ser algo mejor que realmente muestre la forma del arbol */
void BTreeNode::traverse() {
    
    //Desplegamos todos los hijos de forma recursiva, empezando por aquellos que se encuentren mas
    //a la izquierda.
    int i;
    for (i = 0; i <= number_keys; i++) {
        
        //si no nos encontramos actualmente en una hoja entonces volvemos a hacer la llamada recursiva a 
        //este metodo pero esta ves al hijo[i]. Esto lo realizamos antes de imprimir en pantalla la llave 'i' del 
        //nodo actual.
        if (leaf == false)
            children[i]->traverse();
        if (i != number_keys) cout << "  " << keys[i]; //esto daria un segmentatio fail si no nos fijaramos si 'i != number_keys'
    }

}

/* Esta funcion retorna un puntero al nodo que contiene un valor 'k' */
BTreeNode *BTreeNode::search(int k) {
    
    // encuentra la primer llave mayor o igual a 'k'
    //en otras palabras, el siguiente while aumenta el valor de 'i' en 1 mientras con la condicion de que el valor de 
    // i sea menor al numero de llaves en el nodo y que al mismo tiempo el valor de k  sea mayor al valor de la llave que 
    //se encuentra en la posicion i
    int i = 0;
    while (i < number_keys && k > keys[i])
        i++;

    // Si la llave encontrada es igual a k entonces retornamos un puntero a este nodo
    if (keys[i] == k)
        return this;

    //Si la llave no fue encontrada (su hibiera sido no esta parte de codigo no llegaria a ejecutarse nunca)
    //entonces nos fijamos si el nodo actual es una hoja, en caso que lo fuera sabemos que el valor
    //que estamos buscando no fue encontrado, por lo tanto retornamos NULL
    if (leaf == true)
        return NULL;

    // Aplicamos otra vez la busqueda al nodo apropiado
    return children[i]->search(k);
}

/* Esta es la funcion principal cuando se trata de insertar algo en el arbol, 
 * es la que inicia toda la cadena de acciones necesarias para una correcta incercion */
void BigTree::insert(int k) {
    
    // Si el arbol esta vacio entonces simplemente hacemos un nuevo nodo y le asignamos la llave que queremos insertar
    if (root == NULL) {
         //creamos un nuevo nodo que tiene un grado igual al grado del arbol y tambien le pasamos el parametro 'true' 
        //para simbolizar que dicho nodo es una hoja
        root = new BTreeNode(tree_degree, true);
        root->keys[0] = k; // Insertamos la llave
        root->number_keys = 1; // Modificamos el numero de llaves que contiene el nodo para reflejar el cambio
        
    } else { //en el caso de que el arbol no se encuentre vacio
        
        // Si el arbol no se encuentra vacio lo primero que hacemos es fijarnos si la raiz esa llena
        //si lo esta entonces crecemos en altura. Hay que tomar en cuenta que la cantidad maxima de llaves
        //que puede tener un nodo es de 2*grad_arbol -1
        if (root->number_keys == 2 * tree_degree - 1) {
            
            BTreeNode * new_node = new BTreeNode(tree_degree, false);

            // Asignamos al nodo que se encuentra actualmente en la raiz como hijo del nuevo nodo
            new_node->children[0] = root;

            //Separamos a la raiz y movemos una llave de la raiz al nuevo nodo
            new_node->splitChild(0, root);

            //Actualmente la nueva raiz tiene 2 hijos, por lo tanto hay que decidir cual de los dos va a tener la una nueva llave
            //(esta seria la llave que estamos intentando insertar). Para decidir esto vemos si k es mayor que la raiz actual (en cuyo caso iria en 
            //el segundo hijo que tiene valores mayores) o si es menor (en cuyo caso iria en el primer hijo que tiene valores 
            //menores que la raiz).
            int i = 0;
            if (new_node->keys[0] < k)
                i++;
            new_node->children[i]->insertNonFull(k);

            // cambiamos el puntero de la razi para que apunte al nuevo nodo
            root = new_node;
        } else // Si la raiz no esta llena entonces llamamos al metodo insertNonFull sobre la raiz, pasandole 'k'
            root->insertNonFull(k);
    }
}

/* Se encarga de insertar una llave en el nodo actual. Se asume que el nodo actual no esta lleno cuando esta
 * funcion se ejecuta, es responsabilidad de la funcion que llama a esta fijarse que este sea el caso */
void BTreeNode::insertNonFull(int k) {
    // Inicializamos 'i' como el index del elemento de mayor tamanno (aquel que se encuentra mas a las derecha)
    int i = number_keys - 1;

    // Si el nodo actual es una hoja..
    if (leaf == true) {
        
        //El siguiente while hace dos cosas
        //      a) Encuentra la posicion en la que insertar la nueva llave
        //      b) Mueve todas las llaves mayores que la nueva llave un campo a la derecha
        //Este while se realiza una vez por cada llava en el nodo mientras el valor de la llave de la iteracion
        //X sea mayor que k (Apenas vemos que k es mayor q la llave de interes sabemos que k tiene que ser isertado
        //entre dicha llave y la llave inmediatamente mayor)
        while (i >= 0 && keys[i] > k) {
            if(k == keys[i]) return; //no queremos que haya valores repetidos
            keys[i + 1] = keys[i]; //movemos la llave que esta en el indice i un espacio para la derecha (a i+1)
            i--;
        }

        // Insertamos la nueva llave en la posicion encontrada
        keys[i + 1] = k;
        number_keys = number_keys + 1; ////////////////////////////////////////////-----------------------------------------------------------------------MACK AQUI quede
    } else // If this node is not leaf
    {
        // Find the child which is going to have the new key
        while (i >= 0 && keys[i] > k)
            i--;

        // See if the found child is full
        if (children[i + 1]->number_keys == 2 * degree - 1) {
            // If the child is full, then split it
            splitChild(i + 1, children[i + 1]);

            // After split, the middle key of children[i] goes up and
            // children[i] is splitted into two.  See which of the two
            // is going to have the new key
            if (keys[i + 1] < k)
                i++;
        }
        children[i + 1]->insertNonFull(k);
    }
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called

void BTreeNode::splitChild(int i, BTreeNode *y) {
    // Create a new node which is going to store (degree-1) keys
    // of y
    BTreeNode *z = new BTreeNode(y->degree, y->leaf);
    z->number_keys = degree - 1;

    // Copy the last (degree-1) keys of y to z
    for (int j = 0; j < degree - 1; j++)
        z->keys[j] = y->keys[j + degree];

    // Copy the last degree children of y to z
    if (y->leaf == false) {
        for (int j = 0; j < degree; j++)
            z->children[j] = y->children[j + degree];
    }

    // Reduce the number of keys in y
    y->number_keys = degree - 1;

    // Since this node is going to have a new child,
    // create space of new child
    for (int j = number_keys; j >= i + 1; j--)
        children[j + 1] = children[j];

    // Link the new child to this node
    children[i + 1] = z;

    // A key of y will move to this node. Find location of
    // new key and move all greater keys one space ahead
    for (int j = number_keys - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    // Copy the middle key of y to this node
    keys[i] = y->keys[degree - 1];

    // Increment count of keys in this node
    number_keys = number_keys + 1;
}

// A utility function that returns the index of the first key that is
// greater than or equal to k

int BTreeNode::findKey(int k) {
    int idx = 0;
    while (idx < number_keys && keys[idx] < k)
        ++idx;
    return idx;
}

// A function to remove the key k from the sub-tree rooted with this node

void BTreeNode::remove(int k) {
    int idx = findKey(k);

    // The key to be removed is present in this node
    if (idx < number_keys && keys[idx] == k) {

        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    } else {

        // If this node is a leaf node, then the key is not present in tree
        if (leaf) {
            cout << "The key " << k << " is does not exist in the tree\number_keys";
            return;
        }

        // The key to be removed is present in the sub-tree rooted with this node
        // The flag indicates whether the key is present in the sub-tree rooted
        // with the last child of this node
        bool flag = ((idx == number_keys) ? true : false);

        // If the child where the key is supposed to exist has less that degree keys,
        // we fill that child
        if (children[idx]->number_keys < degree)
            fill(idx);

        // If the last child has been merged, it must have merged with the previous
        // child and so we recurse on the (idx-1)th child. Else, we recurse on the
        // (idx)th child which now has atleast degree keys
        if (flag && idx > number_keys)
            children[idx - 1]->remove(k);
        else
            children[idx]->remove(k);
    }
    return;
}

// A function to remove the idx-th key from this node - which is a leaf node

void BTreeNode::removeFromLeaf(int idx) {

    // Move all the keys after the idx-th pos one place backward
    for (int i = idx + 1; i < number_keys; ++i)
        keys[i - 1] = keys[i];

    // Reduce the count of keys
    number_keys--;

    return;
}

// A function to remove the idx-th key from this node - which is a non-leaf node

void BTreeNode::removeFromNonLeaf(int idx) {

    int k = keys[idx];

    // If the child that precedes k (children[idx]) has atleast degree keys,
    // find the predecessor 'pred' of k in the subtree rooted at
    // children[idx]. Replace k by pred. Recursively delete pred
    // in children[idx]
    if (children[idx]->number_keys >= degree) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    }
        // If the child children[idx] has less that degree keys, examine children[idx+1].
        // If children[idx+1] has atleast degree keys, find the successor 'succ' of k in
        // the subtree rooted at children[idx+1]
        // Replace k by succ
        // Recursively delete succ in children[idx+1]
    else if (children[idx + 1]->number_keys >= degree) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    }
        // If both children[idx] and children[idx+1] has less that degree keys,merge k and all of children[idx+1]
        // into children[idx]
        // Now children[idx] contains 2t-1 keys
        // Free children[idx+1] and recursively delete k from children[idx]
    else {
        merge(idx);
        children[idx]->remove(k);
    }
    return;
}

// A function to get predecessor of keys[idx]

int BTreeNode::getPred(int idx) {
    // Keep moving to the right most node until we reach a leaf
    BTreeNode *cur = children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->number_keys];

    // Return the last key of the leaf
    return cur->keys[cur->number_keys - 1];
}

int BTreeNode::getSucc(int idx) {

    // Keep moving the left most node starting from children[idx+1] until we reach a leaf
    BTreeNode *cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];

    // Return the first key of the leaf
    return cur->keys[0];
}

// A function to fill child children[idx] which has less than degree-1 keys

void BTreeNode::fill(int idx) {

    // If the previous child(children[idx-1]) has more than degree-1 keys, borrow a key
    // from that child
    if (idx != 0 && children[idx - 1]->number_keys >= degree)
        borrowFromPrev(idx);

        // If the next child(children[idx+1]) has more than degree-1 keys, borrow a key
        // from that child
    else if (idx != number_keys && children[idx + 1]->number_keys >= degree)
        borrowFromNext(idx);

        // Merge children[idx] with its sibling
        // If children[idx] is the last child, merge it with with its previous sibling
        // Otherwise merge it with its next sibling
    else {
        if (idx != number_keys)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// A function to borrow a key from children[idx-1] and insert it
// into children[idx]

void BTreeNode::borrowFromPrev(int idx) {

    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx - 1];

    // The last key from children[idx-1] goes up to the parent and key[idx-1]
    // from parent is inserted as the first key in children[idx]. Thus, the  loses
    // sibling one key and child gains one key

    // Moving all key in children[idx] one step ahead
    for (int i = child->number_keys - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    // If children[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->leaf) {
        for (int i = child->number_keys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    // Setting child's first key equal to keys[idx-1] from the current node
    child->keys[0] = keys[idx - 1];

    // Moving sibling's last child as children[idx]'s first child
    if (!leaf)
        child->children[0] = sibling->children[sibling->number_keys];

    // Moving the key from the sibling to the parent
    // This reduces the number of keys in the sibling
    keys[idx - 1] = sibling->keys[sibling->number_keys - 1];

    child->number_keys += 1;
    sibling->number_keys -= 1;

    return;
}

// A function to borrow a key from the children[idx+1] and place
// it in children[idx]

void BTreeNode::borrowFromNext(int idx) {

    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx + 1];

    // keys[idx] is inserted as the last key in children[idx]
    child->keys[(child->number_keys)] = keys[idx];

    // Sibling's first child is inserted as the last child
    // into children[idx]
    if (!(child->leaf))
        child->children[(child->number_keys) + 1] = sibling->children[0];

    //The first key from sibling is inserted into keys[idx]
    keys[idx] = sibling->keys[0];

    // Moving all keys in sibling one step behind
    for (int i = 1; i < sibling->number_keys; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    // Moving the child pointers one step behind
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->number_keys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    // Increasing and decreasing the key count of children[idx] and children[idx+1]
    // respectively
    child->number_keys += 1;
    sibling->number_keys -= 1;

    return;
}

// A function to merge children[idx] with children[idx+1]
// children[idx+1] is freed after merging

void BTreeNode::merge(int idx) {
    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx + 1];

    // Pulling a key from the current node and inserting it into (degree-1)th
    // position of children[idx]
    child->keys[degree - 1] = keys[idx];

    // Copying the keys from children[idx+1] to children[idx] at the end
    for (int i = 0; i < sibling->number_keys; ++i)
        child->keys[i + degree] = sibling->keys[i];

    // Copying the child pointers from children[idx+1] to children[idx]
    if (!child->leaf) {
        for (int i = 0; i <= sibling->number_keys; ++i)
            child->children[i + degree] = sibling->children[i];
    }

    // Moving all keys after idx in the current node one step before -
    // to fill the gap created by moving keys[idx] to children[idx]
    for (int i = idx + 1; i < number_keys; ++i)
        keys[i - 1] = keys[i];

    // Moving the child pointers after (idx+1) in the current node one
    // step before
    for (int i = idx + 2; i <= number_keys; ++i)
        children[i - 1] = children[i];

    // Updating the key count of child and the current node
    child->number_keys += sibling->number_keys + 1;
    number_keys--;

    // Freeing the memory occupied by sibling
    delete(sibling);
    return;
}

void BigTree::remove(int k) {
    if (!root) {
        cout << "The tree is empty\number_keys";
        return;
    }

    // Call the remove function for root
    root->remove(k);

    // If the root node has 0 keys, make its first child as the new root
    //  if it has a child, otherwise set root as NULL
    if (root->number_keys == 0) {
        BTreeNode *tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->children[0];

        // Free the old root
        delete tmp;
    }
    return;
}

void BigTree::BigMenu() {
    using namespace std;
    BigTree  bigT (3);  //Create a Big tree with degree 3

    int choice, item;
    while (1) {
        cout << endl;
        cout << "---------------------" << endl;
        cout << "Arbol BigTree" << endl;
        cout << "---------------------" << endl;
        cout << "\t1.Insertar elemento" << endl;
        cout << "\t2.Desplegar Arbol" << endl;
        cout << "\t3.Eliminar Elemento" << endl;
        cout << "\t4.Salir" << endl;
        cout << "\tIngrese su opcion: ";
        cin>>choice;
        switch (choice) {
            case 1:
                cout << "Ingrese el valor deseado: ";
                cin>>item;
                bigT.insert(item);
                break;
            case 2:
                            if (bigT.root == NULL)
                            {
                                cout<<"Arbol vacio"<<endl;
                                continue;
                            }
                            cout<<"Arbol Rojo-Negro:"<<endl;
                            bigT.traverse(); //display the tree in Transverse mode;
                break;
            case 3:
                cout << "Ingrese el elemento a eliminar: ";
                cin>>item;
                bigT.remove(item);
                break;
            case 4:
                return;
                break;
            default:
                cout << "Opcion Invalida" << endl;
        }
    }
}
