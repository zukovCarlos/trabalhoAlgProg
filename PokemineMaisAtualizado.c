#include "raylib.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LARGURAJANELA 800
#define ALTURAJANELA 800
#define ARESTA 20
#define LARGURAMAPA 30
#define ALTURAMAPA 20
#define MAXTOUPEIRAS 200
#define SEGUNDO 60
#define POSICAOMAPA 20
#define ESCALA 0.835
#define IMAGENSTOTAL 25

typedef struct Vetor2{
    int x;
    int y;
}Vetor2;

typedef struct Tiro{
    Vetor2 posicaoAtual;
    int direcaoAtual;
    int umTiroPorVez;
}TIRO;

/* Tipo Jogador
    Basicamente o player, possuindo dois Vetor2, um para a posicao atual e outro
    para indicar o deslocamento que o player deseja fazer.
    Alem disso, um contador de vidas
*/
typedef struct Jogador{
    Vetor2 posInicial;
    Vetor2 posicaoAtual; //Vetor2 armazena a atual posicao x e y do jogador.
    Vetor2 deslocamento; //Vetor2 armazena como o jogador quer se movimentar
    int vidas; // vidas do jogador
    int direcaoAtual;// direcao para qual ele esta se movimentando
    int pontos; // pontuacao do jogador
    int esmeraldas; // esmeraldas restantes
    TIRO tiro; // TIRO
    int powerUp; // powerup presente
    int mapaAtual; // mapa atual
    int jogoPausado;
    int sairJogo;
    int primeiraVezRodadando;
    int perdeu;
}JOGADOR;

/* Tipo toupeira
    Inimigos do jogo, possuem um Vetor2 com a posicao atual e um contador de vida
*/
typedef struct Toupeira{
    Vetor2 posicaoAtual;
    Vetor2 posInicial;
    int vida;
    int contadorDirecao;
    int direcaoAtual;
    Vetor2 deslocamento;
} TOUPEIRA;

typedef struct Imagens{
    Texture2D imagem;
} IMAGENS;

TOUPEIRA IniciaToupeira(int x, int y){
    TOUPEIRA auxiliar;
    auxiliar.posicaoAtual.x = auxiliar.posInicial.x = x;
    auxiliar.posicaoAtual.y = auxiliar.posInicial.y = y;
    auxiliar.vida = 1;
    auxiliar.direcaoAtual = 1;
    return auxiliar;
}


void GanhouOJogo(JOGADOR jogador){
    Sound vitoriaMus = LoadSound("Sons/vitoria.mp3");
    Texture2D vitoria = LoadTexture("Texturas/vitoria.png");
    printf("oi");
    while(!WindowShouldClose()){
        BeginDrawing();
        int centroX = GetScreenWidth() / 2;
        int centroY = GetScreenHeight() / 2;
        ClearBackground(BLACK);
        if(!IsSoundPlaying(vitoriaMus))
            PlaySound(vitoriaMus);

        Vector2 posicao;
        posicao.x = centroX - (vitoria.width ); //
        posicao.y = centroY - (vitoria.height);


        DrawTextureEx(vitoria, posicao, 0, 2, WHITE);
        DrawText(TextFormat("PONTOS: %d",jogador.pontos),posicao.x+600, posicao.y+600, 40, WHITE);
        EndDrawing();
    }

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Termina o programa
    //-----------------------------------------------------------------------------------

}

/* Funcao IniciaRodada
    Percorre o mapa desejado e atribui a todos os objetos suas posicoes atuais baseando-se
    no mapa. Alem disso, confere a fase atual e carrega o mapa desejado e faz uma copia para o mapa auxiliar, para que ele possa ser atualizado e modificado
*/
int IniciaRodada(Vetor2* posInicial,Vetor2* posicaoAtual, TOUPEIRA toupeiras[], char mapaAuxiliar[][LARGURAMAPA], int* esmeraldas, int mapaAtual, Camera2D* camera, JOGADOR jogador){
    FILE *arq;
    int i,j, toupeirasAtualmente = 0;
    char c, numeroMapa[1], mapaDaRodada [50] = {'\0'};
    *esmeraldas = 0;
    numeroMapa[0] = '0' + (mapaAtual + 1);
    strcat(mapaDaRodada, "mapas/mapa");
    strcat(mapaDaRodada, numeroMapa);
    strcat(mapaDaRodada, ".txt");
    if(!(arq = fopen(mapaDaRodada,"r")))
        GanhouOJogo(jogador);
    else{
        for (i = 0; i < ALTURAMAPA;i++){
            for(j = 0; j < LARGURAMAPA+1;j++){
                c = getc(arq);
                if(c != '\n' ){
                    mapaAuxiliar[i][j] = c;
                    if(mapaAuxiliar[i][j]=='J'){
                        posicaoAtual->x = posInicial->x = j; //Coloca o player na posicao inicial
                        posicaoAtual->y = posInicial->y = i;
                    }
                    else if (mapaAuxiliar[i][j] == 'T'){
                        toupeiras[toupeirasAtualmente] = IniciaToupeira(j, i);
                        toupeirasAtualmente++;
                    }
                    else if(mapaAuxiliar[i][j] == 'E'){
                        (*esmeraldas)++; // Diz quantas esmeraldas tem na rodada
                    }
                }

            }
        }
        fclose(arq);
    }

    camera->target.x = posicaoAtual->x * ARESTA -20;
    camera->target.y = posicaoAtual->y * ARESTA;
    camera->offset.x = 100;
    camera->offset.y = 100;
    camera->rotation = 0;
    camera->zoom = 6;
    return toupeirasAtualmente;
}

int jogadorPerdeVida(JOGADOR* jogador, char mapaAuxiliar[][LARGURAMAPA], TOUPEIRA toupeiras [], int toupeirasNaRodada){
    int i, j;
    int vidas = jogador->vidas;
    Sound dano = LoadSound("Sons/squirtleDano.mp3");
    if(!IsSoundPlaying(dano))
        PlaySound(dano);
    jogador->vidas--;
    if (jogador->vidas == 0)
        jogador->sairJogo = 1;
    else{
        for(i = 0; i < toupeirasNaRodada; i++){
            if(toupeiras[i].vida != 0){
                toupeiras[i].posicaoAtual.x = toupeiras[i].posInicial.x;
                toupeiras[i].posicaoAtual.y = toupeiras[i].posInicial.y;
            }
        }
        jogador->posicaoAtual.x = jogador->posInicial.x;
        jogador->posicaoAtual.y = jogador->posInicial.y;
    }
}

int toupeiraNoLugar(TOUPEIRA toupeiras [], JOGADOR jogador, int toupeirasNaRodada){
    int i, posX, posY, posXToupeira, posYToupeira;
    posX = jogador.posicaoAtual.x;
    posY = jogador.posicaoAtual.y;
    for(i = 0; i < toupeirasNaRodada; i++){
        posXToupeira = toupeiras[i].posicaoAtual.x;
        posYToupeira = toupeiras[i].posicaoAtual.y;
        if(posX == posXToupeira && posY == posYToupeira)
            return 1;
    }
    return 0;
}


/* Funcao PodeAndar
    Verifica se a posicao para qual o jogador tentou se deslocar eh uma posicao valida
    ou nao, comparando com o mapa
*/
void PodeAndar(JOGADOR* jogador, char mapaAuxiliar[][LARGURAMAPA], TOUPEIRA toupeiras [], int toupeirasNaRodada, Sound esmeralda, Sound moeda){
    int x, y;

    x = jogador->posicaoAtual.x + jogador->deslocamento.x; // Verifica qual o local para qual o player quer se movimentar
    y = jogador->posicaoAtual.y + jogador->deslocamento.y;

    if(mapaAuxiliar[y][x] != '#' && mapaAuxiliar[y][x] != 'S'){ //Se for diferente de # ele pode se mexer
        jogador->posicaoAtual.x = x;
        jogador->posicaoAtual.y = y;
        if(mapaAuxiliar[y][x] == 'O'){ // ouro
            mapaAuxiliar[y][x] = ' ';
            jogador->pontos+= 50;
            PlaySound(moeda);
        }
        else if(mapaAuxiliar[y][x] == 'E'){ // esmeralda
            mapaAuxiliar[y][x] = ' ';
            jogador->esmeraldas--;
            jogador->pontos+= 100;
            PlaySound(esmeralda);
        }
        else if(mapaAuxiliar[y][x] == 'A'){ // powerup
            mapaAuxiliar[y][x] = ' ';
            jogador->powerUp = 1;
        }
        else if(toupeiraNoLugar(toupeiras, *jogador, toupeirasNaRodada)){
            jogadorPerdeVida(jogador, mapaAuxiliar, toupeiras, toupeirasNaRodada);
        }

    }
    jogador->deslocamento.x = 0; //Reinicia o deslocamento para poder andar de novo
    jogador->deslocamento.y = 0;
}

/* Funcao SalvaEstado
    Salva o jogo assim que o jogador apertar a tecla */
int SalvaEstado(JOGADOR *jogador, TOUPEIRA toupeiras [], char nomeArquivo[], int ToupeirasNaRodada, char MapaAuxiliar[][LARGURAMAPA]){
    FILE *arq;
    int i, j, retorno = 1;
    if (!(arq = fopen(nomeArquivo,"wb")))
        retorno = 0;
    else{
        if(!(fwrite(jogador,sizeof(JOGADOR),1,arq))){        // Escreve as informações da struct jogador no arquivo texto. Trocar fwrite por fprintf
            retorno = 0;
            printf("\nErro para escrever infos do jogador");
        }
        for(i=0;i<ALTURAMAPA;i++){
            for(j=0;j<LARGURAMAPA;j++){
                if(!(fwrite(&MapaAuxiliar[i][j],sizeof(char),1,arq))){      // Escreve cada char da matriz MapaAuxiliar um por um se fwrite resultar em 1
                    retorno = 0;
                    printf("Erro de escrita na altura %d e largura %d", i, j);
                }
            }
        }
        for (i=0;i<ToupeirasNaRodada;i++){
            if(!(fwrite(&toupeiras[i],sizeof(TOUPEIRA),1,arq))){        // Escreve as informações das structs toupeiras uma por uma. Caso de errado, diz em qual toupeira deu ruim.
                retorno = 0;
                printf("\nErro para escrever infos da toupeira %d", i);
            }
        }
    }
    fclose(arq);
    return retorno;
}

/* Funcao leEstado eh responsavel por ler o arquivo de salvamento e possibilitar o carregamento de um save antigo do jogador */
int leEstado(JOGADOR *jogador, TOUPEIRA toupeiras [], char nomeArquivo[], int ToupeirasNaRodada, char MapaAuxiliar[][LARGURAMAPA]){
    FILE *arq;
    int retorno = 1, i, j;
    if (!(arq=fopen(nomeArquivo,"rb"))){
        retorno = 0;
        printf("\nErro para abrir arquivo binario dentro da funcao leEstado");
    }
    else{
        if(!(fread(jogador,sizeof(JOGADOR),1,arq))){
            retorno = 0;
            printf("\nErro para ler a struct jogador do arquivo binario.");
        }
        for(i=0;i<ALTURAMAPA;i++){
            for(j=0;j<LARGURAMAPA;j++){
                if(!(fread(&MapaAuxiliar[i][j],sizeof(char),1,arq))){
                    retorno = 0;
                    printf("\nErro para ler o Mapa auxiliar dentro da funcao leEstado.");
                }
            }
        }
        for(i=0;i<ToupeirasNaRodada;i++){
            if(!(fread(&toupeiras[i],sizeof(TOUPEIRA),1,arq))){
                retorno = 0;
                printf("\nErro para ler a toupeira %d dentro da funcao leEstado.",i);
            }
        }


    }
    fclose(arq);
    return retorno;
}

/* Funcao MovimentoJogador
    Verifica se alguma tecla foi pressionada e chama a funcao PodeAndar pra permitir
    o deslocamento
*/
void MovimentoJogador(JOGADOR* jogador, char mapaAuxiliar[][LARGURAMAPA], TOUPEIRA toupeiras [], int toupeirasNaRodada, Sound esmeralda, Sound moeda){
    if(IsKeyPressed(KEY_RIGHT)){
        jogador->deslocamento.x += 1;
        jogador->direcaoAtual = 1; // direita
    }
    else if(IsKeyPressed(KEY_LEFT)){
        jogador->deslocamento.x -= 1;
        jogador->direcaoAtual = 2; // esquerda
    }
    else if(IsKeyPressed(KEY_UP)){
        jogador->deslocamento.y -= 1;
        jogador->direcaoAtual = 3; // cima
    }
    else if(IsKeyPressed(KEY_DOWN)){
        jogador->deslocamento.y += 1;
        jogador->direcaoAtual = 4; // baixo
    }
    PodeAndar(jogador, mapaAuxiliar, toupeiras, toupeirasNaRodada, esmeralda, moeda);
}

/*
A toupeira anda em uma direcao dando ate 5 passos, se bater numa parede ou o contador chegar
ate 5 ela muda de direcao
*/
void MoveToupeira(TOUPEIRA toupeiras [], int toupeirasNaRodada, char mapaAuxiliar[][LARGURAMAPA]){
    int i,x,y;
    for(i = 0; i < toupeirasNaRodada; i++){
        do{
            if(toupeiras[i].contadorDirecao >= 5){ // Se a toupeira ja andou 5, ganha uma direcao nova, considerando o livre arbitrio
                toupeiras[i].direcaoAtual = (rand()%4);
                toupeiras[i].contadorDirecao = 0;

            }
            switch(toupeiras[i].direcaoAtual){ // Dependendo da direcao atual, ela anda uma vez nessa direcao
                case 0: // esquerda
                    toupeiras[i].deslocamento.x = toupeiras[i].posicaoAtual.x - 1;
                    toupeiras[i].deslocamento.y = toupeiras[i].posicaoAtual.y;
                    break;
                case 1: // direita
                    toupeiras[i].deslocamento.x = toupeiras[i].posicaoAtual.x + 1;
                    toupeiras[i].deslocamento.y = toupeiras[i].posicaoAtual.y;
                    break;
                case 2: // cima
                    toupeiras[i].deslocamento.y = toupeiras[i].posicaoAtual.y - 1;
                    toupeiras[i].deslocamento.x = toupeiras[i].posicaoAtual.x;
                    break;
                case 3: // baixo
                    toupeiras[i].deslocamento.y = toupeiras[i].posicaoAtual.y + 1;
                    toupeiras[i].deslocamento.x = toupeiras[i].posicaoAtual.x;
                    break;
            }
            toupeiras[i].contadorDirecao++; //incrementa o contador de direcao da toupeira
            x = toupeiras[i].deslocamento.x;
            y = toupeiras[i].deslocamento.y;

        }while (mapaAuxiliar[y][x] == '#'); //enquanto nao for uma parede, ela pode andar


        toupeiras[i].posicaoAtual.x = toupeiras[i].deslocamento.x; // Se deu tudo certo, ela se desloca pra posicao nova
        toupeiras[i].posicaoAtual.y = toupeiras[i].deslocamento.y; //
        toupeiras[i].deslocamento.x = 0; //
        toupeiras[i].deslocamento.y = 0; //

    }

}

/* Funcao TirosJogador
Cria o tiro considerando a posicao do jogador e move ele na direcao que o personagem esta virado
*/
void TirosJogador(JOGADOR jogador, TIRO* tiro){
    if(jogador.tiro.umTiroPorVez == 0){ //Se nao tem um tiro ainda, pode atirar
        tiro->direcaoAtual = jogador.direcaoAtual; // Determina a direcao do tiro
        tiro->posicaoAtual.x = jogador.posicaoAtual.x; // Determina a posicao atual do tiro
        tiro->posicaoAtual.y = jogador.posicaoAtual.y; //
        tiro->umTiroPorVez = 1; // Garante que nao seja atirado outro tiro
    }
}

void MoveTiro(JOGADOR jogador, TIRO* tiro, char mapaAuxiliar[][LARGURAMAPA],int toupeirasNaRodada, TOUPEIRA toupeiras [], int* pontos, Sound bidoof){
    int x, y, yExtra = 0, xExtra = 0, i, matouToupeira = 0;
    if(jogador.tiro.umTiroPorVez == 1){ // Se existe um tiro no mapa
        x = jogador.tiro.posicaoAtual.x;
        y = jogador.tiro.posicaoAtual.y;

        switch (jogador.tiro.direcaoAtual){ // Verifica em que direcao este tiro esta indo
            case 1: // direita
                x += 1;
                xExtra = 1;
                break;
            case 2: // esquerda
                x -= 1;
                xExtra = -1;
                break;
            case 3: // cima
                y -= 1;
                yExtra = -1;
                break;
            case 4: // baixo
                y += 1;
                yExtra = 1;
                break;
        }

        if(mapaAuxiliar[y][x] == '#'){ // Se nao tiver obstaculos na frente, o tiro pode se mover
            tiro->umTiroPorVez = 0;
        }
        else if(mapaAuxiliar[y][x] == 'S'){
            mapaAuxiliar[y][x] = ' ';
            tiro->umTiroPorVez = 0;
        }
        else if(mapaAuxiliar[y][x] != '#'){
            for (i=0; i < toupeirasNaRodada; i++){
                if (toupeiras[i].posicaoAtual.x == x && toupeiras[i].posicaoAtual.y == y){
                    toupeiras[i].vida = 0;   // descanse em paz nobre guerrreiro
                    toupeiras[i].posicaoAtual.x = 0;
                    toupeiras[i].posicaoAtual.y = 0;
                    tiro->umTiroPorVez = 0;
                    matouToupeira = 1;
                    *pontos+= 200;
                    PlaySound(bidoof);
                }
            }
            if(matouToupeira != 1){
                tiro->posicaoAtual.x += xExtra;
                tiro->posicaoAtual.y += yExtra;
            }
        }


    }
}
int NevoaDestruicao(JOGADOR jogador, int x, int y, char mapaAuxiliar[][LARGURAMAPA]){
    int distanciaX = (x - jogador.posicaoAtual.x);
    int distanciaY = (y - jogador.posicaoAtual.y);
    int distanciaXTiro = (x - jogador.tiro.posicaoAtual.x);
    int distanciaYTiro = (y - jogador.tiro.posicaoAtual.y);
    int i;

    if(jogador.powerUp == 1)
        return 1;
    if(distanciaX <= 2 && distanciaX >= -2 && distanciaY <= 2 && distanciaY >= -2){
        return 1;
    }
    else if(distanciaXTiro <= 1 && distanciaXTiro >= -1 && distanciaYTiro <= 1 && distanciaYTiro >= -1){
        return 1;
    }
    return 0;
}


/* Funcao DesenhaMapa
Desenha o mapa considerando a modificacao nas posicoes de todos os elementos
*/
void DesenhaMapa(char mapaAuxiliar[][LARGURAMAPA], JOGADOR jogador, TOUPEIRA toupeiras[], int toupeirasNoMapa, IMAGENS imagensMapa[]){
    int i, j; //Para percorrer o mapa
    int x, y; // Quadrados do jogo
    int toupeiraX, toupeiraY;

    Vector2 pos;
    //Desenha a grama de fora
    for(i = 0; i < ALTURAMAPA+10; i++){
        for(j = 0; j < LARGURAJANELA+18; j++){
            pos.x = j*ARESTA + POSICAOMAPA - 160;
            pos.y = i*ARESTA + POSICAOMAPA - 80;
            if(NevoaDestruicao(jogador, pos.x/20-1, pos.y/20-1, mapaAuxiliar)){
                DrawTextureEx(imagensMapa[0].imagem, pos, 0, ESCALA, WHITE);
            }
            else{
                DrawTextureEx(imagensMapa[0].imagem, pos, 0, ESCALA, WHITE);
                DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
            }
        }
    }


    for(i = 0; i < ALTURAMAPA; i++){
        for(j = 0; j < LARGURAMAPA; j++){

            x = POSICAOMAPA + j*ARESTA; // Tamanho de um quadrado do jogo
            y = POSICAOMAPA + i*ARESTA; //
            pos.x = x;
            pos.y = y;


            switch(mapaAuxiliar[i][j]){ // Desenha os objetos em suas posicoes
                case 'J':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'T':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }

                    break;
                case '#':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar)){
                        if(i == 19 && j == 0)
                                DrawTextureEx(imagensMapa[7].imagem, pos, 0, ESCALA, WHITE);
                        else if(i == 0 && j == 0)
                            DrawTextureEx(imagensMapa[8].imagem, pos, 0, ESCALA, WHITE);
                        else if(i == 0 && j == 29)
                            DrawTextureEx(imagensMapa[10].imagem, pos, 0, ESCALA, WHITE);
                        else if(i == 0)
                            DrawTextureEx(imagensMapa[9].imagem, pos, 0, ESCALA, WHITE);
                        else if(j == 0)
                            DrawTextureEx(imagensMapa[1].imagem, pos, 0, ESCALA, WHITE);
                        else if(i == 19 && j == 29)
                            DrawTextureEx(imagensMapa[13].imagem, pos, 0, ESCALA, WHITE);
                        else if(i == 19)
                            DrawTextureEx(imagensMapa[12].imagem, pos, 0, ESCALA, WHITE);
                        else if(j == 29)
                            DrawTextureEx(imagensMapa[14].imagem, pos, 0, ESCALA, WHITE);
                        else
                            DrawTextureEx(imagensMapa[15].imagem, pos, 0, ESCALA, WHITE);
                    }else{
                        if(i == 19 && j == 0){
                            DrawTextureEx(imagensMapa[7].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(i == 0 && j == 0){
                            DrawTextureEx(imagensMapa[8].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(i == 0 && j == 29){
                            DrawTextureEx(imagensMapa[10].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(i == 0){
                            DrawTextureEx(imagensMapa[9].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(j == 0){
                            DrawTextureEx(imagensMapa[1].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(i == 19 && j == 29){
                            DrawTextureEx(imagensMapa[13].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(i == 19){
                            DrawTextureEx(imagensMapa[12].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else if(j == 29){
                            DrawTextureEx(imagensMapa[14].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                        else{
                            DrawTextureEx(imagensMapa[15].imagem, pos, 0, ESCALA, WHITE);
                            DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                        }
                    }
                    break;
                case 'S':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[11].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[11].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'O':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[17].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }

                    break;
                case 'E':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[18].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'A':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[16].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case ' ':
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
            }
        }

    }

    pos.x = jogador.posicaoAtual.x*ARESTA + POSICAOMAPA;
    pos.y = jogador.posicaoAtual.y*ARESTA + POSICAOMAPA;
    // Desenha o jogador
    switch(jogador.direcaoAtual){
        case 1: //direita
            DrawTextureEx(imagensMapa[3].imagem, pos, 0, ESCALA, WHITE);
            break;
        case 2://esquerda
            DrawTextureEx(imagensMapa[5].imagem, pos, 0, ESCALA, WHITE);
            break;
        case 3://cima
            DrawTextureEx(imagensMapa[2].imagem, pos, 0, ESCALA, WHITE);
            break;
        case 4://baixo
            DrawTextureEx(imagensMapa[6].imagem, pos, 0, ESCALA, WHITE);
            break;

    }
    //

    // Desenha o tiro
    if(jogador.tiro.umTiroPorVez == 1){

        switch(jogador.tiro.direcaoAtual){
            case 1:
                DrawRectangle(jogador.tiro.posicaoAtual.x*ARESTA + POSICAOMAPA + 15, jogador.tiro.posicaoAtual.y*ARESTA + POSICAOMAPA + ARESTA/4 ,ARESTA/2, ARESTA/4, SKYBLUE);
                break;
            case 2:
                DrawRectangle(jogador.tiro.posicaoAtual.x*ARESTA + POSICAOMAPA - 10, jogador.tiro.posicaoAtual.y*ARESTA + POSICAOMAPA + ARESTA/4 ,ARESTA/2, ARESTA/4, SKYBLUE);
                break;
            case 3:
                DrawRectangle(jogador.tiro.posicaoAtual.x*ARESTA + POSICAOMAPA + ARESTA/4, jogador.tiro.posicaoAtual.y*ARESTA + POSICAOMAPA - 10,ARESTA/4, ARESTA/2, SKYBLUE);
                break;
            case 4:
                DrawRectangle(jogador.tiro.posicaoAtual.x*ARESTA + POSICAOMAPA + ARESTA/4, jogador.tiro.posicaoAtual.y*ARESTA + POSICAOMAPA + 10,ARESTA/4, ARESTA/2, SKYBLUE);
                break;
        }
    }
    //

    // Desenha toupeiras
    for(i = 0; i < toupeirasNoMapa; i++){
        if(toupeiras[i].vida==1){

            toupeiraX = toupeiras[i].posicaoAtual.x;
            toupeiraY = toupeiras[i].posicaoAtual.y;
            Vector2 posTop;
            posTop.x = toupeiraX*ARESTA + POSICAOMAPA;
            posTop.y = toupeiraY*ARESTA + POSICAOMAPA;
            if(NevoaDestruicao(jogador, toupeiraX, toupeiraY, mapaAuxiliar)){
                if(mapaAuxiliar[toupeiraY][toupeiraX] != 'S' || jogador.powerUp == 1){
                    if(toupeiras[i].direcaoAtual == 0) //direita
                        DrawTextureEx(imagensMapa[19].imagem, posTop, 0, ESCALA, WHITE);
                    else if(toupeiras[i].direcaoAtual == 1) // esquerda
                        DrawTextureEx(imagensMapa[20].imagem, posTop, 0, ESCALA, WHITE);
                    else if(toupeiras[i].direcaoAtual == 2) // cima
                        DrawTextureEx(imagensMapa[23].imagem, posTop, 0, ESCALA, WHITE);
                    else if(toupeiras[i].direcaoAtual == 3) // baixo
                        DrawTextureEx(imagensMapa[22].imagem, posTop, 0, ESCALA, WHITE);
                }


                else
                    DrawTextureEx(imagensMapa[11].imagem, posTop, 0, ESCALA, WHITE);
            }
            else if(mapaAuxiliar[toupeiraY][toupeiraX] == 'S'){
                DrawTextureEx(imagensMapa[11].imagem, posTop, 0, ESCALA, WHITE);
                DrawRectangle(posTop.x, posTop.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
            }
            else{
                DrawTextureEx(imagensMapa[4].imagem, posTop, 0, ESCALA, WHITE);
                DrawRectangle(posTop.x, posTop.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
            }
        }
    }

}


/* Funcao RodadaTerminou
Verifica se a rodada terminou, caso o jogador obtenha todas as esmeraldas
*/
int RodadaTerminou(JOGADOR jogador){
    // if esmeraldas = x, return 1 algo assim
    if(jogador.esmeraldas == 0)
        return 1;
    return 0;
}

int DesenhaMenu(char mapaAuxiliar[][LARGURAMAPA], JOGADOR jogador, TOUPEIRA toupeiras[], int toupeirasNoMapa, IMAGENS imagensMapa [], Camera2D camera){
    float posX = camera.target.x;
    float posY = camera.target.y;
    int tamanhoLetra = 10;
    int tamanhoTitulo = 30;
    char titulo[30] = "JOGO PAUSADO";
    char n[30] = "(N)Novo Jogo";
    char c[30] = "(C)Carregar Jogo";
    char s[30] = "(S)Salvar Jogo";
    char q[30] = "(Q)Sair do Jogo";
    char v[30] = "(V)Voltar";
    DesenhaMapa(mapaAuxiliar, jogador, toupeiras, toupeirasNoMapa, imagensMapa);
    if(jogador.powerUp == 1)
        DrawRectangle(camera.target.x - camera.offset.x, camera.target.y- camera.offset.y, 5000, 5000, CLITERAL(Color){ 160, 255, 255, 128 });
    else
        DrawRectangle(camera.target.x - camera.offset.x/6, camera.target.y -camera.offset.y/6, 1000, 1000, CLITERAL(Color){ 160, 255, 255, 128 });
    DrawText(titulo, posX - (MeasureText(titulo, tamanhoTitulo)/2) , posY - camera.offset.y/8, tamanhoTitulo, RED);
    DrawText(n, posX - (MeasureText(n, tamanhoLetra)/2) , posY - camera.offset.y/18, tamanhoLetra, BLACK);
    DrawText(c, posX - (MeasureText(c, tamanhoLetra)/2), posY - camera.offset.y/40, tamanhoLetra, BLACK);
    DrawText(s, posX - (MeasureText(s, tamanhoLetra)/2), posY , tamanhoLetra, BLACK);
    DrawText(q, posX - (MeasureText(q, tamanhoLetra)/2), posY + camera.offset.y/40, tamanhoLetra, BLACK);
    DrawText(v, posX - (MeasureText(v, tamanhoLetra)/2), posY + camera.offset.y/10, tamanhoLetra, BLACK);
}

int MenuPrincipal(JOGADOR* jogador, Texture2D* titulo, Sound musicaTitulo){
    int sairLoop, Q = 1;

    float scaleWidth = (float)GetScreenWidth() / titulo->width;
    float scaleHeight = (float)GetScreenHeight() / titulo->height;

    float escala = (scaleWidth < scaleHeight) ? scaleWidth : scaleHeight;

    int imageWidth = (int)(titulo->width * escala);
    int imageHeight = (int)(titulo->height * escala);

    Vector2 posFundo;
    posFundo.x = (GetScreenWidth() - imageWidth) / 2;
    posFundo.y = (GetScreenHeight() - imageHeight) / 2;

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTextureEx(*titulo, posFundo, 0, escala, WHITE);

    if (IsKeyPressed(KEY_Q)) {
        Q = 1;
        jogador->sairJogo = 1;
    }
    if (IsKeyPressed(KEY_N))
        Q = 0;

    if(!IsSoundPlaying(musicaTitulo))
    {
        PlaySound(musicaTitulo);
    }

    EndDrawing();


    return Q;
}

void MostraPontos(Camera2D camera, JOGADOR jogador){
    int tamanhoLetra = 7;
    DrawText(TextFormat("%dF",(jogador.mapaAtual+1)),camera.target.x-(GetScreenHeight()/8), camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("HP %d/3",jogador.vidas),camera.target.x-(GetScreenHeight()/12), camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("Apples: %d",jogador.esmeraldas),camera.target.x - 10, camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("Score: %d",jogador.pontos),camera.target.x+ 60, camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
}

void AtualizaCamera(JOGADOR jogador, Camera2D* camera){
    camera->target.x = jogador.posicaoAtual.x * ARESTA +30;
    camera->target.y = jogador.posicaoAtual.y * ARESTA +30;
    camera->offset.x = GetScreenWidth()/2;
    camera->offset.y = GetScreenHeight()/2;
}

void CarregaImagens(IMAGENS imagens[]){
    int i;
    char imagemAtual [50] = {'\0'};
    for(i = 0;i < IMAGENSTOTAL; i++){
        sprintf(imagemAtual, "texturas/imagem%d.png", i);
        imagens[i].imagem = LoadTexture(imagemAtual);
    }
}


void FadeIn(){
    int i = 0;
    while(i < 255){
        BeginDrawing();
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CLITERAL(Color){ 0, 0, 0, i });
        i++;
        EndDrawing();
    }
}


int main(){
    // Mapa auxiliar para save do jogo
    char mapaAuxiliar [ALTURAMAPA][LARGURAMAPA];
    //
    Vector2 posFlash;
    Camera2D camera;
    // Toupeiras presentes no mapa atual e velocidade dos objetos
    int toupeirasNaRodada, velocidade, temporizadorPowerUp, Q = 1;
    //
    Vector2 nevoa;
    InitAudioDevice();

    // Toupeiras presentes no jogo
    TOUPEIRA toupeiras[MAXTOUPEIRAS];
    //
    IMAGENS imagensMapa[IMAGENSTOTAL];
    // Nome do arquivo para salvamento do jogo
    char arquivoSalvamento [] = "EstadoDoJogo";
    //

    // Numeros aleatorios
    srand((unsigned)time(NULL));
    //

    //Inicia a janela do jogo
    InitWindow(GetScreenWidth(), GetScreenHeight(), "POKEMINE");
    //
    SetWindowIcon(LoadImage("Texturas/icone.png"));
    // Coloca o jogo para 60 FPS
    SetTargetFPS(60);
    //
    // Cria um jogador novo
    JOGADOR jogador = {0};
    jogador.direcaoAtual = 1;
    jogador.pontos = 0;
    jogador.vidas = 3;
    jogador.esmeraldas = 0;
    jogador.mapaAtual = 0;
    jogador.primeiraVezRodadando = 0;
    //
    CarregaImagens(imagensMapa);

    Sound bidoof = LoadSound("Sons/somBidoof.mp3");
    Sound musicaTitulo = LoadSound("Sons/MusicaTitulo.mp3");
    Sound somAmbiente = LoadSound("Sons/MusicaMapa1.mp3");
    Sound derrotaMus = LoadSound("Sons/musicaderrota.mp3");
    Sound ataque = LoadSound("Sons/squirtleAtaque.mp3");
    Sound esmeralda = LoadSound("Sons/esmeraldaSom.mp3");
    Sound moeda = LoadSound("Sons/moeda.mp3");
    Sound umaVida = LoadSound("Sons/1vida.mp3");
    //
    Texture2D derrota = LoadTexture("Texturas/ImagemFim.png");
    Texture2D titulo = LoadTexture("Texturas/titulo.png");
    Texture2D flash = LoadTexture("Texturas/flash.png");
    while(!WindowShouldClose() && Q == 1 && jogador.sairJogo != 1)
        Q = MenuPrincipal(&jogador, &titulo, musicaTitulo);
    //
    StopSound(musicaTitulo);

    // Loop principal do jogo
    while(!WindowShouldClose() && Q != 1 && jogador.sairJogo != 1){
    //
        if(RodadaTerminou(jogador)){
            FadeIn();
            StopSound(somAmbiente);
            toupeirasNaRodada = IniciaRodada(&jogador.posInicial, &jogador.posicaoAtual, toupeiras, mapaAuxiliar, &jogador.esmeraldas, jogador.mapaAtual, &camera, jogador);
            velocidade = temporizadorPowerUp = 0;
        }
        else{
            if(jogador.jogoPausado == 0){
                //------------------------------------------
                BeginDrawing();       // Comeca o desenho
                //------------------------------------------
                BeginMode2D(camera);
                //-------------------------------------------------------------
                ClearBackground(RAYWHITE); //Limpa o desenho do frame anterior
                //-------------------------------------------------------------

                //--------------------------------------------------------------------------------
                DesenhaMapa(mapaAuxiliar, jogador, toupeiras, toupeirasNaRodada, imagensMapa);//Desenha o mapa
                //--------------------------------------------------------------------------------
                MostraPontos(camera, jogador);
                //---------------------------------------------------------------------------
                MovimentoJogador(&jogador, mapaAuxiliar, toupeiras, toupeirasNaRodada, esmeralda, moeda);// Cuida da movimentacao do jogador
                //---------------------------------------------------------------------------

                if(jogador.vidas!= 1){
                    if(!IsSoundPlaying(somAmbiente))
                        PlaySound(somAmbiente);
                }
                else{
                    StopSound(somAmbiente);
                    if(!IsSoundPlaying(umaVida))
                        PlaySound(umaVida);
                }

                AtualizaCamera(jogador, &camera);
                //-------------------------------------------------------------------------------------------
                if(velocidade%(SEGUNDO/2) == 0) //
                    MoveToupeira(toupeiras, toupeirasNaRodada, mapaAuxiliar); // Funcao que move as toupeiras
                //--------------------------------------------------------------------------------------------

                //-----------------------------------------------------------------------------------
                if (IsKeyPressed(KEY_G)){// Se aperta G
                    if(!IsSoundPlaying(ataque))
                        PlaySound(ataque);
                    TirosJogador(jogador, &jogador.tiro); // Posiciona o tiro e verifica a direcao
                }
                //-----------------------------------------------------------------------------------

                //---------------------------------------------------------------------------------------------------------------
                if(velocidade%(SEGUNDO/30) == 0)// velocidade alta
                    MoveTiro(jogador, &jogador.tiro, mapaAuxiliar, toupeirasNaRodada, toupeiras, &jogador.pontos, bidoof); // move o tiro e verifica colisoes, caso haja um tiro no mapa
                //---------------------------------------------------------------------------------------------------------------

                //----------------------------------------------
                if(jogador.powerUp == 1){
                    posFlash.x = camera.target.x - flash.width * 0.05;;
                    posFlash.y = camera.target.y + flash.height/10;
                    DrawTextureEx(flash, posFlash, 0, 0.1, WHITE);
                    temporizadorPowerUp++;
                    camera.zoom = 5.5;
                    if(temporizadorPowerUp == SEGUNDO*3){
                        jogador.powerUp = temporizadorPowerUp = 0;
                        camera.zoom = 6.0;
                    }
                }

                //-----------------------------------------------
                if(jogador.tiro.umTiroPorVez == 0){
                    jogador.tiro.posicaoAtual.x = jogador.posicaoAtual.x;
                    jogador.tiro.posicaoAtual.y = jogador.posicaoAtual.y;
                }

                if(IsKeyPressed(KEY_TAB))
                    jogador.jogoPausado = 1;
                //------------------------------------------
                EndDrawing();   //   Termina o desenho
                //------------------------------------------
                EndMode2D();

                if(RodadaTerminou(jogador))
                    jogador.mapaAtual++;



                velocidade++; // variavel que mantem controle da velocidade do tiro, das toupeiras e do power up

            }
            else{
                //------------------------------------------
                BeginDrawing();       // Comeca o desenho
                //------------------------------------------
                BeginMode2D(camera);
                //-------------------------------------------------------------
                ClearBackground(RAYWHITE); //Limpa o desenho do frame anterior
                //-------------------------------------------------------------
                MostraPontos(camera, jogador);
                //-------------------------------------------------------------
                DesenhaMenu(mapaAuxiliar, jogador, toupeiras, toupeirasNaRodada, imagensMapa, camera);
                //-------------------------------------------------------------

                if(IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_V))
                    jogador.jogoPausado = 0;
                if (IsKeyPressed(KEY_S)){
                    if (SalvaEstado(&jogador,toupeiras,arquivoSalvamento,toupeirasNaRodada,mapaAuxiliar)== 1)
                        printf("\nJogo foi salvo.");
                    else
                        printf("\nJogo foi salvo incorretamente");
                }
                if (IsKeyPressed(KEY_C)){
                    if (leEstado(&jogador,toupeiras,arquivoSalvamento,toupeirasNaRodada,mapaAuxiliar)== 1)
                        printf("\nArquivo foi lido corretamente.");
                    else
                        printf("\nArquivo nao foi lido incorretamente");
                }
                if (IsKeyPressed(KEY_Q))
                    Q = 1;
                if (IsKeyPressed(KEY_N)){
                    jogador.mapaAtual = 0;
                    jogador.direcaoAtual = 1;
                    jogador.pontos = 0;
                    jogador.esmeraldas = 0;
                    jogador.vidas = 3;
                    toupeirasNaRodada = IniciaRodada(&jogador.posInicial, &jogador.posicaoAtual, toupeiras, mapaAuxiliar, &jogador.esmeraldas, jogador.mapaAtual, &camera, jogador);
                }
                AtualizaCamera(jogador, &camera);

                //------------------------------------------
                EndDrawing();   //   Termina o desenho
                //------------------------------------------

            }


        }
    }
    StopSound(somAmbiente);
    while (!WindowShouldClose() && jogador.vidas == 0) {
        BeginDrawing();
        int centroX = GetScreenWidth() / 2;
        int centroY = GetScreenHeight() / 2;
        ClearBackground(BLACK);
        if(!IsSoundPlaying(derrotaMus))
            PlaySound(derrotaMus);

        Vector2 posicao;
        posicao.x = centroX - (derrota.width / 2); //
        posicao.y = centroY - (derrota.height / 2);

        //
        DrawTextureEx(derrota, posicao, 0, 1, WHITE);
        DrawText(TextFormat("PONTOS: %d",jogador.pontos),posicao.x+600, posicao.y+600, 40, WHITE);
        EndDrawing();
}
    CloseAudioDevice();
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Termina o programa
    //-----------------------------------------------------------------------------------

    return 0;
}
