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

Nodo* evaluarSumaResta(const char **ptr);

int validarParentesis(const char* expresion) {
    int contador = 0;
    int posUltimoAbierto = -1;
    
    for(int i = 0; expresion[i] != '\0'; i++) {
        if(expresion[i] == '(') {
            contador++;
            posUltimoAbierto = i;
        }
        else if(expresion[i] == ')') {
            if(contador <= 0) return 0;  
            if(i == posUltimoAbierto + 1) return 0;  
            contador--;
        }
    }
    return contador == 0;  
}

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

    if(raiz->operador == '/'){
        if(der == 0){
            printf("Error: División por cero\n");
            exit(1);
        }
    }

    switch (raiz->operador) {
        case '+': return izq + der;
        case '-': return izq - der;
        case '*': return izq * der;
        case '/': return izq / der;
    }
    return 0;
}

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
        if (!validarParentesis(expresion)) {
            printf("Error: Los parentesis en la expresion no son validos\n");
            fclose(archivo);
            return 1;
        }

        Nodo* raiz = construirArbol(expresion);
        printf("\nResultado: %d\n", evaluarArbol(raiz));
        liberarArbol(raiz);
    } else {
        printf("El archivo está vacío o no se pudo leer la expresión.\n");
    }

    fclose(archivo);
    return 0;
}