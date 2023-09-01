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


/* Tipo Vetor2
    Um vetor com x e y
*/
typedef struct Vetor2{
    int x;
    int y;
}Vetor2;


/* Tipo Tiro
    O tiro do jogador. Possui um Vetor2 que indica a posicao do tiro, um numero que indica a direcao e outro inteiro que indica se ha um tiro
    sendo atirado ou nao.
*/
typedef struct Tiro{
    Vetor2 posicaoAtual;
    int direcaoAtual;
    int umTiroPorVez;
}TIRO;


/* Tipo Jogador
    Basicamente o player, possuindo tres Vetor2, um para a posicao atual, outro
    para indicar o deslocamento que o player deseja fazer e outro para guardar a posicao no player para o caso
    de saves. Alem disso, um contador de vidas, um inteiro que indica a direcao para qual o jogador esta olhando, um contador
    de quantas esmeraldas faltam, um membro da struct TIRO, um inteiro que indica se o power-up esta ativo ou nao, um indicador do mapa atual,
    uma variavel que indica se o jogo esta pausado e uma variavel que indica a necessidade de sair do jogo.
*/
typedef struct Jogador{
    Vetor2 posInicial; //Vetor2 armazena posicao inicial naquele mapa
    Vetor2 posicaoAtual; //Vetor2 armazena a atual posicao x e y do jogador.
    Vetor2 deslocamento; //Vetor2 armazena como o jogador quer se movimentar
    int vidas; // vidas do jogador
    int direcaoAtual;// direcao para qual ele esta se movimentando
    int pontos; // pontuacao do jogador
    int esmeraldas; // esmeraldas restantes
    TIRO tiro; // TIRO
    int powerUp; // powerup presente
    int mapaAtual; // mapa atual
    int jogoPausado; // jogo pausado
    int sairJogo; // variavel pra sair do jogo
}JOGADOR;


/* Tipo Toupeira
    Inimigos do jogo, possuem 2 Vetor2 um com a posicao atual e outro com a posicao inicial no mapa, um contador de vida, um contador de direcao
    para limitar os 5 passos, um inteiro que indica a direcao atual e um Vetor2 que indica o deslocamento que a toupeira deseja fazer.
*/
typedef struct Toupeira{
    Vetor2 posicaoAtual;
    Vetor2 posInicial;
    int vida;
    int contadorDirecao;
    int direcaoAtual;
    Vetor2 deslocamento;
} TOUPEIRA;


/* Tipo Imagens
    Exatamente um Texture2D, mas abrasileirado
*/
typedef struct Imagens{
    Texture2D imagem;
} IMAGENS;


/* Funcao IniciaToupeiras
    Paramentros: posicao x no vetor e posicao y no vetor
    Utilizada na funcao IniciaRodada, ela inicializa todas toupeiras no inicio do jogo.
*/
TOUPEIRA IniciaToupeira(int x, int y){
    TOUPEIRA auxiliar;
    auxiliar.posicaoAtual.x = auxiliar.posInicial.x = x;
    auxiliar.posicaoAtual.y = auxiliar.posInicial.y = y;
    auxiliar.vida = 1;
    auxiliar.direcaoAtual = 1;
    return auxiliar;
}


/* Funcao GanhouOJogo
    Paramentros: jogador
    Desenha tela de vitoria quando jogo chega ao fim, quando o contador de esmeraldas chega ate 0 e nao existem mais mapas.
*/
void GanhouOJogo(JOGADOR jogador){
    Sound vitoriaMus = LoadSound("Sons/vitoria.mp3");
    Texture2D vitoria = LoadTexture("Texturas/vitoria.png");
    while(!WindowShouldClose()){ // So sai dessa janela com esc ou fechando a aba
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


    CloseWindow();        // Termina o programa


}


/* Funcao IniciaRodada
    Parametros: posicao inicial do jogador, posicao atual do jogador, array de toupeiras, matriz do mapa auxiliar(utilizado para alterar valores, como
    ouros coletados e etc), esmeraldas, mapa da rodada, camera e jogador
    Percorre o mapa desejado e atribui a todos os objetos suas posicoes atuais baseando-se
    no mapa atual. Alem disso, confere a fase atual e carrega o mapa desejado e faz uma copia para o mapa auxiliar, para que ele possa ser atualizado e modificado
*/
int IniciaRodada(Vetor2* posInicial,Vetor2* posicaoAtual, TOUPEIRA toupeiras[], char mapaAuxiliar[][LARGURAMAPA], int* esmeraldas, int mapaAtual, Camera2D* camera, JOGADOR jogador){
    FILE *arq;
    int i,j, toupeirasAtualmente = 0;
    char c, numeroMapa[1], mapaDaRodada [50] = {'\0'};
    *esmeraldas = 0;
    numeroMapa[0] = '0' + (mapaAtual + 1);
    strcat(mapaDaRodada, "mapas/mapa");
    strcat(mapaDaRodada, numeroMapa);
    strcat(mapaDaRodada, ".txt"); // metodo utilizado para transformar o numero do mapa em string e concatenar com o a string do mapa para abrir o arquivo
    if(!(arq = fopen(mapaDaRodada,"r")))
        GanhouOJogo(jogador);
    else{
        for (i = 0; i < ALTURAMAPA;i++){
            for(j = 0; j < LARGURAMAPA+1;j++){
                c = getc(arq);
                if(c != '\n' ){
                    mapaAuxiliar[i][j] = c;
                    if(mapaAuxiliar[i][j]=='J'){
                        posicaoAtual->x = posInicial->x = j; //Coloca o player na posicao inicial e salva a inicial
                        posicaoAtual->y = posInicial->y = i;
                    }
                    else if (mapaAuxiliar[i][j] == 'T'){ // inicializa as toupeiras
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

    //Inicializa a camera
    camera->target.x = posicaoAtual->x * ARESTA -20;
    camera->target.y = posicaoAtual->y * ARESTA;
    camera->offset.x = 100;
    camera->offset.y = 100;
    camera->rotation = 0;
    camera->zoom = 6;

    return toupeirasAtualmente;
}


/* Funcao jogadorPerdeVida
    Parametros: jogador, mapa auxiliar, array de toupeiras e contador de toupeiras na rodada
    Tira vida, toca som de dano se toupeira encostar e averigua se vidas chegaram em 0.
*/
int jogadorPerdeVida(JOGADOR* jogador, char mapaAuxiliar[][LARGURAMAPA], TOUPEIRA toupeiras [], int toupeirasNaRodada){
    int i, j;
    int vidas = jogador->vidas;
    Sound dano = LoadSound("Sons/squirtleDano.mp3");
    if(!IsSoundPlaying(dano))
        PlaySound(dano);
    jogador->vidas--;
    if (jogador->vidas == 0)
        jogador->sairJogo = 1; //flag pra sair do jogo
    else{
        for(i = 0; i < toupeirasNaRodada; i++){ // toupeiras voltam pra posicao inicial
            if(toupeiras[i].vida != 0){
                toupeiras[i].posicaoAtual.x = toupeiras[i].posInicial.x;
                toupeiras[i].posicaoAtual.y = toupeiras[i].posInicial.y;
            }
        }
        jogador->posicaoAtual.x = jogador->posInicial.x; //jogador volta pra posicao inicial
        jogador->posicaoAtual.y = jogador->posInicial.y;
    }
}

/* Funcao toupeiraNoLugar
    Parametros: array de toupeiras, jogador e contador de toupeiras na rodada
    Funcao responsavel por verificar se o jogador e a toupeira estao na mesma posicao
*/
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
    ou nao, comparando com o mapa. Tambem toca os sons e atualiza variaveis como esmeraldas, pontos e o power up
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
        else if(toupeiraNoLugar(toupeiras, *jogador, toupeirasNaRodada)){ //verifica se jogador ta com uma toupeira no lugar
            jogadorPerdeVida(jogador, mapaAuxiliar, toupeiras, toupeirasNaRodada);
        }

    }
    jogador->deslocamento.x = 0; //Reinicia o deslocamento para poder andar de novo
    jogador->deslocamento.y = 0;
}

/* Funcao SalvaEstado
    Parametros: jogador, array de toupeiras, string com o nome do arquivo, numero de toupeiras na rodada, matriz do mapa auxiliar
    Salva o jogo assim que o jogador apertar a tecla S durante o pause
*/
int SalvaEstado(JOGADOR *jogador, TOUPEIRA toupeiras [], char nomeArquivo[], int ToupeirasNaRodada, char MapaAuxiliar[][LARGURAMAPA]){
    FILE *arq;
    int i, j, retorno = 1;
    if (!(arq = fopen(nomeArquivo,"wb")))
        retorno = 0;
    else{
        if(!(fwrite(jogador,sizeof(JOGADOR),1,arq))){        // Escreve as informacoes da struct jogador no arquivo texto.
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
            if(!(fwrite(&toupeiras[i],sizeof(TOUPEIRA),1,arq))){        // Escreve as informacoes das structs toupeiras uma por uma. Caso de errado, diz em qual toupeira deu ruim.
                retorno = 0;
                printf("\nErro para escrever infos da toupeira %d", i);
            }
        }
    }
    fclose(arq);
    return retorno;
}

/* Funcao leEstado
    Parametros: jogador, vetor de toupeiras, string com o nome do arquivo, numero de toupeiras na rodada e matriz do mapa auxiliar
    eh responsavel por ler o arquivo de salvamento e possibilitar o carregamento de um save antigo do jogador
*/
int leEstado(JOGADOR *jogador, TOUPEIRA toupeiras [], char nomeArquivo[], int ToupeirasNaRodada, char MapaAuxiliar[][LARGURAMAPA]){
    FILE *arq;
    int retorno = 1, i, j;
    if (!(arq=fopen(nomeArquivo,"rb"))){
        retorno = 0;
        printf("\nErro para abrir arquivo binario dentro da funcao leEstado");
    }
    else{ // caso tenha conseguido abrir
        if(!(fread(jogador,sizeof(JOGADOR),1,arq))){    // Le do arquivo informacoes do tamanho da struct jogador. As armazena na struct jogador.
            retorno = 0;
            printf("\nErro para ler a struct jogador do arquivo binario.");
        }
        for(i=0;i<ALTURAMAPA;i++){
            for(j=0;j<LARGURAMAPA;j++){
                if(!(fread(&MapaAuxiliar[i][j],sizeof(char),1,arq))){ // Le do arquivo informacoes do tamanho de char e guarda uma a uma dentro da matriz MapaAuxiliar
                    retorno = 0;
                    printf("\nErro para ler o Mapa auxiliar dentro da funcao leEstado.");
                }
            }
        }
        for(i=0;i<ToupeirasNaRodada;i++){
            if(!(fread(&toupeiras[i],sizeof(TOUPEIRA),1,arq))){ // Le do arquivo informacoes do tamanho da struct toupeira e guarda uma por uma em uma toupeira.
                retorno = 0;
                printf("\nErro para ler a toupeira %d dentro da funcao leEstado.",i);
            }
        }
    }
    fclose(arq);
    return retorno;
}

/* Funcao MovimentoJogador
    Parametros: jogador, matriz mapa auxiliar, array vetor, contador de toupeiras na rodada, som das esmeraldas/macas e som da moeda
    Verifica se alguma tecla foi pressionada e chama a funcao PodeAndar pra permitir o deslocamento
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

/* Funcao MoveToupeira
    Paramemetros: array de toupeiras, contador de toupeiras na rodada e matriz do mapa auxiliar
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
    Parametros: jogador e tiro
    Cria o tiro considerando a posicao do jogador e move ele na direcao que o personagem esta virado(a move tiro que move)
*/
void TirosJogador(JOGADOR jogador, TIRO* tiro){
    if(jogador.tiro.umTiroPorVez == 0){ //Se nao tem um tiro ainda, pode atirar
        tiro->direcaoAtual = jogador.direcaoAtual; // Determina a direcao do tiro
        tiro->posicaoAtual.x = jogador.posicaoAtual.x; // Determina a posicao atual do tiro
        tiro->posicaoAtual.y = jogador.posicaoAtual.y; //
        tiro->umTiroPorVez = 1; // Garante que nao seja atirado outro tiro
    }
}

/* Funcao MoveTiro
    Parametros: jogador, tiro, matriz do mapa, contador de toupeiras, array de toupeiras, pontos, som do bidoof
    Responsavel por mover o tiro e detectar se ele bate ou nao em algum obstaculo, e verifica a morte das toupeiras
*/
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
            if(matouToupeira != 1){ // se nao matou a toupeira o tiro anda
                tiro->posicaoAtual.x += xExtra;
                tiro->posicaoAtual.y += yExtra;
            }
        }


    }
}

/* Funcao Nevoa
    Parametros: jogador, posicao x e posicao y do bloco desejado e matriz do mapa auxiliar
    Compara o quao perto o bloco esta do jogador, se estiver longe devolve 0 e sera colocado a nevoa sobre o objeto
*/
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
    Parametros: matriz do mapa auxiliar, jogador, array de toupeiras, contador de toupeiras, array com todas as imagens do mapa
    Desenha o mapa considerando a modificacao nas posicoes de todos os elementos e considerando a nevoa
*/
void DesenhaMapa(char mapaAuxiliar[][LARGURAMAPA], JOGADOR jogador, TOUPEIRA toupeiras[], int toupeirasNoMapa, IMAGENS imagensMapa[]){
    int i, j; //Para percorrer o mapa
    int x, y; // Quadrados do jogo
    int toupeiraX, toupeiraY;
    Vector2 pos, posTop;
    //Desenha a grama de fora
    for(i = 0; i < ALTURAMAPA+10; i++){
        for(j = 0; j < LARGURAJANELA+18; j++){
            pos.x = j*ARESTA + POSICAOMAPA - 160;
            pos.y = i*ARESTA + POSICAOMAPA - 80;
            if(NevoaDestruicao(jogador, pos.x/20-1, pos.y/20-1, mapaAuxiliar)){ // se tiver perto desenha normal
                DrawTextureEx(imagensMapa[0].imagem, pos, 0, ESCALA, WHITE);
            }
            else{
                DrawTextureEx(imagensMapa[0].imagem, pos, 0, ESCALA, WHITE); //nevoa
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
                case 'J': //posicao onde o jogador nasce
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'T': // posicao onde as toupeiras nascem
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case '#': // paredes do mapa
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar)){ // se esta dentro do campo de visao
                        if(i == 19 && j == 0) // cada posicao da parede exterior tem uma textura propria
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
                            DrawTextureEx(imagensMapa[15].imagem, pos, 0, ESCALA, WHITE); // arvores
                    }else{ //nevoa
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
                case 'S': // troncos (quebraveis) eles tem sombra e o resto nao!
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[11].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[11].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'O': //ouro (Moeda)
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[17].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'E': //esmeraldas (Macas, snorlax adora!)
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[18].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case 'A': //poderzinho (HM FLASH)
                    if(NevoaDestruicao(jogador, j, i, mapaAuxiliar))
                        DrawTextureEx(imagensMapa[16].imagem, pos, 0, ESCALA, WHITE);
                    else{
                        DrawTextureEx(imagensMapa[4].imagem, pos, 0, ESCALA, WHITE);
                        DrawRectangle(pos.x, pos.y, ARESTA, ARESTA, CLITERAL(Color){ 0, 0, 0, 128 });
                    }
                    break;
                case ' ': // chao
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

    if(jogador.tiro.umTiroPorVez == 1){ // Desenha o tiro, considerando a direcao para sair da boca do squirtle
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
    for(i = 0; i < toupeirasNoMapa; i++){// Desenha toupeiras
        if(toupeiras[i].vida==1){
            toupeiraX = toupeiras[i].posicaoAtual.x;
            toupeiraY = toupeiras[i].posicaoAtual.y;
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
    Parametros: jogador
    Verifica se a rodada terminou, caso o jogador obtenha todas as esmeraldas
*/
int RodadaTerminou(JOGADOR jogador){
    // if esmeraldas = x, return 1 algo assim
    if(jogador.esmeraldas == 0)
        return 1;
    return 0;
}

/* Funcao DesenhaMenu
    Parametros: matriz do mapa auxiliar, jogador, array de toupeiras, contador de toupeiras, array de imagens e camera
    Funcao que desenha menu da tecla TAB
*/
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
    if(jogador.powerUp == 1) // arruma a tela pro power
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

/* Funcao MenuPrincipal
    Parametros: jogador, imagem do titulo e musica do titulo
    Funcao do menu principal (o inicial)
*/
int MenuPrincipal(JOGADOR* jogador, Texture2D* titulo, Sound musicaTitulo){
    int sairLoop, Q = 1;
    float larguraEscala = (float)GetScreenWidth() / titulo->width;
    float alturaEscala = (float)GetScreenHeight() / titulo->height;
    float escala = (larguraEscala < alturaEscala) ? larguraEscala : alturaEscala;
    int larguraImagem = (int)(titulo->width * escala);
    int alturaImagem = (int)(titulo->height * escala);
    Vector2 posFundo;
    posFundo.x = (GetScreenWidth() - larguraImagem) / 2;
    posFundo.y = (GetScreenHeight() - alturaImagem) / 2;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTextureEx(*titulo, posFundo, 0, escala, WHITE);
    if (IsKeyPressed(KEY_Q)) { // tecla 'q' pra sair do jogo
        Q = 1;
        jogador->sairJogo = 1;
    }
    if (IsKeyPressed(KEY_N)) // tecla 'n' pra novo jogo
        Q = 0;
    if(!IsSoundPlaying(musicaTitulo)){
        PlaySound(musicaTitulo);
    }
    EndDrawing();
    return Q;
}

/* Funcao MostraPontos
    Parametros: camera e jogador
    Funcao que mostra os pontos ao longo do jogo
*/
void MostraPontos(Camera2D camera, JOGADOR jogador){
    int tamanhoLetra = 7;
    DrawText(TextFormat("%dF",(jogador.mapaAtual+1)),camera.target.x-(GetScreenHeight()/8), camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("HP %d/3",jogador.vidas),camera.target.x-(GetScreenHeight()/12), camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("Apples: %d",jogador.esmeraldas),camera.target.x - 10, camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
    DrawText(TextFormat("Score: %d",jogador.pontos),camera.target.x+ 60, camera.target.y-(GetScreenWidth()/22), tamanhoLetra, RED);
}

/* Funcao MostraPontos
    Parametros: camera e jogador
    Funcao que mostra os pontos ao longo do jogo
*/
void AtualizaCamera(JOGADOR jogador, Camera2D* camera){
    camera->target.x = jogador.posicaoAtual.x * ARESTA +30;
    camera->target.y = jogador.posicaoAtual.y * ARESTA +30;
    camera->offset.x = GetScreenWidth()/2;
    camera->offset.y = GetScreenHeight()/2;
}

/* Funcao MostraPontos
    Parametros: camera e jogador
    Funcao que carrega todas as imagens do jogo
*/
void CarregaImagens(IMAGENS imagens[]){
    int i;
    char imagemAtual [50] = {'\0'};
    for(i = 0;i < IMAGENSTOTAL; i++){
        sprintf(imagemAtual, "texturas/imagem%d.png", i);
        imagens[i].imagem = LoadTexture(imagemAtual);
    }
}

/* Funcao FadeIn
    Parametros:
    Desenha uma quadradro pra transicao do jogo
*/
void FadeIn(){
    int i = 0;
    while(i < 255){ // podia ser um for, podia...
        BeginDrawing();
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CLITERAL(Color){ 0, 0, 0, i });
        i++;
        EndDrawing();
    }
}

/* Funcao main
    Parametros:
    Funcao main, declaracao de todas as variaveis praticamente, mais loop principal do jogo e varias chamadas de funcoes
*/
int main(){
    char mapaAuxiliar [ALTURAMAPA][LARGURAMAPA];// Mapa auxiliar para save do jogo
    Vector2 posFlash; // Inicializa o vetor flash.
    Camera2D camera; // Inicializa a camera do jogo.
    int toupeirasNaRodada, velocidade, temporizadorPowerUp, Q = 1;// Toupeiras presentes no mapa atual e velocidade dos objetos
    Vector2 nevoa; // Inicializa o vetor nevoa.
    InitAudioDevice();
    TOUPEIRA toupeiras[MAXTOUPEIRAS];// Toupeiras presentes no jogo
    IMAGENS imagensMapa[IMAGENSTOTAL];
    char arquivoSalvamento [] = "EstadoDoJogo";// Nome do arquivo para salvamento do jogo
    srand((unsigned)time(NULL));   // Numeros aleatorios
    InitWindow(GetScreenWidth(), GetScreenHeight(), "POKEMINE");  //Inicia a janela do jogo
    SetWindowIcon(LoadImage("Texturas/icone.png"));
    SetTargetFPS(60);// Coloca o jogo para 60 FPS
    // Cria um jogador novo
    JOGADOR jogador = {0};
    jogador.direcaoAtual = 1; // Definimos uma direcao padrao na inicializacao.
    jogador.pontos = 0;
    jogador.vidas = 3;
    jogador.esmeraldas = 0;
    jogador.mapaAtual = 0;
    // Aqui eh feito o carregamento de algumas imagens e sons que serao usados posteriormente no jogo.
    CarregaImagens(imagensMapa);
    Sound bidoof = LoadSound("Sons/somBidoof.mp3");
    Sound musicaTitulo = LoadSound("Sons/MusicaTitulo.mp3");
    Sound somAmbiente = LoadSound("Sons/MusicaMapa1.mp3");
    Sound derrotaMus = LoadSound("Sons/musicaderrota.mp3");
    Sound ataque = LoadSound("Sons/squirtleAtaque.mp3");
    Sound esmeralda = LoadSound("Sons/esmeraldaSom.mp3");
    Sound moeda = LoadSound("Sons/moeda.mp3");
    Sound umaVida = LoadSound("Sons/1vida.mp3");
    Texture2D derrota = LoadTexture("Texturas/ImagemFim.png");
    Texture2D titulo = LoadTexture("Texturas/titulo.png");
    Texture2D flash = LoadTexture("Texturas/flash.png");
    //--------------------------------------------------------------
    while(!WindowShouldClose() && Q == 1 && jogador.sairJogo != 1) // Aqui eh feito o menu inicial do jogo.
        Q = MenuPrincipal(&jogador, &titulo, musicaTitulo);
    StopSound(musicaTitulo);
    //------------------------------------------
    // Loop principal do jogo
    //------------------------------------------
    while(!WindowShouldClose() && Q != 1 && jogador.sairJogo != 1){
        /*Aqui eh feita a transicao no fim da rodada*/
        if(RodadaTerminou(jogador)){
            jogador.powerUp = 0;
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
                BeginMode2D(camera); // Inicia a camera que foca onde o jogador esta
                //-------------------------------------------------------------
                ClearBackground(RAYWHITE); //Limpa o desenho do frame anterior
                //--------------------------------------------------------------------------------
                DesenhaMapa(mapaAuxiliar, jogador, toupeiras, toupeirasNaRodada, imagensMapa);//Desenha o mapa
                //--------------------------------------------------------------------------------
                MostraPontos(camera, jogador); // funcao que mostra os pintos na tela
                //---------------------------------------------------------------------------
                MovimentoJogador(&jogador, mapaAuxiliar, toupeiras, toupeirasNaRodada, esmeralda, moeda);// Cuida da movimentacao do jogador
                //---------------------------------------------------------------------------

                /*Aqui eh definido qual musica ambiente sera tocada de acordo com a vida do jogador.*/
                if(jogador.vidas!= 1){
                    if(!IsSoundPlaying(somAmbiente))
                        PlaySound(somAmbiente);
                }
                else{
                    StopSound(somAmbiente);
                    if(!IsSoundPlaying(umaVida))
                        PlaySound(umaVida);
                }
                //----------------------------------------------------------------------------------
                AtualizaCamera(jogador, &camera); // Atualiza a camera que segue o jogador
                //-----------------------------------------------------------------------------------
                if(velocidade%(SEGUNDO/2) == 0) //
                    MoveToupeira(toupeiras, toupeirasNaRodada, mapaAuxiliar); // Funcao que move as toupeiras
                //-----------------------------------------------------------------------------------
                if (IsKeyPressed(KEY_G)){// Se aperta G, toca som de ataque e tiro do jogador eh disparado
                    if(!IsSoundPlaying(ataque))
                        PlaySound(ataque);
                    TirosJogador(jogador, &jogador.tiro); // Posiciona o tiro e verifica a direcao
                }
                //-----------------------------------------------------------------------------------
                if(velocidade%(SEGUNDO/30) == 0)// velocidade alta
                    MoveTiro(jogador, &jogador.tiro, mapaAuxiliar, toupeirasNaRodada, toupeiras, &jogador.pontos, bidoof); // move o tiro e verifica colisoes, caso haja um tiro no mapa
                //----------------------------------------------
                if(jogador.powerUp == 1){ // temporizador e posicionador do desenho durante o powerUp
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
                if(jogador.tiro.umTiroPorVez == 0){ // Se nao houver tiro sendo disparado, a posicao inicial do futuro disparo eh atualizada constantemente
                    jogador.tiro.posicaoAtual.x = jogador.posicaoAtual.x;
                    jogador.tiro.posicaoAtual.y = jogador.posicaoAtual.y;
                }
                if(IsKeyPressed(KEY_TAB))
                    jogador.jogoPausado = 1;  // Abre menu.
                //------------------------------------------
                EndDrawing();   //   Termina o desenho
                //------------------------------------------
                EndMode2D(); // Termina a camera
                //--------------------------------------------
                if(RodadaTerminou(jogador)){// Aqui eh checado se a rodada terminou. Se for positivo, proximo mapa é indicado.
                    jogador.mapaAtual++;
                    jogador.powerUp = 0;
                }

                velocidade++; // variavel que mantem controle da velocidade do tiro, das toupeiras e do power up
            }
            else{
                //------------------------------------------
                BeginDrawing();       // Comeca o desenho
                //------------------------------------------
                BeginMode2D(camera);  // Inicializa a camera
                //-------------------------------------------------------------
                ClearBackground(RAYWHITE); //Limpa o desenho do frame anterior
                //-------------------------------------------------------------
                MostraPontos(camera, jogador); // Mostra pontos no pause
                //-------------------------------------------------------------
                DesenhaMenu(mapaAuxiliar, jogador, toupeiras, toupeirasNaRodada, imagensMapa, camera);
                //-------------------------------------------------------------
                if(IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_V)) // Se jogo esta pausado, V despausa. Apertar TAB pausa o jogo.
                    jogador.jogoPausado = 0;
                //-------------------------------------------------------------
                if (IsKeyPressed(KEY_S)){ // Aqui eh feito o salvamento do jogo
                    if (SalvaEstado(&jogador,toupeiras,arquivoSalvamento,toupeirasNaRodada,mapaAuxiliar)== 1)
                        printf("\nJogo foi salvo.");
                    else
                        printf("\nJogo foi salvo incorretamente");
                }
                //-------------------------------------------------------------
                if (IsKeyPressed(KEY_C)){ // Aqui eh feita a parte de carrregamento do jogo
                    if (leEstado(&jogador,toupeiras,arquivoSalvamento,toupeirasNaRodada,mapaAuxiliar)== 1)
                        printf("\nArquivo foi lido corretamente.");
                    else
                        printf("\nArquivo nao foi lido incorretamente");
                }
                //-------------------------------------------------------------
                if (IsKeyPressed(KEY_Q)) // Testa se Q foi pressionado, se sim fecha o jogo.
                    Q = 1;
                //-------------------------------------------------------------
                if (IsKeyPressed(KEY_N)){ // Se apertar N, novo jogo eh carregado e alguns valores tem que ser alterados para bom funcionamento do jogo.
                    jogador.mapaAtual = 0;
                    jogador.direcaoAtual = 1;
                    jogador.pontos = 0;
                    jogador.esmeraldas = 0;
                    jogador.vidas = 3;
                    toupeirasNaRodada = IniciaRodada(&jogador.posInicial, &jogador.posicaoAtual, toupeiras, mapaAuxiliar, &jogador.esmeraldas, jogador.mapaAtual, &camera, jogador);
                }
                //-----------------------------------------------------------------------------------------------//
                AtualizaCamera(jogador, &camera);// A camera se baseia na posicao do jogador, entao eh necessario atualizar ela constantemente
                //-----------------------------------------------------------------------------------------------//
                EndDrawing();   //   Termina o desenho
            }
        }
    }
    StopSound(somAmbiente);
    StopSound(umaVida);
    while (!WindowShouldClose() && jogador.vidas == 0) { // Aqui eh desenhado a tela final caso o jogador seja derrotado.
        BeginDrawing();
        int centroX = GetScreenWidth() / 2;
        int centroY = GetScreenHeight() / 2;
        ClearBackground(BLACK);
        if(!IsSoundPlaying(derrotaMus))
            PlaySound(derrotaMus);
        Vector2 posicao;
        posicao.x = centroX - (derrota.width / 2); //
        posicao.y = centroY - (derrota.height / 2);
        DrawTextureEx(derrota, posicao, 0, 1, WHITE);
        DrawText(TextFormat("PONTOS: %d",jogador.pontos),posicao.x+600, posicao.y+600, 40, WHITE);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();        // Termina o programa
    return 0;
}
