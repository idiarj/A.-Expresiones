#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Estructura de un nodo del árbol
typedef struct Nodo {
    char operador;
    int valor;
    char variable;
    struct Nodo *izq, *der;
} Nodo;

Nodo* evaluarSumaResta(const char **ptr); // Declaración de la función

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

int isVariable(const char* c) {
    return isalpha(*c);
}

Nodo* obtenerVariable(const char **ptr) {
    saltarEspacios(ptr);
    char variable[100];
    int i = 0;
    while (isalpha(**ptr) || isdigit(**ptr)) {
        variable[i++] = **ptr;
        (*ptr)++;
    }
    variable[i] = '\0';
    int valor;
    printf("Ingrese el valor de la variable %s: ", variable);
    scanf("%d", &valor);
    Nodo* nodo = nuevoNodoNumero(valor);
    nodo->variable = variable[0]; 
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
    if (isVariable(*ptr)) {
        return obtenerVariable(ptr);
    }
    return obtenerNumero(ptr);
}

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
    char expresion[256];
    FILE *archivo = fopen("ecuacion.txt", "r");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return 1;
    }

    if (fgets(expresion, sizeof(expresion), archivo) != NULL) {
        Nodo* raiz = construirArbol(expresion);

        //printf("Árbol de expresión:\n");
        //imprimirArbol(raiz, 0);

        printf("\nResultado: %d\n", evaluarArbol(raiz));

        liberarArbol(raiz);
    } else {
        printf("El archivo está vacío o no se pudo leer la expresión.\n");
    }

    fclose(archivo);
    return 0;
}
