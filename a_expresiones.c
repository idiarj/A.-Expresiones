#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Estructura de un nodo del árbol
typedef struct Nodo {
    char operador;
    int valor;
    struct Nodo *izq, *der;
} Nodo;


Nodo* nuevoNodoOperador(char operador, Nodo* izq, Nodo* der) {
    Nodo* nodo = (Nodo*)malloc(sizeof(Nodo));
    nodo->operador = operador;
    nodo->valor = 0;
    nodo->izq = izq;
    nodo->der = der;
    return nodo;
}


Nodo* nuevoNodoNumero(int valor) {
    Nodo* nodo = (Nodo*)malloc(sizeof(Nodo));
    nodo->operador = '\0';
    nodo->valor = valor;
    nodo->izq = nodo->der = NULL;
    return nodo;
}


void saltarEspacios(const char **ptr) {
    while (**ptr == ' ') (*ptr)++;
}


Nodo* obtenerNumero(const char **ptr) {
    saltarEspacios(ptr);
    int valor = 0;
    while (isdigit(**ptr)) {
        valor = valor * 10 + (**ptr - '0');
        (*ptr)++;
    }
    return nuevoNodoNumero(valor);
}


Nodo* evaluarFactor(const char **ptr);


Nodo* evaluarMultiplicacionDivision(const char **ptr) {
    Nodo* nodo = evaluarFactor(ptr);
    while (1) {
        saltarEspacios(ptr);
        if (**ptr == '*' || **ptr == '/') {
            char operador = **ptr;
            (*ptr)++;
            Nodo* der = evaluarFactor(ptr);
            nodo = nuevoNodoOperador(operador, nodo, der);
        } else {
            break;
        }
    }
    return nodo;
}


Nodo* evaluarSumaResta(const char **ptr) {
    Nodo* nodo = evaluarMultiplicacionDivision(ptr);
    while (1) {
        saltarEspacios(ptr);
        if (**ptr == '+' || **ptr == '-') {
            char operador = **ptr;
            (*ptr)++;
            Nodo* der = evaluarMultiplicacionDivision(ptr);
            nodo = nuevoNodoOperador(operador, nodo, der);
        } else {
            break;
        }
    }
    return nodo;
}


Nodo* evaluarFactor(const char **ptr) {
    saltarEspacios(ptr);
    if (**ptr == '(') {  
        (*ptr)++; 
        Nodo* nodo = evaluarSumaResta(ptr);
        (*ptr)++; 
        return nodo;
    }
    return obtenerNumero(ptr);
}


Nodo* construirArbol(const char *expresion) {
    return evaluarSumaResta(&expresion);
}


int evaluarArbol(Nodo* raiz) {
    if (!raiz) return 0;
    if (raiz->operador == '\0') return raiz->valor;  

    int izq = evaluarArbol(raiz->izq);
    int der = evaluarArbol(raiz->der);

    switch (raiz->operador) {
        case '+': return izq + der;
        case '-': return izq - der;
        case '*': return izq * der;
        case '/': return izq / der;
    }
    return 0;
}


// void imprimirArbol(Nodo* nodo, int nivel) {
//     if (!nodo) return;
//     imprimirArbol(nodo->der, nivel + 1);
//     for (int i = 0; i < nivel; i++) printf("   ");
//     if (nodo->operador)
//         printf("%c\n", nodo->operador);
//     else
//         printf("%d\n", nodo->valor);
//     imprimirArbol(nodo->izq, nivel + 1);
// }


void liberarArbol(Nodo* nodo) {
    if (!nodo) return;
    liberarArbol(nodo->izq);
    liberarArbol(nodo->der);
    free(nodo);
}


int main() {
    const char expresion[] = "50/(5+5)*(6-2)-3";
    Nodo* raiz = construirArbol(expresion);

    //printf("Árbol de expresión:\n");
    //imprimirArbol(raiz, 0);

    printf("\nResultado: %d\n", evaluarArbol(raiz));

    liberarArbol(raiz);
    return 0;
}
