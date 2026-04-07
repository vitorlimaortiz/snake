#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define TOTAL_FRUTAS 10
#define LARGURA_MAPA 80 
#define ALTURA_MAPA 25

#define NIVEL_FACIL 1
#define NIVEL_NORMAL 2
#define NIVEL_DIFICIL 3

typedef struct El {
    int posX, posY;
    struct El* prox;
} Elemento;

typedef struct {
    int x, y;
} Comida;

Elemento* primeiro = NULL;
Elemento* ultimo = NULL;
Comida lista_comidas[TOTAL_FRUTAS];


int din_offset_x = 0;
int din_margem_superior = 0;

int deltaX = 0, deltaY = 0;
int pontos = 0, fim_de_jogo = 0;
int recorde = 0; 
int modoClaro = 0;
int modo_ativo = NIVEL_NORMAL;
int delay = 140;

HANDLE hSaida;

/* utilitarios */

void irPara(int x, int y) {
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(hSaida, pos);
}

void centralizarJanela() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hSaida, &csbi);
    
    int largura_janela = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int altura_janela = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    din_offset_x = (largura_janela - LARGURA_MAPA) / 2;
    din_margem_superior = (altura_janela - ALTURA_MAPA) / 2;

    if (din_offset_x < 0) din_offset_x = 0;
    if (din_margem_superior < 0) din_margem_superior = 0;
}

void trocarCor(int cor) {
    if (modoClaro == 1) {
        if (cor == 11 || cor == 14 || cor == 15) SetConsoleTextAttribute(hSaida, 241); 
        else if (cor == 10 || cor == 2) SetConsoleTextAttribute(hSaida, 242); 
        else if (cor == 12) SetConsoleTextAttribute(hSaida, 244); 
        else SetConsoleTextAttribute(hSaida, 240); 
    } else {
        if (cor == 10 || cor == 2) SetConsoleTextAttribute(hSaida, 15); 
        else SetConsoleTextAttribute(hSaida, cor);
    }
}

void ocultarCursor() {
    CONSOLE_CURSOR_INFO c_info;
    c_info.dwSize = 1;
    c_info.bVisible = FALSE;
    SetConsoleCursorInfo(hSaida, &c_info);
}

/*  menu  */

int exibirMenu() {
    centralizarJanela(); // Recalcula ao abrir o menu
    if (modoClaro == 1) system("color F8"); else system("color 07");
    system("cls");

    int centro_x = din_offset_x + (LARGURA_MAPA / 2);
    
    trocarCor(11); 
    irPara(centro_x - 17, 5);  printf("===================================");
    irPara(centro_x - 17, 6);  printf("||                               ||");
    irPara(centro_x - 17, 7);  printf("||             Snake             ||");
    irPara(centro_x - 17, 8);  printf("||                               ||");
    irPara(centro_x - 17, 9);  printf("===================================");

    trocarCor(15); 
    irPara(centro_x - 10, 12); printf("[1] -> INICIAR JOGO");
    irPara(centro_x - 10, 13); printf("[2] -> DIFICULDADE");
    irPara(centro_x - 10, 14); printf("[3] -> SAIR");
    
    while (1) {
        if (_kbhit()) {
            char btn = _getch();
            if (btn == '1') return 1;
            if (btn == '2') return 2;
            if (btn == '3') return 3;
        }
    }
}

void configurarModo() {
    system("cls");
    trocarCor(11);
    irPara(din_offset_x + 7, 8);
    printf("=== SELECIONE A DIFICULDADE ===");
    trocarCor(15);
    irPara(din_offset_x + 12, 11); printf("1 - Iniciante");
    irPara(din_offset_x + 12, 12); printf("2 - Intermediario");
    irPara(din_offset_x + 12, 13); printf("3 - Avancado");
    while (1) {
        if (_kbhit()) {
            char btn = _getch();
            if (btn == '1') { modo_ativo = NIVEL_FACIL; break; }
            if (btn == '2') { modo_ativo = NIVEL_NORMAL; break; }
            if (btn == '3') { modo_ativo = NIVEL_DIFICIL; break; }
        }
    }
}

/*  CORPO DA COBRA  */

void inserir_no_inicio(int x, int y) {
    Elemento* novo = (Elemento*)malloc(sizeof(Elemento));
    if (novo == NULL) return;
    novo->posX = x; novo->posY = y;
    if (!primeiro) {
        primeiro = ultimo = novo;
        novo->prox = novo;
    } else {
        novo->prox = primeiro;
        ultimo->prox = novo;
        primeiro = novo;
    }
}

void excluir_do_final() {
    if (primeiro == NULL || primeiro == ultimo) return;
    Elemento* aux = primeiro;
    while (aux->prox != ultimo) aux = aux->prox;
    irPara(ultimo->posX, ultimo->posY);
    printf(" ");
    aux->prox = primeiro;
    free(ultimo);
    ultimo = aux;
}

void limparCobra() {
    if (primeiro == NULL) return;
    Elemento* atual = primeiro;
    Elemento* proximo;
    do {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    } while (atual != primeiro);
    primeiro = ultimo = NULL;
}

/*  mapa e interface  */

void desenharMoldura() {
    trocarCor(11);
    for (int i = din_offset_x; i <= LARGURA_MAPA + din_offset_x; i++) {
        irPara(i, din_margem_superior); printf("=");
        irPara(i, ALTURA_MAPA + din_margem_superior); printf("=");
    }
    for (int i = din_margem_superior; i <= ALTURA_MAPA + din_margem_superior; i++) {
        irPara(din_offset_x, i); printf("|");
        irPara(LARGURA_MAPA + din_offset_x, i); printf("|");
    }
}

void mostrarPainel() {
    int lateralX = LARGURA_MAPA + din_offset_x + 5;
    int base_y = din_margem_superior;
    int largura_caixa = 22; 
    int altura_caixa = 11;  

    trocarCor(11);
    irPara(lateralX, base_y); printf("+----------------------+"); 
    for(int i = 1; i <= altura_caixa; i++) {
        irPara(lateralX, base_y + i); printf("|");
        irPara(lateralX + largura_caixa + 1, base_y + i); printf("|");
    }
    irPara(lateralX, base_y + altura_caixa + 1); printf("+----------------------+");

    irPara(lateralX + 3, base_y + 1); printf("==== STATUS ====");
    irPara(lateralX + 2, base_y + 3); trocarCor(15); printf("MODO: ");
    if (modo_ativo == NIVEL_FACIL) { trocarCor(10); printf("FACIL   "); }
    else if (modo_ativo == NIVEL_NORMAL) { trocarCor(14); printf("NORMAL  "); }
    else if (modo_ativo == NIVEL_DIFICIL) { trocarCor(12); printf("DIFICIL "); }

    irPara(lateralX + 2, base_y + 5); trocarCor(15); printf("VELO: %dms", delay);
    irPara(lateralX + 2, base_y + 7); trocarCor(15); printf("PONTOS: ");
    trocarCor(10); printf("%06d", pontos);
    irPara(lateralX + 2, base_y + 8); trocarCor(15); printf("RECORDE: ");
    trocarCor(14); printf("%06d", recorde);
    irPara(lateralX + 2, base_y + 10); trocarCor(8); printf("P-Pausar  C-Tema");
}

/* colisao e comida */

int detectarImpacto(int x, int y) {
    if (x <= din_offset_x || x >= LARGURA_MAPA + din_offset_x || y <= din_margem_superior || y >= ALTURA_MAPA + din_margem_superior)
        return 1;
    if (primeiro == NULL) return 0;
    Elemento* aux = primeiro->prox;
    while (aux != primeiro) {
        if (aux->posX == x && aux->posY == y) return 1;
        aux = aux->prox;
    }
    return 0;
}

int verificarSobreposicao(int x, int y) {
    if (primeiro == NULL) return 0;
    Elemento* aux = primeiro;
    do {
        if (aux->posX == x && aux->posY == y) return 1;
        aux = aux->prox;
    } while (aux != primeiro);
    return 0;
}

void criarComida(int i) {
    do {
        lista_comidas[i].x = rand() % (LARGURA_MAPA - 2) + 1 + din_offset_x;
        lista_comidas[i].y = rand() % (ALTURA_MAPA - 1) + 1 + din_margem_superior;
    } while (verificarSobreposicao(lista_comidas[i].x, lista_comidas[i].y));
    trocarCor(12);
    irPara(lista_comidas[i].x, lista_comidas[i].y);
    printf("#");
}

void espalharComidas() {
    for (int i = 0; i < TOTAL_FRUTAS; i++) criarComida(i);
}

/* logica */

void realizarPause() {
    irPara(din_offset_x + (LARGURA_MAPA / 2) - 4, ALTURA_MAPA / 2 + din_margem_superior);
    trocarCor(14); printf(" PAUSADO ");
    while (1) {
        if (_kbhit()) { char c = _getch(); if (c == 'p' || c == 'P') break; }
        Sleep(50);
    }
    irPara(din_offset_x + (LARGURA_MAPA / 2) - 4, ALTURA_MAPA / 2 + din_margem_superior);
    printf("         ");
}

void lerTeclado() {
    if (_kbhit()) {
        int tecla = _getch();
        if (tecla == 'p' || tecla == 'P') { realizarPause(); return; }
        if (tecla == 'c' || tecla == 'C') {
            modoClaro = !modoClaro;
            if (modoClaro == 1) system("color F8"); else system("color 07");
            system("cls"); centralizarJanela(); desenharMoldura(); espalharComidas(); mostrarPainel();
            return;
        }
        if (tecla == 224) {
            tecla = _getch();
            if (tecla == 72 && deltaY != 1) { deltaX = 0;  deltaY = -1; }
            if (tecla == 80 && deltaY != -1) { deltaX = 0;  deltaY = 1; }
            if (tecla == 75 && deltaX != 1) { deltaX = -1; deltaY = 0; }
            if (tecla == 77 && deltaX != -1) { deltaX = 1;  deltaY = 0; }
        }
        if ((tecla == 'w' || tecla == 'W') && deltaY != 1) { deltaX = 0;  deltaY = -1; }
        if ((tecla == 's' || tecla == 'S') && deltaY != -1) { deltaX = 0;  deltaY = 1; }
        if ((tecla == 'a' || tecla == 'A') && deltaX != 1) { deltaX = -1; deltaY = 0; }
        if ((tecla == 'd' || tecla == 'D') && deltaX != -1) { deltaX = 1;  deltaY = 0; }
    }
}

void processarLogica() {
    if (deltaX == 0 && deltaY == 0) return;
    int nextX = primeiro->posX + deltaX;
    int nextY = primeiro->posY + deltaY;
    if (detectarImpacto(nextX, nextY)) { fim_de_jogo = 1; return; }
    trocarCor(2); irPara(primeiro->posX, primeiro->posY); printf("o");
    inserir_no_inicio(nextX, nextY);
    trocarCor(10); irPara(nextX, nextY); printf("@");
    int pegouItem = 0;
    for (int i = 0; i < TOTAL_FRUTAS; i++) {
        if (nextX == lista_comidas[i].x && nextY == lista_comidas[i].y) {
            pontos += 15; 
            if (pontos > recorde) recorde = pontos;
            criarComida(i); 
            pegouItem = 1;
        }
    }
    if (!pegouItem) excluir_do_final();
    mostrarPainel();
}

int telaPerdeu() {
    system("cls");
    int centro_x = din_offset_x + (LARGURA_MAPA / 2);
    trocarCor(12); irPara(centro_x - 5, 10); printf("FIM DE JOGO");
    trocarCor(14); irPara(centro_x - 5, 12); printf("Pontos: %d", pontos);
    trocarCor(10); irPara(centro_x - 5, 13); printf("Recorde: %d", recorde);
    trocarCor(15);
    irPara(centro_x - 8, 15); printf("1 - Tentar Novamente");
    irPara(centro_x - 8, 16); printf("2 - Voltar ao Menu");
    irPara(centro_x - 8, 17); printf("3 - Sair");
    while (1) {
        if (_kbhit()) {
            char r = _getch();
            if (r == '1') return 1;
            if (r == '2') return 2;
            if (r == '3') return 3;
        }
    }
}

void resetarSistema() {
    limparCobra();
    pontos = 0; deltaX = 0; deltaY = 0; fim_de_jogo = 0;
    system("cls");
    centralizarJanela(); // Centraliza antes de redesenhar tudo
    inserir_no_inicio(din_offset_x + (LARGURA_MAPA / 2), (ALTURA_MAPA / 2) + din_margem_superior);
    desenharMoldura(); espalharComidas();
    trocarCor(10); irPara(primeiro->posX, primeiro->posY); printf("@");
    mostrarPainel();
}

int main() {
    hSaida = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned int)time(NULL));
    ocultarCursor();
    centralizarJanela();

    while (1) {
        int menuOp = exibirMenu(); 
        if (menuOp == 2) { configurarModo(); continue; }
        if (menuOp == 3) return 0;
        if (menuOp == 1) {
            if (modo_ativo == NIVEL_FACIL) delay = 170;
            else if (modo_ativo == NIVEL_NORMAL) delay = 120;
            else if (modo_ativo == NIVEL_DIFICIL) delay = 75;

            resetarSistema();
            while (!fim_de_jogo) {
                lerTeclado();
                processarLogica();
                Sleep(delay);
            }
            int posJogo = telaPerdeu();
            if (posJogo == 2) continue; 
            if (posJogo == 3) return 0;
            if (posJogo == 1) {
                fim_de_jogo = 0;
                resetarSistema();
                while (!fim_de_jogo) {
                    lerTeclado();
                    processarLogica();
                    Sleep(delay);
                }
            }
        }
    }
    return 0;
}
