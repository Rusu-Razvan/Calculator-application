// Eval.cpp : This file contains the 'main' function. Program execution begins and ends there.//
#include <iostream>
#include "graphics.h"
#include <math.h>
#include <cmath>
#pragma comment(lib,"graphics.lib")
#include "winbgim.h"
#include <cstring>
#define infinit INT_MAX
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#define epsi 0.0001
#define  MAX 100
#define MAX1 20
#include <stdio.h>
#include <Windows.h>
using namespace std;
char formula[256][256], propozitie[256], numar[256], functii[8][256] = { "sin", "cos", "tg", "ctg", "ln", "exp", "rad", "abs" }, copie[256], valoriVariabile[256][256], stiva[256][256], operatori[256][256];
int ok = 1, adevarat = 1, j, nrcifre, lungime_formula, pozitie_greseala[256], nrgreseli, paranteza, tipuriGreseli[101], esteFunctie, i, tipuri[256], pozitia, tip, valoare, totalVariabile, totals, totalo;
int pozVecCar = 0, length = 1920,height = 1080;
double valori[1000];
const int MAX_LUNGIME_PROPOZITIE = 100;
int xMouse,yMouse,esteCasutaTextActiva = 0;
char variabile[256][256];
int top1, top2; // varfurile celor doua stive
const int max_stiva = 100;

double Opd[max_stiva]; // stiva operanzilor
char Op[max_stiva][max_stiva]; // stiva operatorilor

char OperatiiBinare[200] = "+-*/^<>=#";
char OperatiiUnare[200] = "scarel";
char Operatii[200] = "+-*/^<>=#scarel";
const float pi = 3.1415926; // se da sub forma literei q

struct coordonateCaractere
{
    int x;
    int y;

} caracter[256];

struct coordonateTextVariabile
{
    int x;
    int y;
    int cursorVariabia=0;
    int casutaVarActiva=0;
    int primitValoare = 0;
    int aFostScrisaCifra = 0;
    int aparitiePunct = 0;
} casutaText[256];

struct nodArbore
{
    nodArbore* stg;
    nodArbore* drp;
    char data[256];
};

int nrNiveluri(nodArbore *a)
{
    if(a==NULL)return 0;
    else
    {
        int n1=nrNiveluri(a->stg);
        int n2=nrNiveluri(a->drp);
        return 1+max(n1,n2);
    }
}
int nrColoane(nodArbore *a)
{
    if(a==NULL)return 0;
    else
    {
        int n1=nrColoane(a->stg);
        int n2=nrColoane(a->drp);
        return 1+n1+n2;
    }
}

void deseneazaNod(char elem[256], int xc, int yc, int latime, int inaltime)
{
    char arr[256];
    strcpy(arr, elem);
    setcolor(WHITE);
    setfillstyle(SOLID_FILL,LIGHTCYAN);
    fillellipse(xc,yc,textwidth(arr)+2, textheight("M")/2+6);
    settextstyle(SANS_SERIF_FONT,HORIZ_DIR,1);
    setbkcolor(LIGHTCYAN);
    setcolor(BLUE);
    settextjustify(1,1);
    outtextxy(xc,yc+4,arr);
}

void deseneazaArbore(nodArbore *a, int niv, int stanga, int latime, int inaltime)
{
    if(a!=NULL)
    {
        int n1=nrColoane(a->stg);
        int xc=stanga+latime*n1+latime/2;
        int yc=niv*inaltime-inaltime/2;

        if (a->stg!=NULL)
        {
            int xcs=stanga+latime*nrColoane(a->stg->stg)+latime/2;
            setcolor(WHITE);
            line(xc,yc,xcs,yc+inaltime);
        }
        if (a->drp!=NULL)
        {
            int xcd=stanga+latime*(n1+1)+latime*nrColoane(a->drp->stg)+latime/2;
            setcolor(WHITE);
            line(xc,yc,xcd,yc+inaltime);
        }
        deseneazaNod(a->data,xc,yc,latime, inaltime);
        deseneazaArbore(a->stg,niv+1,stanga, latime, inaltime);
        deseneazaArbore(a->drp,niv+1,stanga+latime*(n1+1), latime, inaltime);
    }
}

// Facem un nod nou
nodArbore* createNode(char data[256])
{
    nodArbore* newNode = new nodArbore;
    strcpy(newNode->data, data);
    newNode->stg = NULL;
    newNode->drp = NULL;
    return newNode;
}
bool isFunction(char str[])
{
    if (strcmp("sin", str) == 0)
        return 1;
    if (strcmp("cos", str) == 0)
        return 1;
    if (strcmp("ln", str) == 0)
        return 1;
    if (strcmp("exp", str) == 0)
        return 1;
    if (strcmp("abs", str) == 0)
        return 1;
    if (strcmp("rad", str) == 0)
        return 1;
    return 0;

}
// Functie de a crea un arbore dupa stiva postfixata
nodArbore* buildExpressionTree(const char stiva[256][256], int totals)
{
    // Stack to store nodes during tree construction
    nodArbore* stack[100];  // Adjust the size as needed
    int top = -1;

    for (int i = 1; i <= totals; ++i)
    {
        char symbol[256];
        strcpy(symbol, stiva[i]);
        // If the symbol is an operand, push a node with the operand onto the stack
        if (isFunction(symbol)) // daca este functie DE VERIFICAT CORECT DACA IS FUNCTII
        {
            nodArbore* leftOperand = stack[top--];
            char zero[] = "0";
            nodArbore* rightOperand = NULL;

            nodArbore* functionNode = createNode(symbol);
            functionNode->stg = leftOperand;
            functionNode->drp = rightOperand;

            stack[++top] = functionNode;

        }
        else if (isalnum(symbol[0]) || (symbol[0] == '-' && isdigit(symbol[1])))
        {
            nodArbore* operandNode = createNode(symbol);
            stack[++top] = operandNode;
        }

        else
        {
            // If the symbol is an operator, pop two operands from the stack
            nodArbore* rightOperand = stack[top--];
            nodArbore* leftOperand = stack[top--];

            // Create a new node with the operator and push it onto the stack
            nodArbore* operatorNode = createNode(symbol);
            operatorNode->stg = leftOperand;
            operatorNode->drp = rightOperand;
            stack[++top] = operatorNode;
        }
    }

    // The root of the expression tree is at the top of the stack
    return stack[top];
}

bool esteLitera(char propozitie[256], int i)
{
    return ((propozitie[i] >= 'a' && propozitie[i] <= 'z') || (propozitie[i] >= 'A' && propozitie[i] <= 'Z'));
}

bool esteCifra(char propozitie[256], int i)
{
    return (propozitie[i] >= '0' && propozitie[i] <= '9');
}

bool esteParantezaDeschisa(char propozitie[256], int i)
{
    return (propozitie[i] == '(');
}

bool esteParantezaInchisa(char propozitie[256], int i)
{
    return (propozitie[i] == ')');
}

bool esteSemn(char propozitie[256], int i)
{
    int ok = 0;
    if (strchr("+-*/^<>=#", propozitie[i]) != NULL)
        ok = 1;
    return ok;
}

void adaugaFunctie(char propozitie[256], int& i, int& esteFunctie)
{
    char copie[256] = "";
    int ok = 0;
    esteFunctie = 0;
    if (strchr("scare", propozitie[i]) != NULL)
    {
        strncpy(copie, propozitie + i, 3);
        i = i + 2;
        ok = 1;
    }

    else if (strchr("l", propozitie[i]) != NULL)
    {
        strncpy(copie, propozitie + i, 2);
        i = i + 1;
        ok = 1;
    }

    if (strstr("sincoslnexpabsradtgctg", copie) != NULL && ok == 1)
    {
        strcat(formula[lungime_formula++], copie);
        esteFunctie = 1;
    }
    if (esteFunctie == 0 && (strlen(copie) > 1))
    {
        if (copie[0] == 'l')
            i = i - 1;
        else i = i - 2;
    }
}

void adaugaSemn(char propozitie[256], int& i)
{
    if ((propozitie[i] == '>' || propozitie[i] == '<') && propozitie[i + 1] == '=')
    {
        strncat(formula[lungime_formula++], propozitie + i, 2);
        i = i + 1;
    }
    else
    {
        strncat(formula[lungime_formula++], propozitie + i, 1);
    }
}

void adaugaVariabila(char propozitie[256], int& i)
{
    int lungime_variabila = 0, copie_i = i;
    while (esteLitera(propozitie, copie_i))
    {
        lungime_variabila = lungime_variabila + 1;
        copie_i = copie_i + 1;
    }
    strncat(formula[lungime_formula++], propozitie + i, lungime_variabila);
    i = i + lungime_variabila - 1;
}

bool DifInf(float x)
{
    return fabs(infinit - fabs(x)) > infinit / 2.0;
}

float Logaritm(float x)
{
    if (x > epsi && DifInf(x))
        return log(x);
    else
        return infinit;
}

float Exponential(float x)
{
    if (DifInf(x)) return exp(x);
    else return infinit;
}

float Inmultit(float x, float y)
{
    if (fabs(x < epsi) || fabs(y) < epsi) return 0;
    else if (DifInf(x) && DifInf(y)) return x * y;
    else return infinit;
}

float Putere(float x, float y)
{
    // float p;
    if (x == 0) return 0;
    else if (y == 0) return 1;
    else if (x == infinit || y == infinit) return infinit;
    else
        return pow(x, y);
}

float Egal(float x, float y)
{
    return x == y;
}

float Diferit(float x, float y)
{
    return x != y;
}

float MaiMic(float x, float y)
{
    return x < y;
}

float MaiMicEgal(float x, float y)
{
    return x <= y;
}

bool MaiMare(float x, float y)
{
    return x > y;
}

bool MaiMareEgal(float x, float y)
{
    return x >= y;
}

float Plus(float x, float y)
{
    if (DifInf(x) && DifInf(y))  return x + y;
    else return infinit;
}

float Minus(float x, float y)
{
    if (DifInf(x) && DifInf(y))  return x - y;
    else return infinit;
}

float Impartit(float x, float y)
{
    if (fabs(y) > epsi) return x / y;
    else return infinit;
}

float Sinus(float x)
{
    if (DifInf(x))  return sin(x);
    else return infinit;
}

float Cosinus(float x)
{
    if (DifInf(x))  return cos(x);
    else return infinit;
}

float Modul(float x)
{
    if (DifInf(x)) return fabs(x);
    else return infinit;
}

float Radical(float x)
{
    if (DifInf(x) && (x > epsi)) return sqrt(x);
    else return infinit;
}

bool EsteNumar(char sir[MAX1])
{
    return (strchr("0123456789", sir[0]));
}

void adaugaParantezaDeschisa(char propozitie[256], int i, int& paranteza)
{
    paranteza++;
    formula[lungime_formula++][0] = propozitie[i];
}

void adaugaParantezaInchisa(char propozitie[256], int i, int& paranteza)
{
    paranteza--;
    formula[lungime_formula++][0] = propozitie[i];
}

void adaugaNumar(char propozitie[256], int& j)
{
    int nrcifre, adevarat = 1, i = j, nrAparitiiPunct = 0;
    char numar[256];

    nrcifre = 0;
    numar[0] = NULL;
    numar[nrcifre++] = propozitie[i];
    i++;
    while (esteCifra(propozitie, i) || propozitie[i] == '.')
    {
        if (propozitie[i] == '.')  //daca este numar real
        {
            nrAparitiiPunct++;
            if (nrAparitiiPunct > 1)
            {
                pozitie_greseala[++nrgreseli] = pozVecCar + 1;
                tipuriGreseli[nrgreseli] = 1;  //prea multe puncte
            }
            if (!esteCifra(propozitie, i + 1))
            {
                pozitie_greseala[++nrgreseli] = pozVecCar + 1;
                tipuriGreseli[nrgreseli] = 2; //dupa punct nu este scrisa o cifra
            }
        }
        numar[nrcifre++] = propozitie[i];
        i++;
    }
    numar[nrcifre] = '\0';
    strcat(formula[lungime_formula++], numar);
    j = j + nrcifre - 1;
}

int DenumireTip(char c[256])  // prioritate operatorilor
{
    if (c[0] == '(' || c[0] == ')')
        return 0;
    if (c[0] == '+' || c[0] == '-')
    {
        if (c[0] == '-' && isdigit(c[1])) // daca e nr negativ
            return 6;
        return 1;
    }
    if (c[0] == '*' || c[0] == '/')
        return 2;
    if (c[0] == '^')
        return 3;
    if (c[0] == '=' || c[0] == '#' || c[0] == '<' || c[0] == '>')
        return 4;
    if (c[0] == 'c' || c[0] == 's' || c[0] == 'l' || c[0] == 'e' || c[0] == 't' || c[0] == 'a' || c[0] == 'r')
        return 5;
    else if(esteLitera(c,0) || esteCifra(c,0))
        return 6; //pentru numere/variabile(x,y,etc)
}

void verificareValiditateSintactica() //verificam cat de corecta este formula
{
    int i;
    for (i = 0; i < lungime_formula; i++)
    {
        if (formula[i][0] == '(')
        {
            if (!(tipuri[i + 1] == 1 || tipuri[i + 1] == 5 || tipuri[i + 1] == 6 || formula[i + 1][0] == '('))
            {
                pozitie_greseala[++nrgreseli] = i + 1;
                tipuriGreseli[nrgreseli] = 4;
            }
        }
        else if (formula[i][0] == ')' || tipuri[i] == 6)
        {

            if (!(tipuri[i + 1] == 1 || tipuri[i + 1] == 2 || tipuri[i + 1] == 3 || tipuri[i + 1] == 4 || formula[i + 1][0] == ')' || i == lungime_formula - 1))
            {
                pozitie_greseala[++nrgreseli] = i + 1;
                tipuriGreseli[nrgreseli] = 4;
            }
        }
        else if (tipuri[i] == 1 || tipuri[i] == 2 || tipuri[i] == 3 || tipuri[i] == 4)
        {

            if (!(formula[i + 1][0] == '(' || tipuri[i + 1] == 5 || tipuri[i + 1] == 6))
            {
                pozitie_greseala[++nrgreseli] = i + 1;
                tipuriGreseli[nrgreseli] = 4;
            }
        }
        else
        {
            if (tipuri[i] == 5 && !(formula[i + 1][0] == '('))
            {
                pozitie_greseala[++nrgreseli] = i + 1;
                tipuriGreseli[nrgreseli] = 4;
            }
        }
    }
}

void enuntareGreseli(int i, int x, int y)
{
    switch(tipuriGreseli[i])
    {
    case 1:
        outtextxy(x, y,":Prea multe puncte folosite la scrierea numarului real");
        break;
    case 2:
        outtextxy(x, y,":Cifra lipsa dupa punct la scrierea numarului real");
        break;
    case 3:
        outtextxy(x, y,":Caracter invalid -> ");
        outtextxy(x + textwidth(":Caracter invalid -> "), y, formula[pozitie_greseala[i]-1] );
        break;
    case 4:
        outtextxy(x, y, ":Urmatorul caracter nu este plasat corect");
        break;
    case 5:
        outtextxy(x, y, ":Calcul imposibil");
        break;
    case 6:
        outtextxy(x, y, ":Parantezele nu se inchid corect!");
        break;
    }

}

double ValoareFunctie(char formula[256][256], int lungime_formula, char valoriVariabile[256][256])
{
    int i;
    int indiceVariabile = 1;
    // returneaza valoarea functiei E in punctul x
    double valo, x_1, x_2;
    //calculul expresiei
    for (i = 1; i <= max_stiva; i++)
    {
        Opd[i] = 0;
        Op[i][0] = '@';
    }
    top1 = 0;
    top2 = 1;
    Op[top2][0] = '(';
    i = 0;
    while (i <= lungime_formula && top2 > 0)
    {
        i++;
        if ((tipuri[i] == 6 && !esteCifra(formula[i],0)) && formula[i][0] != 'q' && formula[i][0] != '-')
        {
            top1++;
            valo = atof(valoriVariabile[indiceVariabile++]);
            Opd[top1] = valo;
        }
        else if (esteCifra(formula[i], 0) || (tipuri[i] == 6 && esteCifra(formula[i], 1)))
        {
            top1++;
            valo = atof(formula[i]);
            Opd[top1] = valo;
        }
        else
            switch (formula[i][0])
            {
            /* constanta pi=3.1415926 se da sub forma literei q */
            case 'q':
                top1++;
                Opd[top1] = pi;
                break;
            case '(': /* inceput de bloc */
                top2++;
                Op[top2][0] = '(';
                break;
            case ' ':
                top2++;
                Op[top2][0] = ' ';
                break;
            default:
                /* operatii binare si unare */
                while ((top2 > 0) && !(strchr("()", Op[top2][0])) &&
                        DenumireTip(Op[top2]) >= DenumireTip(formula[i]))
                {
                    if (top1 > 1) x_1 = Opd[top1 - 1];
                    x_2 = Opd[top1];
                    // depanare();
                    /* functii scrise in OPERATII */
                    if (Op[top2][0] == '=')
                        valo = Egal(x_1, x_2);
                    else if (Op[top2][0] == '#')
                        valo = Diferit(x_1, x_2);
                    else if (strcmp(Op[top2], "<=") == 0)
                        valo = MaiMicEgal(x_1, x_2);
                    else if (Op[top2][0] == '<')
                        valo = MaiMic(x_1, x_2);
                    else if (strcmp(Op[top2], ">=") == 0)
                        valo = MaiMareEgal(x_1, x_2);
                    else if (Op[top2][0] == '>')
                        valo = MaiMare(x_1, x_2);
                    else if (Op[top2][0] == '+')
                        valo = Plus(x_1, x_2);
                    else if (Op[top2][0] == '-')
                        valo = Minus(x_1, x_2);
                    else if (Op[top2][0] == '*')
                        valo = Inmultit(x_1, x_2);
                    else if (Op[top2][0] == '/')
                        valo = Impartit(x_1, x_2);
                    else if (Op[top2][0] == '^')
                        valo = Putere(x_1, x_2);
                    else if (Op[top2][0] == 's')
                        valo = Sinus(x_2);
                    else if (Op[top2][0] == 'c')
                        valo = Cosinus(x_2);
                    else if (Op[top2][0] == 'l')
                        valo = Logaritm(x_2);
                    else if (Op[top2][0] == 'e')
                        valo = Exponential(x_2);
                    else if (Op[top2][0] == 'a')
                        valo = Modul(x_2);
                    else if (Op[top2][0] == 'r')
                        valo = Radical(x_2);
                    //operator[top2]
                    if (strchr(OperatiiBinare, Op[top2][0])) top1--;
                    if (strchr(Operatii, Op[top2][0])) Opd[top1] = valo;
                    top2--;
                }
                if ((strcmp(Op[top2], "(") != 0) || (strcmp(formula[i], ")") != 0))
                {
                    top2++;
                    strcpy(Op[top2], formula[i]);
                }
                else top2--;

            }
    }
    if (top2 == 0) return Opd[1];
    else return infinit;
}

void deseneazaSageata(int x, int y, char text[256])
{

    line(x, y, x, y + 20);  //linia din mijloc

    if(textwidth(text) > textwidth("+"))
    {
        line(x, y, x-10, y + 10);
        line(x, y, x+10, y + 10);    //am limitat marimea sagetii, in caz ca elem este foarte lat

    }
    else
    {
        line(x, y, x-textwidth(text)/2, y + 10);
        line(x, y, x+textwidth(text)/2, y + 10);
    }
}

int lungimeTextPartial(const char* text, int pozitie)
{
    if (pozitie < 0 || pozitie > strlen(text))
    {
        return -1;  // Verifică dacă poziția este validă
    }

    char* textPartial = new char[pozitie + 1];  // Creează un șir temporar pentru porțiunea de text până la poziția cursorului
    strncpy(textPartial, text, pozitie);
    textPartial[pozitie] = '\0';  // Adaugă terminatorul de șir

    int lungime = textwidth(textPartial);  // Măsoară lungimea textului parțial

    delete[] textPartial;  // Eliberează memoria alocată

    return lungime;
}

void deseneazaInputText(int x, int y, int latime, int inaltime, int pozitieCursor,  char* propozitie)
{

    setfillstyle(SOLID_FILL, getbkcolor());
    bar(x + 1, y + 1, x + latime - 1, y + inaltime - 1);
    setcolor(WHITE);

    // Desenează textul
    outtextxy(x + 5, y + 5, propozitie);

    // Calculați coordonatele liniei verticale în funcție de poziția cursorului și textul introdus
    int xCursor = x + 5 + textwidth(propozitie);
    if (pozitieCursor >= 0 && pozitieCursor <= strlen(propozitie))
    {
        // Calculează coordonatele liniei verticale în funcție de poziția cursorului și textul introdus

        int xCursor = x + 5 + lungimeTextPartial(propozitie, pozitieCursor);;

        // Desenează linia verticală
        line(xCursor, y + 5, xCursor, y + inaltime - 5);
    }
}

void gestioneazaApasareTasta(char* propozitie, int& pozitieCursor, int lungimeMaxima, char tasta)
{
    if (tasta == '\b' && pozitieCursor > 0)
    {
        // Gestionează backspace
        pozitieCursor--;
        for (int i = pozitieCursor; i < strlen(propozitie); i++)
        {
            propozitie[i] = propozitie[i + 1];
        }
        propozitie[lungimeMaxima - 1] = '\0';
    }
    else if (tasta == 0 || tasta == 224)
    {
        // Gestionează tastele săgeată
        tasta = getch();
        if (tasta == 75 && pozitieCursor > 0)
        {
            pozitieCursor--; // Săgeată stânga
        }
        else if (tasta == 77 && pozitieCursor < strlen(propozitie))
        {
            pozitieCursor++; // Săgeată dreapta
        }
    }
    else if (tasta >= 32 && tasta <= 126 && strlen(propozitie) < lungimeMaxima - 1 && textwidth(propozitie)<680)
    {
        // Gestionează caracterele imprimabile
        for (int i = strlen(propozitie); i > pozitieCursor; i--)
        {
            propozitie[i] = propozitie[i - 1];
        }
        propozitie[pozitieCursor] = tasta;
        pozitieCursor++;
    }
}

void gestioneazaTastaVariabile(char* propozitie, int& pozitieCursor, int lungimeMaxima, char tasta, int i)
{
    if (tasta == '\b' && pozitieCursor > 0)
    {
        // Handling backspace
        pozitieCursor--;
        if (propozitie[pozitieCursor] == '.')
        {
            casutaText[i].aparitiePunct = 0;
        }
        for (int j = pozitieCursor; j < strlen(propozitie); j++)
        {
            propozitie[j] = propozitie[j + 1];
        }
        propozitie[lungimeMaxima - 1] = '\0';
    }
    else if (tasta == 0 || tasta == 224)
    {
        // Handling arrow keys
        tasta = getch();
        if (tasta == 75 && pozitieCursor > 0)
        {
            pozitieCursor--; // Left arrow
        }
        else if (tasta == 77 && pozitieCursor < strlen(propozitie))
        {
            pozitieCursor++; // Right arrow
        }
    }
    else
    {
        if ((tasta >= '0' && tasta <= '9') && strlen(propozitie) < lungimeMaxima - 1)
        {
            // Handling digits
            casutaText[i].aFostScrisaCifra = 1;
            for (int j = strlen(propozitie); j > pozitieCursor; j--)
            {
                propozitie[j] = propozitie[j - 1];
            }
            propozitie[pozitieCursor] = tasta;
            pozitieCursor++;
        }
        else if (tasta == '.' && casutaText[i].aFostScrisaCifra == 1 && casutaText[i].aparitiePunct == 0)
        {
            // Handling dot
            casutaText[i].aparitiePunct = 1;
            for (int j = strlen(propozitie); j > pozitieCursor; j--)
            {
                propozitie[j] = propozitie[j - 1];
            }
            propozitie[pozitieCursor] = tasta;
            pozitieCursor++;
        }
    }
}

void clearInputBuffer()
{
    while (kbhit())
    {
        getch();
    }
}

int ClickInCasuta(int xStart, int xSfarsit, int yStart, int ySfarsit)
{
    int a=-1;   //va returna -1 daca mouse ul nu a fost apasat
    if(ismouseclick(WM_LBUTTONDOWN))
    {

        getmouseclick(WM_LBUTTONDOWN,xMouse,yMouse);

        if(xMouse>=xStart && xMouse<=xSfarsit && yMouse>=yStart && yMouse<=ySfarsit)
            a=1;  //daca mouse ul a fost apasat in chenar
        else a=0;   //daca mouse ul a fost apasat dar nu in chenar
        clearmouseclick(WM_LBUTTONDOWN);
    }
    return a;
}


int main()
{
    initwindow(1920,1080, "Evaluator Matematic");
    int window1 = 0;

    setbkcolor(MAGENTA);
    cleardevice();
    settextstyle(4,0,3);                                 ///initializarea
    outtextxy(50,50,"EVALUATOR DE FORMULE MATEMATICE");
    outtextxy(10, 150, "Dati click mai jos pentru a introduce formula:");

    setcolor(WHITE);
    rectangle(10, 175, 710, 215);


    int x = 10, y = 175, copiex = 15, copiey = 180;   // Initial position to display text
    char ch;


    char propozitie[MAX_LUNGIME_PROPOZITIE] = "";
    int pozitieCursor = 0;
    int primitTextInput = -1;

prop:
    setcurrentwindow(window1);

    while(1)
    {
        if(ClickInCasuta(10,710,175,215) == 1)
            esteCasutaTextActiva = 1;
        else esteCasutaTextActiva = 0;


        while(esteCasutaTextActiva == 1)      ///citesc inputul daca a fost apasata casuta de text respectiva
        {
           
            if(ClickInCasuta(10,710,175,215) == 0)
            {
                esteCasutaTextActiva = 0;
                break;
            }


            deseneazaInputText(10, 175, 700, 40,pozitieCursor, propozitie);
            if(kbhit())
            {
                char tasta = getch();

                if(tasta == 13)
                {
                    primitTextInput = 1;
                    esteCasutaTextActiva = 0;
                }
                else
                    gestioneazaApasareTasta(propozitie, pozitieCursor, MAX_LUNGIME_PROPOZITIE, tasta);

            }
            // else clearInputBuffer();

            delay(50);
        }

        if(esteCasutaTextActiva==0) clearInputBuffer();




        if(primitTextInput == 1)         ///daca am primit un text
        {
            primitTextInput = 2; ///sa afiseze greseli etc o singura data, pana primesc alt text

            cout<<propozitie<<endl;    ///afisare temporara a propozitiei de la input

            for(i=0; i<=lungime_formula; i++) ///reinitializarea variabilelor in caz de input nou
            {
                for(j=0; j<256; j++)
                    formula[i][j]='\0';
                caracter[i].x = 0;
                caracter[i].y = 0;
                tipuri[i]=0;
            }

            paranteza = 0;

            for(i=1; i<=nrgreseli; i++)
            {
                pozitie_greseala[i]=0;
                tipuriGreseli[i]=0;
            }
            lungime_formula=0;
            pozVecCar = 0;
            nrgreseli = 0;

            for (int i = 0; i < strlen(propozitie); i++)
            {
                if (esteCifra(propozitie, i))
                {
                    adaugaNumar(propozitie, i);
                    pozVecCar++;
                }

                else if (esteParantezaDeschisa(propozitie, i))
                {
                    adaugaParantezaDeschisa(propozitie, i, paranteza);
                    pozVecCar++;
                }

                else if (esteParantezaInchisa(propozitie, i))
                {
                    adaugaParantezaInchisa(propozitie, i, paranteza);
                    pozVecCar++;
                }

                else if (esteSemn(propozitie, i))
                {
                    if (propozitie[i] == '-')
                    {
                        if (propozitie[i - 1] == '(' && esteCifra(propozitie, i + 1))
                        {
                            strcat(formula[lungime_formula++], "-");
                            lungime_formula--;
                            i++;
                            adaugaNumar(propozitie, i);
                        }
                        else adaugaSemn(propozitie, i);
                    }
                    else adaugaSemn(propozitie, i);
                    pozVecCar++;
                }

                else if (esteLitera(propozitie, i))
                {
                    adaugaFunctie(propozitie, i, esteFunctie);
                    if (esteFunctie == 0)
                        adaugaVariabila(propozitie, i);
                    pozVecCar++;
                }

                else if (propozitie[i] != ' ')
                {
                    formula[lungime_formula++][0]=propozitie[i];
                    pozitie_greseala[++nrgreseli] = pozVecCar + 1;
                    tipuriGreseli[nrgreseli] = 3;
                    pozVecCar++;
                }  ///caracter necunoscut/invalid
            }

          
            caracter[0].x = copiex;
            caracter[0].y = copiey;        ///coordonatele pe fereastra grafica a fiecarui element
            for(i=1; i<lungime_formula; i++)
            {
                caracter[i].x = caracter[i-1].x + textwidth(formula[i-1]);
                caracter[i].y = copiey;
            }

            for (i = 0; i < lungime_formula; i++) ///am pus la fiecare variabila cate un tip
            {
                if (strchr("scarelt", formula[i][0]) != NULL)
                {
                    adevarat = 0;
                    for (int j = 0; j < 8 && adevarat == 0; j++)
                    {
                        if (strcmp(formula[i], functii[j]) == 0)
                            adevarat = 1;
                    }
                    if (adevarat == 1)
                        tipuri[i] = DenumireTip(formula[i]);
                    else
                        tipuri[i] = 6;
                }
                else
                    tipuri[i] = DenumireTip(formula[i]);
            }

            cout << endl;

            for (i = 0; i < lungime_formula; i++)
                cout << tipuri[i] << " ";
            cout << endl;

            verificareValiditateSintactica(); ///verificam formula

            char string[20];
            int xErori = 10,yErori = 250;

erori:
            if(paranteza)
                tipuriGreseli[++nrgreseli] = 6;

            if(nrgreseli > 0)                  ///daca sunt greseli
            {
                setfillstyle(SOLID_FILL, getbkcolor());
                bar(10,220,1000,1000);                 ///sterg orice text era scris anterior

                for ( i = 1; i <= nrgreseli; i++) ///iterez prin greseli
                {

                    setcolor(BLUE);
                    setlinestyle(SOLID_LINE, 0, 3);   ///setarile de desen pentru subliniere si sageata(care indica greseala)

                    if(pozitie_greseala[i])
                    {
                        deseneazaSageata(caracter[pozitie_greseala[i]-1].x + textwidth(formula[pozitie_greseala[i]-1])/2, copiey + 45, formula[pozitie_greseala[i]-1]);
                        line(caracter[pozitie_greseala[i]-1].x + 3, copiey + textheight(formula[pozitie_greseala[i]-1]), caracter[pozitie_greseala[i]-1].x + textwidth(formula[pozitie_greseala[i]-1]) - 3, copiey +  textheight(formula[pozitie_greseala[i]-1]));
                    }

                    setcolor(WHITE);                     ///reinitializez setarile de desenare
                    setlinestyle(SOLID_LINE, 0, 1);

                    itoa(i,string,10);
                    outtextxy(xErori,yErori,string);           ///enunt erorile
                    enuntareGreseli(i,xErori+20,yErori);
                    yErori +=25;

                }

            }
            else                                               ///daca nu am greseli
            {

///daca se scrie 13. spre exemplu sa il facem ca si cum ar fi scris 13.0 => sa devina 13

                for(i=1; i<=totalVariabile; i++)
                {
                    
                    casutaText[i].casutaVarActiva = -1;        ///resetez structurile folosite
                    casutaText[i].cursorVariabia = 0;
                    casutaText[i].primitValoare = 0;
                    casutaText[i].aFostScrisaCifra=0;
                    casutaText[i].aparitiePunct = 0;
                    
                    for(j=0; j<=256; j++)
                    {
                        
                        valoriVariabile[i][j] = NULL;
                    }
                }
                totalVariabile = 0;

                setfillstyle(SOLID_FILL, getbkcolor());
                bar(10,220,1000,1000);                     ///sterg orice text era scris anterior
                setcolor(WHITE);


                outtextxy(xErori,yErori,"Formula este corecta!");

               


                for(i=0; i<lungime_formula; i++)
                {
                    if ((tipuri[i] == 6 && !EsteNumar(formula[i])) && formula[i][0] != 'q' && formula[i][0] != '-')
                    {
                        int ok = 0, indiceValoare = 1;
                        char aux1[256];
                        strcpy(aux1, formula[i]);
                        for(int j = 0; j < strlen(aux1); j++)
                            if(aux1[j] >= 'A' && aux1[j] <= 'Z')
                                aux1[j] = aux1[j] + ('a' - 'A');
                        for(int j = 1; j <= totalVariabile; j++)
                        {
                            char aux2[256];
                            aux2[0] = NULL;
                            strcpy(aux2, variabile[j]);
                            for(int h = 0; h < strlen(aux1); h++)
                                if(aux2[h] >= 'A' && aux2[h] <= 'Z')
                                    aux2[h] = aux2[h] + ('a' - 'A');
                            if(strcmp(aux1, aux2) == 0)
                                ok = 1;
                            else
                                indiceValoare++;
                        }
                        if(!ok)
                        {
                            strcpy(variabile[++totalVariabile], formula[i]);
                        }
                        ok = 0;
                    }
                }                                               
                cout<<totalVariabile;
                int xTextVar = xErori;
                int yTextVar = yErori + 25;

                for(i=1; i<=totalVariabile; i++)
                {
                    outtextxy(xTextVar, yTextVar, "Introduceti valoarea variabilei ");
                    outtextxy(xTextVar + textwidth("Introduceti valoarea variabilei "),yTextVar, variabile[i] );

                    casutaText[i].x = textwidth("Introduceti valoarea variabilei ") + textwidth(variabile[i]) + 15;
                    casutaText[i].y = yTextVar;                                                                    ///determin coordonatele casutei

                    rectangle(casutaText[i].x, casutaText[i].y, casutaText[i].x + 300, casutaText[i].y + 27);  ///desenez casuta
                    yTextVar += 35;
                }
                yErori = yTextVar;

                while(1)
                {
                    ok=1;
                    for(int t=1; t<=totalVariabile; t++)
                        if(casutaText[t].primitValoare == 0)
                        {
                            ok=0;
                            break;
                        }


                    if(ok==1)
                        break;

                    if(ismouseclick(WM_LBUTTONDOWN))
                    {
                        getmouseclick(WM_LBUTTONDOWN,xMouse,yMouse);

                        for(j=1; j<=totalVariabile; j++)
                            if(xMouse>=casutaText[j].x && xMouse<=casutaText[j].x + 300 && yMouse>=casutaText[j].y && yMouse<=casutaText[j].y+27 )
                            {
                                casutaText[j].casutaVarActiva = 1;
                                casutaText[j].primitValoare = 0;
                            }
                            else casutaText[j].casutaVarActiva = 0;
                    }
                    else clearInputBuffer();

                    for(i=1; i<=totalVariabile; i++)
                        if(casutaText[i].casutaVarActiva == 1)
                            while( casutaText[i].casutaVarActiva == 1 )
                            {

                                if(ismouseclick(WM_LBUTTONDOWN))
                                {
                                    getmouseclick(WM_LBUTTONDOWN,xMouse,yMouse);

                                    for(j=1; j<=totalVariabile; j++)
                                        if(xMouse>=casutaText[j].x && xMouse<=casutaText[j].x + 300 && yMouse>=casutaText[j].y && yMouse<=casutaText[j].y+27 )
                                        {
                                            casutaText[i].casutaVarActiva = 0 ;
                                            casutaText[j].casutaVarActiva = 1;
                                            casutaText[j].primitValoare = 0;
                                        }
                                        else casutaText[i].casutaVarActiva = 0;
                                  
                                }

                                if(casutaText[i].casutaVarActiva == 0)
                                    break;

                                settextstyle(4,0,2);
                                for(int k=1; k<=totalVariabile; k++)
                                {
                                    deseneazaInputText(casutaText[k].x, casutaText[k].y, 300, 27, casutaText[k].cursorVariabia, valoriVariabile[k]);

                                }

                                delay(50);
                                if(kbhit())
                                {
                                    char tasta = getch();                  ///desenez textul si gestionez tastele apasate

                                    if(tasta == 13)
                                    {
                                        casutaText[i].primitValoare = 1;
                                        casutaText[i].casutaVarActiva = 0;

                                       

                                    }
                                    else
                                        gestioneazaTastaVariabile(valoriVariabile[i], casutaText[i].cursorVariabia, 256, tasta,i);

                                }
                            }
                } //acolada while(1)

                settextstyle(4,0,3);    ///setat textul inapoi

                lungime_formula++;
                for(i=lungime_formula; i>=0; i--)
                    strcpy(formula[i],formula[i-1]);
                strcpy(formula[0],"(");               ///shiftare cu 1 mai incolo a elementelor pt a pune paranteza deschisa si inchisa
                lungime_formula++;
                strcpy(formula[lungime_formula-1],")");

                for(i=lungime_formula-1; i>=1; i--)
                    tipuri[i] = tipuri[i-1];
                tipuri[i] = 0;

                settextstyle(4,0,3);    ///setat textul inapoi
                for (i = 1; i <= nrgreseli; i++) ///afiseaza greselile
                {
                    cout << "Pe pozitia cu numarul " << pozitie_greseala[i] - 1 << ' ' << "se afla o greseala de tipul " << tipuriGreseli[pozitie_greseala[i]];
                    cout << endl;
                }

                for (i = 0; i <= lungime_formula; i++) /// facem stiva pentru arbore
                {
                    if (tipuri[i] == 6)
                        strcpy(stiva[++totals], formula[i]);
                    else if (tipuri[i] == 5)
                        strcpy(operatori[++totalo], formula[i]);
                    else if (tipuri[i] == 1 || tipuri[i] == 2 || tipuri[i] == 3 || tipuri[i] == 4)
                    {
                        while (operatori[totalo][0] != '(' && (DenumireTip(operatori[totalo]) >= DenumireTip(formula[i])))
                        {
                            strcpy(stiva[++totals], operatori[totalo]);
                            totalo--;
                        }
                        strcpy(operatori[++totalo], formula[i]);
                    }
                    else if (formula[i][0] == ',')
                    {
                        while (operatori[totalo][0] != '(')
                        {
                            strcpy(stiva[++totals], operatori[totalo]);
                            totalo--;
                        }
                    }
                    else if (formula[i][0] == '(')
                        strcpy(operatori[++totalo], formula[i]);
                    else if (formula[i][0] == ')')
                    {
                        while (operatori[totalo][0] != '(')
                        {
                            if (totalo == 0)
                                cout << "Paranteze gresite" << endl;
                            strcpy(stiva[++totals], operatori[totalo]);
                            totalo--;
                        }
                        totalo--;
                        if (esteLitera(operatori[totalo], 0))
                        {
                            strcpy(stiva[++totals], operatori[totalo]);
                            totalo--;
                        }
                    }
                }
                while (totalo) ///punem restul operatorilor din stiva de operatori in cea principala
                {
                    if (operatori[totalo][0] != '(')
                        strcpy(stiva[totals++], operatori[totalo]);
                    totalo--;
                }

                for (i = 1; i <= totals; i++)
                    cout << stiva[i] << " ";
                cout << endl;

             

                float a = ValoareFunctie(formula, lungime_formula, valoriVariabile);

                if (a == INT_MAX)
                {
                    tipuriGreseli[++nrgreseli] = 5;
                    setfillstyle(SOLID_FILL, getbkcolor());
                    bar(10,220,1000,1000);
                    outtextxy(xErori,yErori,"1");
                    enuntareGreseli(nrgreseli,xErori+20,yErori); 
                    cout << "Eroare! Ecuatia are cazuri de limita!";

                    
                }
                else
                {
                    char raspuns[256];
                    sprintf(raspuns, "%f", a);
                    outtextxy(xErori+20,yErori+25,"Raspunsul: ");
                    outtextxy(xTextVar + textwidth("Raspunsul: ") + 10,yErori+25, raspuns);

                    cout << a;

                    ///prezentarea textului

                /// Build the expression tree
                nodArbore* root = buildExpressionTree(stiva, totals);

                int height=1080, width=1920;
                int window2 = initwindow(width,height, "Arborele expresiei");
                setcurrentwindow(window2);
                setcolor(WHITE);
                setbkcolor(MAGENTA);
                cleardevice();
                rectangle(1,1,width-1,height-1);
                int latime, inaltime;
                latime=width/nrColoane(root);
                inaltime=height/nrNiveluri(root);
                deseneazaArbore(root,1,0,latime,inaltime);
                while (1)
                {
                    if (kbhit())
                    {
                        char ch = getch();
                        if (ch == 27)    // 27 is the ASCII code for the escape key
                        {
                            // Close the first window
                            closegraph(window2);

                            goto prop;
                            break;
                        }
                    }
                }
                }

                ///exemplu pt compilare 5+7*sin(&)+15.7.8 ; 5+7*4/sin(&)+ln((7/sexy)+9)


            }
        }

  


    }

    return 0;
}
