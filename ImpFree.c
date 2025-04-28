#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct node{
  char data;
  struct node* next;
  struct node* prev;
  struct node* past;
} node;

int isAtom (char c);
int isConect (char c);
void menu (char arr[]);
void display (node* front);
node* searchRear (node* front);
void insertEnd (node** front, char new_data, node* last_node, int verif);
int empty (node* front);
node* arrNode (char arr[]);
node* atomP (node* front);
node* negP (node* front);
node* conectP (node* front);
void freeHistory(node* front);
void freeList(node* front);
int currentImp (node* temp);
node* impFree (node* old_front, int* m);
node* createListEquiv (node* m_front);
node* createListCopy (node* front, int* copy_atoms);
void displayQueue (node* front);
int currentAtoms (node* m_front);
int contieneP(node* front);

int main (){
  int m = 0;
  node* front = NULL;
  node* temp = NULL;
  char arr [100];
  menu (arr);
  front = arrNode (arr);
  node* front_initial = front;
  temp = front;
  front = atomP (temp);
  temp = front;

  do {
    node* current_list = temp;
      // aplicar negP
    node* new_neg = negP (temp);
    if (new_neg != NULL){
      temp = new_neg;
    }

    node* new_conect = conectP (temp);
    if (new_conect != NULL){
      temp = new_conect;
    }

    if (new_neg == NULL && new_conect == NULL){
      break;
    }
  }
  while (1);
  display(temp);
  // Verificación final en main():
    if (temp->data == 'P' && temp->next == NULL) {
        printf("¡Fórmula Bien Formada!\n\n");
    } else {
        printf("No es una Fórmula Bien Formada.\n");
        exit (1);
    }
  /* Inicio de impFree*/
  printf ("-----------------------------------------------------\n");
  printf ("impFree\n");
  node* new_front = impFree (temp, &m);
  printf("\033[1;33m\nAntes de impFree:\n \033[0m");
  displayQueue (front_initial);
  printf("\033[1;35m\nDespues de impFree:\n \033[0m");
  displayQueue (new_front);

  /* Liberar todos los nodos*/
  freeHistory (temp);
  free (new_front);

  return 0;
}

node* getNode (void){
  node* p;
  p = (node*)(malloc(sizeof(node)));
  p -> next = NULL;
  p -> prev = NULL;
  p -> past = NULL;
  return p;
}

node* arrNode (char arr[]){
  node* front = NULL;
  int n = strlen (arr);
  for (int i = 0; i < n; i++){
    // Le paso NULL pq con el 0 no hara nada
    insertEnd (&front, arr [i], NULL, 1);
  }
  return front;
}

void menu (char arr[]){
  system ("clear");
  printf("Ingresa tu fórmula para verificar: \n");
  scanf ("%s", arr);
  int n = strlen (arr);
  for (int i = 0; i < n; i++){
    int validez = 1;
    if (isAtom(arr[i]))
      validez = 0;
    if (isConect(arr[i]))
      validez = 0;
    if ((arr[i]) == '(')
      validez = 0;
    if ((arr[i]) == ')')
      validez = 0;
    if ((arr[i]) == '~')
      validez = 0;
    if ((validez) == 1){
      printf ("Ingresa caracteres validos\n");
      exit (1);
    }
  }
}

int isAtom (char c){
  return (c >= 'p' && c <= 'z');
}

int isConect (char c){
  return ( c == '*' || c == '+' || c == '-');
}

node* searchRear (node* front){
  if (empty(front)){
    return NULL;
  }
  node* temp = front;
  while (temp -> next != NULL){
    temp = temp -> next;
  }
  return temp;
}

void insertEnd (node** front, char new_data, node* last_node, int verif){
  node* q = getNode();
  q -> data = new_data;
  if (empty(*front)){
    *front = q;
  }
  else {
  node* rear = searchRear (*front);
    rear -> next = q;
    q -> prev = rear;
  }
  if (verif == 1)
    q -> past = last_node; 
}

int empty (node* front){
  return front == NULL;
}

node* atomP (node* front){
  node* new_front = NULL;
  node* temp = front;
  while (temp != NULL){
    if (isAtom(temp -> data)){
      insertEnd(&new_front, 'P', temp, 1);
    } else {
      insertEnd(&new_front, temp -> data, NULL, 1);
    }
    temp = temp -> next;
  }
  if (new_front != NULL) {
    new_front->past = front;
  }
  return new_front; // Recibe el front anterior, genera un new_front y devuelve ese new_front
}

void display (node* front){
  int i = 0;
  int j = 0;
  node* temp_p = front;
  printf ("\n");
  while (temp_p != NULL) {
      node* temp_f = temp_p;
      while (temp_f != NULL) {
          printf("%c", temp_f->data);
          if (temp_f -> data == 'P' && temp_f -> past != NULL)
            i++;
          if (temp_f -> data == '(' && temp_f -> past != NULL)
            j++;
          temp_f = temp_f -> next;
      }
  temp_p = temp_p -> past;
  printf("\n");
  }
  //printf ("\n nodos P con past: %d\n", i);
  //printf ("\n nodos ( con past: %d\n", j);
}

node* negP(node* front) {
    node* new_front = NULL;
    node* temp = front;
    int changes_made = 0; // Bandera para rastrear cambios

    while (temp != NULL) {
        if (temp->data == '(') {
            node* temp_p = temp;
            node* checker = temp;
            char negation[4];
            int i = 0;

            // Verificar patrón (~P)
            while (i < 4 && checker != NULL) {
                negation[i] = checker->data;
                checker = checker->next;
                i++;
            }

            if (i == 4 &&
                negation[0] == '(' &&
                negation[1] == '~' &&
                negation[2] == 'P' &&
                negation[3] == ')') {
                
                insertEnd(&new_front, 'P', temp_p, 1);
                temp = checker; // Avanzar temp al final del patrón
                changes_made = 1; // ¡Hubo un cambio!
                continue;
            }
        }
        // Si no se aplicó el patrón, copiar el nodo actual
        if (temp -> past && (isAtom(temp -> past -> data))){
          insertEnd(&new_front, temp->data, temp -> past, 1);
        }
        else {
          insertEnd(&new_front, temp->data, temp, 1);
        }

        temp = temp->next;
    }

    if (!changes_made) {
        freeList(new_front); // Liberar lista no necesaria
        return NULL; // Indica que no hubo cambios
    }

  if (new_front != NULL)
    new_front->past = front; // Vincular al historial
  //
    return new_front;
}

node* conectP(node* front) {
    node* new_front = NULL;
    node* temp = front;
    int changes_made = 0; // Bandera para rastrear cambios

    while (temp != NULL) {
        if (temp->data == '(') {
            node* temp_p = temp;
            node* checker = temp;
            char pattern[5];
            int i = 0;

            // Verificar patrón (P*P)
            while (i < 5 && checker != NULL) {
                pattern[i] = checker->data;
                checker = checker->next;
                i++;
            }

            if (i == 5 &&
                pattern[0] == '(' &&
                pattern[1] == 'P' &&
                isConect(pattern[2]) &&
                pattern[3] == 'P' &&
                pattern[4] == ')') {
                
                if (!(temp_p)) {
                    printf("⚠️  ERROR: Se está insertando una P sin 'past' en la generación inicial\n");
                }
                insertEnd(&new_front, 'P', temp_p, 1);
                temp = checker; // Avanzar temp al final del patrón
                changes_made = 1; // ¡Hubo un cambio!
                continue;
            }
        }

        // Si no se aplicó el patrón, copiar el nodo actual
        if (temp -> past && (isAtom(temp -> past -> data))){
          insertEnd(&new_front, temp->data, temp -> past, 1);
        }
        else {
          insertEnd(&new_front, temp->data, temp, 1);
        }
        temp = temp->next;
    }

    if (!changes_made) {
        freeList(new_front); // Liberar lista no necesaria
        return NULL; // Indica que no hubo cambios
    }

  if (new_front != NULL)
    new_front->past = front; // Vincular al historial
  //
    return new_front;
}

void freeList(node* front) {
  while (front != NULL) {
    node* next = front->next;
    free(front);
    front = next;
  }
}

void freeHistory(node* front) {
  if (front == NULL)
    return;
  // Primero libera el historial (listas previas) si no se han liberado aún.
  if (front->past != NULL) {
    freeHistory(front->past);
    front->past = NULL;  // Para evitar doble liberación.
    }
  // Luego libera la lista actual.
  freeList(front);
}

int currentImp (node* temp){
  int x = 0;
  while (temp != NULL){
    if (temp -> data == '-'){
      x = 1;
      break;
    }
    temp = temp -> next;
  }
  //printf ("\ncurrentImp: %d\n", x);
  return x;
}

node* impFree (node* old_front, int* m){ // Recibe el ultimo front (P) de la red de validacion
  // *equivalencia: (P-P) a ((~P)+P) 
  int iteraciones = 0;
  int copy_atoms = 0; // Indica si la lista creada ya es la última de la red
  node* m_front = old_front -> past; // front dinamico
  node* new_front = NULL;
  node* temp;

  while (m_front != NULL){
    // 1⨪ Buscar si existen implicaciones
    if (currentImp (m_front)){ // Si existe una impl entonces:
      temp = m_front;
      new_front = createListEquiv (m_front);
    }
    else {
      // Si no hay implicaciones copiar la lista anterior del historial
      new_front = createListCopy (m_front, &copy_atoms);
      //printf("\nDespues de createListCopy\n");
      //displayQueue (new_front);
    }

    if (!(contieneP(new_front))){
      //printf ("Ya no hay 'P'\n");
      break;
    }
      m_front = new_front;

    if (*m > 20){
      printf ("\n lim iter\n");
      exit (1);
    }
    temp = temp -> next;
  }

  //printf ("\nLista final: ");
  //displayQueue (new_front);
  return new_front; // retorna el front de la lista creada que ya contiene los átomos
}

node* createListEquiv(node* m_front){
  node* new_front = NULL;
  node* temp = m_front;

  while (temp != NULL){
    if (temp->data == 'P') {
        if (!(temp->past)) {
            printf("⚠️  WARNING: Copiando 'P' sin past en createListCopy\n");
        }
    }
    if ( temp->data == '('
      && temp->next
      && temp->next->data == 'P'
      && temp->next->next
      && temp->next->next->data == '-'
      && temp->next->next->next
      && temp->next->next->next->data == 'P'
      && temp->next->next->next->next
      && temp->next->next->next->next->data == ')')
    {
      // Capturamos directamente los dos nodos P
      node* p1 = temp->next;
      node* p2 = temp->next->next->next;
      node* past1 = p1->past;   // puede ser NULL, conviene comprobar luego
      node* past2 = p2->past;
      if (!past2) {
          printf("ADVERTENCIA: 'P' derecha sin representación en (P-P)\n");
      }

      // Insertar la equivalencia "((~P)+P)"
      insertEnd(&new_front, '(', NULL, 1);
      insertEnd(&new_front, '(', NULL, 1);
      insertEnd(&new_front, '~', NULL, 1);

      // Primera P con su past
      insertEnd(&new_front, 'P', past1, 1);
      insertEnd(&new_front, ')', NULL, 1);

      insertEnd(&new_front, '+', NULL, 1);

      // Segunda P con su past
      insertEnd(&new_front, 'P', past2, 1);
      insertEnd(&new_front, ')', NULL, 1);

      // Saltamos la parte original "(P-P)"
      temp = temp->next->next->next->next->next;
      continue;
    }

    // Si no hay implicación, copia el símbolo tal cual
    insertEnd(&new_front, temp->data, temp->past, 1);
    temp = temp->next;
  }

  return new_front;
}

node* createListCopy(node* front, int* copy_atoms) {
  node* new_front = NULL;
  node* temp = front;

  while (temp != NULL) {
    if (temp->data == 'P') {
      node* current = temp->past;
      //printf("\nCopiando 'P', inicio buscando desde past %p\n", (void*)current);

      // Corrección: Subir en la cadena de 'past' si algo no encaja
      while (current != NULL) {
        if (isAtom(current->data)) {
          // Encontramos un átomo
          insertEnd(&new_front, current->data, current->past, 1);
       //   printf("Insertado átomo desde 'P': %c\n", current->data);
          *copy_atoms = 1;
          break;
        }
        else if (current->data == '(') {
          node* checker = current;

          if (checker->next &&
            checker->next->data == '~' &&
            checker->next->next &&
            checker->next->next->data == 'P' &&
            checker->next->next->next &&
            checker->next->next->next->data == ')') {
            // Es una negación (~P)
            insertEnd(&new_front, '(', checker->past, 1);
            insertEnd(&new_front, '~', checker->next->past, 1);
            insertEnd(&new_front, 'P', checker->next->next->past, 1);
            insertEnd(&new_front, ')', checker->next->next->next->past, 1);
            //printf("Insertado patrón (~P)\n");
            break;
          }
          else if (checker->next &&
                 checker->next->data == 'P' &&
                 checker->next->next &&
                 isConect(checker->next->next->data) &&
                 checker->next->next->next &&
                 checker->next->next->next->data == 'P' &&
                 checker->next->next->next->next &&
                 checker->next->next->next->next->data == ')') {
            // Es un conectivo (P*P)
            insertEnd(&new_front, '(', checker->past, 1);
            insertEnd(&new_front, 'P', checker->next->past, 1);
            insertEnd(&new_front, checker->next->next->data, checker->next->next->past, 1);
            insertEnd(&new_front, 'P', checker->next->next->next->past, 1);
            insertEnd(&new_front, ')', checker->next->next->next->next->past, 1);
            //printf("Insertado patrón (P*P)\n");
            break;
          }
          else {
            // No es un patrón conocido, seguir subiendo
            //printf("Patrón inválido, subiendo en past...\n");
            current = current->past;
          }
        }
        else {
          // No es átomo ni paréntesis, seguir subiendo
          //printf("Nodo inesperado %c, subiendo en past...\n", current->data);
          current = current->past;
        }
      }

      if (current == NULL) {
        fprintf(stderr, "⚠️ Advertencia: 'P' sin representación válida encontrada.\n");
      }
    }
    else if (isAtom(temp->data)) {
      insertEnd(&new_front, temp->data, temp->past, 1);
      //printf("Insertado átomo directo: %c\n", temp->data);
    }
    else {
      insertEnd(&new_front, temp->data, temp->past, 1);
      //printf("Insertado símbolo: %c\n", temp->data);
    }

    temp = temp->next;
  }

    return new_front;
}

void displayQueue (node* front){
 // sleep (2);
  //system ("clear");
  node* actual = front;
  printf("\n  ");
    while (actual != NULL) {
        printf("%c", actual->data);
        //printf("%d -> ", actual->info);
        actual = actual->next;
    }
  printf ("\n");
}

int currentAtoms (node* m_front){
  node* temp = m_front;

  while (temp != NULL){
    return (isAtom(temp -> data));
    temp = temp -> next;
  }
}

int contieneP(node* front) {
    node* temp = front;
    while (temp != NULL) {
        if (temp->data == 'P') return 1;
        temp = temp->next;
    }
    return 0;
}

