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
    int i = number_keys - 1; //Al final del metodo este 'i' va a ser la posicion en la que vamos a insertar el nuevo valor

    // Si el nodo actual es una hoja..
    if (leaf == true) {
        
        /*El siguiente while hace dos cosas
             a) Encuentra la posicion en la que insertar la nueva llave
              b) Mueve todas las llaves mayores que la nueva llave un campo a la derecha
        Este while se realiza una vez por cada llave en el nodo mientras el valor de la llave de la iteracion
        X sea mayor que k (Apenas vemos que k es mayor q la llave de interes sabemos que k tiene que ser isertado
        entre dicha llave y la llave inmediatamente mayor)  
         Este es un metodo recursivo, la insercion solo se hace si el metodo actual es una hoja, si no lo es entonces
         se hacen las separaciones necesarias y recursivamente se elige en cual hoja debe de ser insertado el nuevo valor*/
        while (i >= 0 && keys[i] > k) {
            if(k == keys[i]) return; //no queremos que haya valores repetidos
            keys[i + 1] = keys[i]; //movemos la llave que esta en el indice i un espacio para la derecha (a i+1)
            i--;
        }

        // Insertamos la nueva llave en la posicion encontrada
        keys[i + 1] = k;
        number_keys = number_keys + 1; 
    } else // Si no es hoja entonces hacemos todas las separaciones necesarias y recursivamente llegamos al nodo hoja en el que vamos a insertar el valor
    {
        // Encuentra en que posicion (dentro de los valores del nodo actual) se va a poner el nuevo valor.
        //Para saber esto encontramos cual valor es inmediaatamente menor que 'k'
        while (i >= 0 && keys[i] > k) //empezamos desde el valor de la derecha que seria el mayor valor e iteramos hasta el valor de la izquierda (el menor valor)
            i--;

        // Una vez encontrado donde poner el nuevo valor nos fijamos si el hijo [i+1] del nodo actual (el hijo que estaria a la derecha del valor que queremos insertar)
        //esta lleno (en cuyo caso tenemos que separarlo)
        if (children[i + 1]->number_keys == 2 * degree - 1) {
            // Si esta lleno, entonces es separado 
            splitChild(i + 1, children[i + 1]);

            /* Despues de seperarse, la llave central del hijo[i] sube y nos fijamos si la nueva llave (la que subio) es menor que el hijo, si asi fuera el caso entonces
             * tenemos que escoger la posicion que esta a la derecha de esta nueva llave para insertar el hijo   */
            if (keys[i + 1] < k)
                i++;
        }
        //Recursivamente volvemos a realizar el mismo metodo sobre el hijo en el que deberiamos de insertar el nuevo valor. La razon por la que se esta insertando en el 
        //hijo i+1 es porque ya sabemos que el hijo i contiene valores que son todos menores a la llave i del nodo actual , por lo tanto si insertamos el nuevo valor en este
        //hijo estariamos rompiendo la regla de que todos los valores tienen que estar ordenados por valor
        children[i + 1]->insertNonFull(k);
    }
}

/*
 * Separa un hijo de este nodo. 
 * 'i' es el indice que tiene el hijo '*y' en el nodo actual
 * '*y'  es un puntero al nodo que queremos partir. Este DEBE de ser hijo del nodo mediante el cual se llama esta funcion 
 * 
 */
void BTreeNode::splitChild(int i, BTreeNode *y) {
    
    /*Situacion inicial:
     *              <nodo actual>
     *                              \
     *                             <*y>  i  </*y> 
     */
    
    
    //Nuevo nodo va a tener el mismo grado que el nodo 'y' y tambien el mismo valor de si es hora o no
    //este nodo 'z' va a simbolizar, despues de partir el nodo, la primera mitad de valores.
    BTreeNode *z = new BTreeNode(y->degree, y->leaf);
    
    z->number_keys = degree - 1;  //el nuevo numero de llaves que va a tener este nodo va a ser de 'grado-1'
                                                                    //(donde grado es el grado del nodo actual) [grado-1 es el minimo valor permitido] 

    //Se copian las llaves desde el nodo 'y' al nodo 'z' . Se copian solamente la SEGUNDA mitad de las llaves - 1. Sabemos que este
    //nodo tiene 2*grado-1 llaves (porque sino no habria razon para partirlo)
    for (int j = 0; j < degree - 1; j++)
        z->keys[j] = y->keys[j + degree];

    
    //si 'y' no es una hoja entonces copiamos los hijos de 'y' a 'z'. Se copian solamente la SEGUNDA mitad de los hijos
    if (y->leaf == false) {
        for (int j = 0; j < degree; j++)
            z->children[j] = y->children[j + degree];
    }

    //Ahora el numero de llaves en Y es igual a grado-1 ... Esto quiere decir que 'y' ahora 'tiene'  (realmente los punteros todavia estan en el arreglo de punteros
    // pero no se van a tomar en cuenta ya que el numero de llaves dice que no existen y pueden volver a ser usados libremente cuando se necesiten)
    //la minima cantidad permitida de hijos para un nodo que no es raiz
    y->number_keys = degree - 1;


    /*Tenemos que hacerle espacio al nuevo hijo (z)  a insertar, por lo tanto corremos todas los hijos (en el nodo actual)
        que se encuentran a la derecha de la posicion (o en la posicion)  'i+1' hacia la derecha */
    for (int j = number_keys;  j >= i + 1;  j--)
        children[j + 1] = children[j];

    //Ponemos el nuevo hijo en la posicion que acabamos de liberarle
    children[i + 1] = z;
    //no es necesario poner a 'y' como hijo porque realmente 'y' es el hijo ['i'] del nodo actual
    
    
    /* Ahora volvemos a hacer lo mismo, pero esta vez corremos todas las llaves que se encuentren antes
     *  de la posicion (o en la posicion)  'i'  hacia la derecha*/
    for (int j = number_keys - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    
    //Copiamos la llave media del nodo 'y' al nodo actual. la llave media siempre se encuentra en la posicion [grado-1]ya que
    //el maximo de llaves permitidas es de 2*grado-1 y si estamos partiendo este nodo entonces sabemos que esa es la cantidad
    //llaves que 'tiene' (En este momento de ejecucion 'y' solo tiene la primera mitad del total maximo de llaves, la segunda 
    //mitad la tiene 'z'). Lo que hace esta instruccion es subir el puntero (que apunta al valor medio del nodo que partimos)
    //al arreglo de llaves del nodo actual. Hay que recordar de que 'y' es hijo del nodo actual
    keys[i] = y->keys[degree - 1];

    //Incrementamos la cantidad de llaves en este nodo 
    number_keys = number_keys + 1;
}

/* Retorna el indice de la primera llave que sea >=k. La busqueda de esta llave se realiza en el arreglo 
 * de llaves que tiene el nodo mediante el cual se invoca este metodo */
int BTreeNode::findKey(int k) {
    int idx = 0;
    while (idx < number_keys && keys[idx] < k)
        ++idx;
    return idx;
}


/* Elimina la llave k del subarbol arraigado en este nodo */
void BTreeNode::remove(int k) {
    
    //encontramos el indice que ocupa la llave 'k' en el arreglo de llaves del nodo actual
    int idx = findKey(k);

    //Se comprueba que la llave a eliminar sea valida y que efectivamente apunta al valor de k
    if (idx < number_keys && keys[idx] == k) {

        //Si el nodo es hoja - se llama removeFromLeaf, si no, se llama removeFromNonLeaf 
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
        
    } else {

        //Si es hoja, la llave no esta en este arbol
        if (leaf) {
            cout << "The key " << k << " does not exist in the tree\n";
            return;
        }

        /* La llave que se eliminara esta en un subarbol arraigado en este nodo. Esta bandera
         indica si la llave esta en el subarbol arraigado en el ultimo hijo de este nodo*/
        bool flag = ((idx == number_keys) ? true : false);

        // Si el hijo donde se supone que esta la llave tiene llaves de grado menor, llenamos ese hijo
        if (children[idx]->number_keys < degree)
            fill(idx);

        /* Si el ultimo hijo se ha unido, se debio haber unido con el hijo previo y asi hacemos recursion con el  hijo en idx-1
         Si no, se realiza recursion con el hijo en idx que ahora debe tener por lo menos una cantidad de llaves igual a grado*/
        if (flag && idx > number_keys)
            children[idx - 1]->remove(k);
        else
            children[idx]->remove(k);
    }
    return;
}

//Elimina la llave idx de este nodo, que es hoja

void BTreeNode::removeFromLeaf(int idx) {

    //Mueve todas las llaves despues de la idx posicion, un lugar para atras
    for (int i = idx + 1; i < number_keys; ++i)
        keys[i - 1] = keys[i];

    // Reduce la cuenta de llaves
    number_keys--;

    return;
}

//Elimina la llave idx de este nodo, que NO es hoja

void BTreeNode::removeFromNonLeaf(int idx) {

    int k = keys[idx];

    /* Si el hijo que precede k (hijo[idx] tiene por lo menos llaves de grado,
     encuentra el predecesor 'pred' de k en el subarbol que esta arraigado en el hijo[idx].
     Reemplaza k con pred. Recursivamente elimina pred en hijo [idx]*/
    if (children[idx]->number_keys >= degree) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    }
    /* Si el hijo[idx] tiene menos llaves de grado, examina hijo[idx+1]. Si este tiene por lo menos
     hijos de grado, encontrar el sucesor 'succ' de k en el subarbol arraigo en hijo [idx+1]*/
    else if (children[idx + 1]->number_keys >= degree) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    }
    /* Si ambos hijos[idx] y [idx+1] tienen menos llaves de grado, unir k y todos los hijos [idx+1] con hijos[idx]. 
     Ahora hijos[idx] contienen 2t-1 llaves. Liberar hijos[idx+1] y eliminar recursivamente k de hijos[idx] */
    else {
        merge(idx);
        children[idx]->remove(k);
    }
    return;
}

//Obtener el predecesor de llaves[idx]

int BTreeNode::getPred(int idx) {
    //Moverse al nodo mas derecho hasta llegar a una hoja
    BTreeNode *cur = children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->number_keys];

    //Retorna la ultima llave de la hoja
    return cur->keys[cur->number_keys - 1];
}

int BTreeNode::getSucc(int idx) {

    //Moverse al nodo mas izquierdo, empezando de hijos[idx+1] hasta llegar a una hoja
    BTreeNode *cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];

    //Retorna la primera llave de la hoja
    return cur->keys[0];
}


/* Este metodo el hijo, lo llena con hijos[idx] que tienen una cantidad menor de grado-1 llaves */
void BTreeNode::fill(int idx) {

    //Si el hijo previo(hijos[idx-1]) tiene mas de grado-1 llaves, pedimos una llave de ese hijo
    if (idx != 0 && children[idx - 1]->number_keys >= degree)
        borrowFromPrev(idx);

    //Si el proximo hijo (hijos[idx+1]) tiene mas de grado-1 grados, prestarse una llave de ese hijo
    else if (idx != number_keys && children[idx + 1]->number_keys >= degree)
        borrowFromNext(idx);

    /* Unir hijos[idx] con este hermano. Si hijos[idx] es el ultimo hijo, unirlo con el hermano previo.
     Si no, unirlo con el proximo hermano*/
    else {
        if (idx != number_keys)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

/* Se toma una llave de hijo[idx-1] y se inserta en hijo    [idx] */
void BTreeNode::borrowFromPrev(int idx) {

    BTreeNode *child = children[idx];               //puntero que apunta al hijo presente en idx
    BTreeNode *sibling = children[idx - 1];     //puntero que apunta al hijo presente en idx-1, que seria hermano de 'child'

    //La ultima llave de hijos[idx-1] va hasta el padre y llave[idx-1] del padre se inserta
    //como la primera llave en hijos[idx]. Por ello, el hermano pierde una llave y el hijo gana una.
    
    //Movemos todas las llaves de hijo[idx] un paso adelante
    for (int i = child->number_keys - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    //Si hijo[idx] no es una hoja, mover todos los punteros de hijos un paso adelante
    if (!child->leaf) {
        for (int i = child->number_keys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    //Settear primera llave  del hijo 'child' igual a llaves[idx-1] del nodo actual
    child->keys[0] = keys[idx - 1];

    //Si el nodo actual no es una hoja entonces movemos el ultimo hijo del hermano como el primer hijo de hijo[idx]
    if (!leaf)
        child->children[0] = sibling->children[sibling->number_keys];

    //Mover la llave del hermano al padre. Esto reduce las llaves del hermano
    keys[idx - 1] = sibling->keys[sibling->number_keys - 1];

    child->number_keys += 1;
    sibling->number_keys -= 1;

    return;
}

//Se presta una llave de hijos[idx+1] y lo pone en hijos[idx]

void BTreeNode::borrowFromNext(int idx) {

    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx + 1];

    //llaves[idx] se inserta en la ultima llave de hijos[idx]
    child->keys[(child->number_keys)] = keys[idx];

    //El primer hijo del hermano se inserta en el ultimo hijo de hijos[idx]
    if (!(child->leaf))
        child->children[(child->number_keys) + 1] = sibling->children[0];

    //La primera llave del hermano se inserta en llaves[idx]
    keys[idx] = sibling->keys[0];

    //Mover todas la llaves en el hermano un paso atras
    for (int i = 1; i < sibling->number_keys; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    //Mover los punteros de los hijos un paso atras
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->number_keys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    //Incrementar y reducir la cantidad de llaves de hijos[idx] e hijos[idx+1] respectivamente
    child->number_keys += 1;
    sibling->number_keys -= 1;

    return;
}


//Une hijos[idx] con hijos[idx+1]. Hijos[idx+1] se libera despues de unirse

void BTreeNode::merge(int idx) {
    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx + 1];

    //Sacar una llave del nodo presente e insertarla la posicion(grado-1) de hijos[idx]
    child->keys[degree - 1] = keys[idx];

    //Copiar las llaves de hijos[idx+1] a hijos[idx] al final
    for (int i = 0; i < sibling->number_keys; ++i)
        child->keys[i + degree] = sibling->keys[i];

    //Copiar los punters hijos de hijos[idx+1] a hijos[idx]
    if (!child->leaf) {
        for (int i = 0; i <= sibling->number_keys; ++i)
            child->children[i + degree] = sibling->children[i];
    }

    //Mover todas las llaves despues de idx en el nodo actual un paso antes
    //para llenar el espacio creado por mover llaves[idx] a hijos[idx]
    for (int i = idx + 1; i < number_keys; ++i)
        keys[i - 1] = keys[i];

    //Mover los punteros de los hijos despues de idx+1 en el nodo presente un paso atras
    for (int i = idx + 2; i <= number_keys; ++i)
        children[i - 1] = children[i];

    //Actualizar la cantidad de llaves de hijos y el nodo actual
    child->number_keys += sibling->number_keys + 1;
    number_keys--;

    //Liberar la memoria ocupado por el hermano
    delete(sibling);
    return;
}

void BigTree::remove(int k) {
    if (!root) {
        cout << "El arbol esta vacio/no hay llaves;
        return;
    }

    //Llama la funcion para eliminar en raiz
    root->remove(k);

    //Si el nodo raiz tiene 0 llaves, hacer su primer hijo como nueva raiz si tiene un hijo. Si no, settear raiz como NULL
    if (root->number_keys == 0) {
        BTreeNode *tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->children[0];

        //Liberar la raiz vieja
        delete tmp;
    }
    return;
}

void BigTree::BigMenu() {
    using namespace std;
    BigTree  bigT (3);  //Creater B-Tree grado 3

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
                            bigT.traverse(); //Desplegar el arbol en modo de atravesado
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
