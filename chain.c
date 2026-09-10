#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_ENEMIES 15
#define TIRO_INATIVO 0
#define TIRO_ATIVO 1
#define RAIO_TIRO 100
#define TEMPO_TIRO 2.0
#define TIRO_RECARREGANDO 2
#define BORDA_TIRO 4
#define SCORE_PENALTY 0.25

const float FPS = 100;

const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int HERO_W = 30;
const int HERO_H = 40;


//para a imagem
ALLEGRO_BITMAP *imagem_de_fundo = NULL;


ALLEGRO_COLOR BKG_COLOR;
ALLEGRO_FONT *FONT_32;

typedef struct Tiro {
int x, y;
float raio;
int modo;
float timer;
ALLEGRO_COLOR cor;
} Tiro;

typedef struct Ship {
int x, y;
int vel;
ALLEGRO_COLOR cor;
Tiro tiro;
} Ship;

typedef struct Hero {

Ship ship;
int dir_x;
int dir_y;
float score;
} Hero;


typedef struct Enemy {

        Ship ship;
        float raio;
        int active;
        int explosao;
        float raio_da_explosao;
        float tempo_de_explosao;
} Enemy;


void initTiro(Ship *s) {

//printf("\nInit tiro!");

s->tiro.x = s->x;
s->tiro.y = s->y;
s->tiro.raio = 3;
s->tiro.cor = s->cor;
s->tiro.timer = TEMPO_TIRO;
s->tiro.modo = TIRO_INATIVO;

}

void initGlobals() {
BKG_COLOR = al_map_rgb(10, 10, 10);
//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
FONT_32 = al_load_font("arial.ttf", 32, 1);  
}


/*-----------------funcao para abrir o arquivo-------*/


int carregaRecorde() {
    FILE *arquivo = fopen("recorde.txt", "r");
    int recorde = 0;
    
    
    if (arquivo != NULL) {
        fscanf(arquivo, "%d", &recorde);
        fclose(arquivo);
    }
    return recorde;
}

/*-----------funcao salva recorde---------------*/

void salvaRecorde(int novo_recorde) {
    FILE *arquivo = fopen("recorde.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", novo_recorde);
        fclose(arquivo);
    }
}


void initHero(Hero *s) {

  s->score = 0;
  s->ship.cor = al_map_rgb(100 + rand()%156, 100 + rand()%156, 100 + rand()%156);
  s->ship.x = SCREEN_W/2;
  s->ship.y = SCREEN_H - HERO_H - 10;
  s->ship.vel = 1;
  s->dir_x = 0;
  s->dir_y = 0;
  initTiro(&s->ship);

}

void initEnemy( Enemy *inimigo)
{
  inimigo->raio = 10 + rand() % 25; /* 35 PIXELS 10+25*/
  inimigo->ship.x = 0 + rand() % (SCREEN_W-35);
  inimigo->ship.y = -35;
  inimigo->explosao = 0;
  inimigo->raio_da_explosao = 0;
  inimigo->tempo_de_explosao = 0;
/*velocidades variadas*/
 if (inimigo->raio <= 10)
    inimigo->ship.vel = 1 + (rand() % 4);
 else
    inimigo->ship.vel = 1 + (rand() % 3);

  /*ativador de inimigos*/
  inimigo->active = 0;

  inimigo->ship.cor =al_map_rgb(100 + rand() % 156, rand() % 156, rand() % 156);
 }


//desenha a imagem na tela
void drawScenario(Hero s) {

   al_draw_scaled_bitmap(imagem_de_fundo, 
   0, 0, al_get_bitmap_width(imagem_de_fundo), al_get_bitmap_height(imagem_de_fundo), //resolusao da foto

   0, 0, SCREEN_W, SCREEN_H,                                     
   0);                                                           
   

  char score_txt[5];
}


void drawHero(Hero *s) {


al_draw_filled_triangle(s->ship.x, s->ship.y, s->ship.x - HERO_W/2, s->ship.y + HERO_H, s->ship.x + HERO_W/2, s->ship.y + HERO_H, s->ship.cor);

char score_txt[5];
sprintf(score_txt, "%d", (int)s->score);
//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
al_draw_text(FONT_32, s->ship.cor, 100, 20, 0, score_txt);

al_draw_circle(s->ship.tiro.x, s->ship.tiro.y, s->ship.tiro.raio, s->ship.tiro.cor, BORDA_TIRO);


}


void updateVilans( Enemy vilans[])
{
   for(int i = 0; i < NUM_ENEMIES; i++)
   {
     if ( vilans[i].active == 1)
     {
        if (vilans[i].explosao == 0)
        {
           vilans[i].ship.y += vilans[i].ship.vel;
           if (vilans[i].ship.y > SCREEN_H + 40)
           {
             initEnemy(&vilans[i]);
             vilans[i].active = 1; 
           } 
        }
        else
        {
           vilans[i].tempo_de_explosao -= 1.0/FPS;
           if( vilans[i].raio_da_explosao < vilans[i].raio)
           vilans[i].raio_da_explosao += 0.5;
           if (vilans[i].tempo_de_explosao <= 0) 
           {
               initEnemy(&vilans[i]);
               vilans[i].active = 1;
            }
        }
    }
  }
}


void updateHero(Hero *s) {


s->ship.x += s->dir_x * s->ship.vel;

s->ship.y += s->dir_y * s->ship.vel;

/*limites*/
     if ( s->ship.x < 40)
        s->ship.x = 40;
     if (s->ship.x > (SCREEN_W - HERO_W))
        s->ship.x = SCREEN_W - HERO_W;
     if ( s->ship.y < 0)
        s->ship.y = 0;
     if (s->ship.y > (SCREEN_H - HERO_H))
        s->ship.y = SCREEN_H - HERO_H;  
    /*limites*/
       
if(s->ship.tiro.modo == TIRO_ATIVO) {
     if(s->ship.tiro.timer > 0){
        s->ship.tiro.timer -= 1.0/FPS;
         }

     else  {
       s->ship.tiro.modo = TIRO_RECARREGANDO;
       s->ship.tiro.timer = TEMPO_TIRO;
       s->ship.tiro.raio = 0;
       }
       }
 else if(s->ship.tiro.modo == TIRO_RECARREGANDO)
      {
   s->ship.tiro.timer -= 1.0 / FPS;
     if (s->ship.tiro.timer <= 0)
      {
         initTiro(&s->ship);
      }
}
    else if (s->ship.tiro.modo == TIRO_INATIVO)
    {
      s->ship.tiro.x = s->ship.x;
      s->ship.tiro.y = s->ship.y;
      s->ship.tiro.raio = 0;
     }
   }
 


    void drawVilan( Enemy vilans[])
    {
     for (int i = 0; i < 15; i++)
     {
        if ( vilans[i].active == 1)
          {
            if (vilans[i].explosao == 0)
   {
            al_draw_filled_circle(
   vilans[i].ship.x,
   vilans[i].ship.y,
   vilans[i].raio,
   vilans[i].ship.cor);
   }
            else
            {
            al_draw_circle(
            vilans[i].ship.x,
            vilans[i].ship.y,
            vilans[i].raio_da_explosao,
            vilans[i].ship.cor,
            2.0);
           }
     }
   }
 }


int checaColisao(int x_enemy, int y_enemy, float raio_enemy, int x_hero, int y_hero, float raio_hero)
{
   int distancia_x = x_hero - x_enemy;
   int distancia_y = y_hero - y_enemy;

                  /*Pitagoras*/


   float distancia = sqrt((distancia_x * distancia_x) +(distancia_y * distancia_y));
   if( distancia <= (raio_enemy + raio_hero)-10)
   {
     return 1;
   }
   else
     return 0;
}


/*---------------funcao main-------------------------*/


int main(int argc, char **argv){

//srand(time(NULL));

int i;
int recorde_quebrado =0;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_SAMPLE *musica_de_fundo = NULL;


//inicializa o audio
al_install_audio();
al_init_acodec_addon();
//4 sons
al_reserve_samples(4);

musica_de_fundo = al_load_sample("title_music.ogg");




   
//----------------------- rotinas de inicializacao ---------------------------------------



   
//inicializa o Allegro
if(!al_init()) {
fprintf(stderr, "failed to initialize allegro!\n");
return -1;
}

    //inicializa o módulo de primitivas do Allegro
     if(!al_init_primitives_addon()){
fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

   
//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
       fprintf(stderr, "failed to create timer!\n");
       return -1;
}
 
 
//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
     display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
    fprintf(stderr, "failed to create display!\n");
    al_destroy_timer(timer);
    return -1;
}

//instala o teclado
if(!al_install_keyboard()) {
fprintf(stderr, "failed to install keyboard!\n");
return -1;
}

//inicializa o modulo allegro que carrega as fontes
al_init_font_addon();

//inicializa o modulo allegro que carrega as imagens
al_init_image_addon();

//inicializa o modulo allegro que entende arquivos tff de fontes
if(!al_init_ttf_addon()) {	
fprintf(stderr, "failed to load tff font module!\n");
return -1;
}

//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);  
if(size_32 == NULL) {
fprintf(stderr, "font file does not exist or cannot be accessed!\n");
}

  //cria a fila de eventos
event_queue = al_create_event_queue();
if(!event_queue) {
fprintf(stderr, "failed to create event_queue!\n");
al_destroy_display(display);
return -1;
}
   


//registra na fila os eventos de tela (ex: clicar no X na janela)
al_register_event_source(event_queue, al_get_display_event_source(display));
//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
al_register_event_source(event_queue, al_get_timer_event_source(timer));
//registra na fila os eventos de teclado (ex: pressionar uma tecla)
al_register_event_source(event_queue, al_get_keyboard_event_source());


//----------------------jogo-------------------

//inicializa globais
initGlobals();

//Cria o heroi
Hero Heroi;
initHero(&Heroi);
 
int recorde_atual = carregaRecorde();
int redesenhar = 0;

//cria os inimigos:
                               /*Minha edicao enemy*/
        Enemy vilans[NUM_ENEMIES];
        for (int i = 0; i < NUM_ENEMIES; i++)
        {
           initEnemy(&vilans[i]);
         }
         

//inicia o temporizador
al_start_timer(timer);

int playing = 1;
int frames_contados = 0;
int proximo_vilao = 0;


//Toca a música de fundo
al_play_sample(musica_de_fundo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

imagem_de_fundo = al_load_bitmap("background.jpg");

while(playing) {
  ALLEGRO_EVENT ev;
  //espera por um evento e o armazena na variavel de evento ev
  al_wait_for_event(event_queue, &ev);

//se o tipo de evento for o fechamento da tela (clique>
  if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
  playing = 0;
  }
//se o tipo de evento for um pressionar de uma tecla
  else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
//imprime qual tecla foi
//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

switch(ev.keyboard.keycode) {
  //se a tecla for o W
  case ALLEGRO_KEY_W:
   Heroi.dir_y = -1;
   break;

  case ALLEGRO_KEY_S:
   Heroi.dir_y = 1;
   break;

  case ALLEGRO_KEY_A:
    Heroi.dir_x = -1;
    break;

  case ALLEGRO_KEY_D:
    Heroi.dir_x = 1;
    break;

  case ALLEGRO_KEY_SPACE:
    if(Heroi.ship.tiro.modo == TIRO_INATIVO) {
      Heroi.ship.tiro.modo = TIRO_ATIVO;
      Heroi.ship.tiro.raio = RAIO_TIRO;
      Heroi.ship.tiro.timer = TEMPO_TIRO;
}
      break;
  case ALLEGRO_KEY_Z:
     if((Heroi.ship.tiro.modo == TIRO_INATIVO) && (Heroi.score >= 100)) {
       Heroi.ship.tiro.modo = TIRO_ATIVO;
       Heroi.ship.tiro.raio = RAIO_TIRO * 2; // SUPER TIRO com o dobro do tamanho
       Heroi.ship.tiro.timer = TEMPO_TIRO;
       Heroi.score -= 100; // Custo do super_tiro
  }
  break;
}
}




else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
//imprime qual tecla foi
//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

switch(ev.keyboard.keycode) {
//se a tecla for o W

  case ALLEGRO_KEY_W:
    Heroi.dir_y = 0;
    break;

  case ALLEGRO_KEY_S:
    Heroi.dir_y = 0;
    break;

  case ALLEGRO_KEY_A:
    Heroi.dir_x = 0;
    break;

  case ALLEGRO_KEY_D:
    Heroi.dir_x = 0;
    break;
/*
case ALLEGRO_KEY_SPACE:
if(Hero.tiro.modo == TIRO_HOLDING)
Hero.tiro.modo = TIRO_ATIVO;
break;
*/
}
}




  //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
  if(ev.type == ALLEGRO_EVENT_TIMER) {
        
      updateHero(&Heroi);
      updateVilans(vilans);
    
    Heroi.score -= SCORE_PENALTY; //Penalidade
    if(Heroi.score < 0)
    Heroi.score = 0;


    if (proximo_vilao < NUM_ENEMIES)
    {
    frames_contados++;
 
      if (frames_contados >= 250) /*2,5 segundos*/
{
      vilans[proximo_vilao].active = 1; /*ativado*/
      proximo_vilao++;
      frames_contados = 0;
      }
      }
     
/*-----------------checando se o vilao colidiu------*/


                if(Heroi.ship.tiro.modo == TIRO_ATIVO)
               {
                  int colisao = 0;
                  for( int i = 0; i < NUM_ENEMIES; i++)
                  {
                     if((vilans[i].active == 1) &&
                          (vilans[i].explosao == 0))
                     {
                        colisao = checaColisao(
                                   Heroi.ship.tiro.x,
                                   Heroi.ship.tiro.y,
                                   Heroi.ship.tiro.raio,
                                   vilans[i].ship.x,
                                   vilans[i].ship.y,
                                   vilans[i].raio);

                          if (colisao == 1)
                          {
                            vilans[i].tempo_de_explosao = 2.0;
                            vilans[i].explosao = 1;
                            vilans[i].raio_da_explosao = 1.0;
                            Heroi.score = Heroi.score + (vilans[i].raio*2);
                           
                           }
                       } 
                  }
               }

               
                for( int i = 0; i < NUM_ENEMIES; i++)
                  {
                  if ((vilans[i].active == 1) && (vilans[i].explosao == 1))
                  {
                     for( int j = 0; j < NUM_ENEMIES; j++)
                     {
                        if (i != j && vilans[j].active == 1 && vilans[j].explosao == 0)
                        {
                            int colisao_cadeia = checaColisao(
                                vilans[i].ship.x,
                                vilans[i].ship.y,
                                vilans[i].raio_da_explosao,
                                vilans[j].ship.x,
                                vilans[j].ship.y,
                                vilans[j].raio);
                           if (colisao_cadeia == 1)
                           {
                               vilans[j].explosao = 1;
                               vilans[j].raio_da_explosao = 1.0;
                               vilans[j].tempo_de_explosao = 2.0;
                               Heroi.score = Heroi.score + (vilans[j].raio * 2);
                           }
                        }        
                     }
                  }
                } 
                     for( int k = 0; k < NUM_ENEMIES; k++)
                       {
                         if ( vilans[k].active == 1 && vilans[k].explosao == 0)
                         {
                             int heroi_ferido = checaColisao(
                                 vilans[k].ship.x,
                                 vilans[k].ship.y,
                                 vilans[k].raio,
                                 Heroi.ship.x,
                                 Heroi.ship.y,
                                 HERO_W/2.0);
                                                      
                           if (heroi_ferido == 1)
                            {
                             al_rest(3);
                             playing = 0;
                            }
                         }
                       }
//atualiza a tela (quando houver algo para mostrar)
//  al_flip_display();

//pausa o jogo por 3 segundos se o jogador morrer
// if(!playing)
// al_rest(3);
          redesenhar = 1;
  }

//redesenha e zera o redesenhador

/*tente comentar essa parte*/if (redesenhar && al_is_event_queue_empty(event_queue)) {
        redesenhar = 0;
        drawScenario(Heroi);
        drawVilan(vilans);
        drawHero(&Heroi);
        
        
        al_flip_display();
}


} //fim do while

al_rest(3);     
//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)

 
   if(Heroi.score > recorde_atual)
     {
       recorde_atual = (int)Heroi.score;
       salvaRecorde(recorde_atual);
       recorde_quebrado = 1;
     }



char my_text[100];
al_clear_to_color(al_map_rgb(0,0,0));
sprintf(my_text, "Pontuação: %d", (int)Heroi.score);
al_draw_text(FONT_32, al_map_rgb(220, 30, 0), SCREEN_W/3, (SCREEN_H/2)+50, 0, my_text);

if (recorde_quebrado == 1){
    al_draw_text(FONT_32, al_map_rgb(255, 215, 0),100, (SCREEN_H/2) -  100, 0, "PARABENS, VOCE QUEBROU O RECORDE!");
    }

/*------------mostra o recorde atual-----------*/
char text_recorde[100];
sprintf(text_recorde, "Recorde: %d", recorde_atual);
al_draw_text(FONT_32, al_map_rgb(200, 200, 200), SCREEN_W/3, SCREEN_H/2, 0, text_recorde);


al_flip_display();
al_rest(3);
 
al_destroy_timer(timer);
al_destroy_display(display);
al_destroy_event_queue(event_queue);
al_destroy_sample(musica_de_fundo);   
al_destroy_bitmap(imagem_de_fundo);
 
return 0;
}
