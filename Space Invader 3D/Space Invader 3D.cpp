//*********************************************************************
//  UNIVERSIDADE FEDERAL DO PIAUÍ  - UFPI
//  CURSO DE BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO
//  DISCIPLINA: Computação Gráfica     PERÍODO: 2012.1
//  PROFESSOR: Laurindo de Sousa Brito Neto
//  GRADUANDO: Jonatas C. dos S. Ferreira
//  CÓDIGO: Space Invader 3D
//*********************************************************************

/* Inclui o header do OpenGL, GLUT */
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <irrKlang.h>

// include console I/O methods (conio.h for windows, our wrapper in linux)
#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

/**********************************************************************/
/*                                                                    */
/*                Declaração de constantes e variáveis                */
/*                                                                    */
/**********************************************************************/
// Constantes tiros
#define TIRONAVE 0
#define TIROINVASOR 1
#define TEMPODELAYTIRONAVE 0
#define TEMPODELAYTIROINVASOR 30
// Constantes vida invasor
#define VIVO       0
#define FOGO1      1
#define FOGO2      2
#define FOGO3      3
#define FOGO4      4
#define FOGO5      5
#define FOGO6      6
#define FOGO7      7
#define FOGO8      8
#define FOGO9      9
#define DESTRUIDO  10
// Constantes movimento invasor
#define ESQUERDA 0
#define DIREITA  1
// Constantes eixo
#define eixoX 0
#define eixoY 1
#define eixoZ 2
// Constantes camera
#define cameraF1 0
#define cameraF2 1


using namespace irrklang;
// start the sound engine with default parameters
ISoundEngine* engine = createIrrKlangDevice();

int largura;
int altura;
int camera;
bool tremerCamera;
bool flagTremerDirEsq;
int contTremeCamera;
int limiteEsqInvasor;
int limiteDirInvasor;

static GLsizei cameraX = 500, cameraY = 500, cameraZ = 1900;
static GLsizei alvoCamX = 500, alvoCamY = 500, alvoCamZ = 500;
static double visaoZMin = 0.5, visaoZMax = cameraZ+2000;
static int ladoX = 0, ladoY = 1, ladoZ = 0;

struct rgb{
    float r;
    float g;
    float b;
};

struct coresBotao{
    rgb playPause;
    rgb iniciar;
};
coresBotao botaoRGB;

struct ponto{
    int x;
    int y;
    int z;
};

struct pontosFogo{
    ponto pontos;
    pontosFogo *prox;
};
pontosFogo *listaPontosFogo;
int raioFogoMax = 12;
int saltoRaioFogo = 2;


struct invasores{
    int delay;
    int situacao;
    ponto translacao;// acumulo de translações
    invasores *prox;
    invasores *anter;
};
invasores *listaInvasor;
int raioColisaoInvasor = 40;
int movInvasorX = 5;
int movInvasorY = 15;
int distInvasorX = 150;
int distInvasorY = 150;
int distInvasorZ = 150;
int direcaoMovimento = ESQUERDA;

struct espacoNave{
    ponto centro;
    int situacao;
    int delay;
    float anguloX;// acumulo de rotaçoes X
    float anguloY;// acumulo de rotaçoes Y
    float anguloZ;// acumulo de rotaçoes Z
    ponto translacao;// acumulo de translações
};
espacoNave nave;
int raioColisaoNave = 40;
int saltoNave = 15;

struct tiros{
    int tipo;
    rgb cor;
    int situacao;
    int raioColisaoTiro;
    ponto translacao;// acumulo de translações
    tiros *prox;
    tiros *anter;
};
tiros *listaTirosNave;
tiros *listaTirosInvasor;
int contDelayTiroInvasores;

// JOGADOR
int vida;
int pontuacao;
int invasoresVivos;
int fase;
int contDelayTiroNave = 0;
bool play = true;// play / pause
bool iniciar = false;
bool fullScreen = false;
int contTirosNave;
int contTempo;

/**********************************************************************/
/*                                                                    */
/*                        Declaração de funções                       */
/*                                                                    */
/**********************************************************************/
void init();
void criaMenu();
void executarMenu(int opcao);
void desenha(void);
void alteraTamanhoJanela(int largura, int altura);
void teclado(unsigned char tecla, int x, int y);
void teclasEspeciais(int tecla, int x, int y);
void gerenciaMouse(int botao, int estado, int x, int y);
void moveMouseBotaoPressionado(int x, int y);
void moveMouse(int x, int y);
void Idle(void);
void desenhaNave(void);
void desenhaPiso(void);
void desenhaCubo(void);
void addTirosJogo(tiros *t);
void remTirosJogo(tiros *t);
void addInvasores(invasores *in);
void remInvasores(invasores *in);
void desenhaTiros(void);
void desenhaInvasores(void);
void movimentarInvasores(void);
void desenhaSombra(int x, int z);
void detectaColisao();
void desenhaFogo(int situacao);
void desenhaPontosDeFogo(int situacao);
void desenhaTextosString(int x, int y, int z, char *text, float scala, int linha, float r, float g, float b);
void desenhaTextosInt(int x, int y, int z, int numero, float scala, int linha, float r, float g, float b);
void desenhaTextosChar(int x, int y, int z, char c);
void desenhaTextosJogoF1(void);
void desenhaTextosJogoF2(void);
char * converteIntParaChar(int x);
void rasterizacaoCircunferencia(int eixoZero, int raio);
void addPontosDeFogo(int a1, int a2, int eixoZero);
void animate_callback(void);
void timer_callback(int value);
void defineParametrosDeVisualizacao(void);
void vibrarCamera();


/**********************************************************************/
/*                                                                    */
/*                       Função principal (main)                      */
/*                                                                    */
/**********************************************************************/
int main(int argc, char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,600);
    glutCreateWindow("Space Invader 3D");
     

    glutDisplayFunc(desenha);
    glutReshapeFunc(alteraTamanhoJanela);
    glutKeyboardFunc(teclado);
    glutMouseFunc(gerenciaMouse);
    glutSpecialFunc(teclasEspeciais);
    glutMotionFunc(moveMouseBotaoPressionado);
    glutPassiveMotionFunc(moveMouse);
    glutIdleFunc(Idle);
    glutTimerFunc(40, timer_callback,40);
    
    init();
    criaMenu();
        
    glutMainLoop();

	// After we are finished, we have to delete the irrKlang Device created earlier
	// with createIrrKlangDevice(). Use ::drop() to do that. In irrKlang, you should
	// delete all objects you created with a method or function that starts with 'create'.
	// (an exception is the play2D()- or play3D()-method, see the documentation or the
	// next example for an explanation)
	// The object is deleted simply by calling ->drop().
	engine->drop(); // delete engine
    return 0;
}






/**********************************************************************/
/*                                                                    */
/*          Função para inicializar parâmetros e variáveis            */
/*                                                                    */
/**********************************************************************/
void init(void){
     if (!engine){
		printf("Could not startup engine\n");
		exit(0); // error starting up the engine
	 }
     engine->play2D("som/air_raid.wav");
     
     GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
	 GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};		 // "cor"
	 GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};   // "brilho"
	 GLfloat posicaoLuz[4]={1500.0, 1500.0, 1500.0, 1.0};
	 GLfloat posicaoLuz2[4]={0.0, 0.0, 0.0, 1.0};
  
	 // Capacidade de brilho do material
	 GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	 GLint especMaterial = 5;

 	 // Especifica que a cor de fundo da janela será preta
	 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	 // Habilita o modelo de tonalização de Gouraud
	 glShadeModel(GL_SMOOTH);
//    glShadeModel(GL_FLAT);

     // Define a refletância do material
     glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	 // Define a concentração do brilho
	 glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	 // Ativa o uso da luz ambiente
	 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	 // Define os parâmetros da luz de número 0
	 glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	 glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	 glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
	 
	 // Define os parâmetros da luz de número 1
	 glLightfv(GL_LIGHT1, GL_AMBIENT, luzAmbiente);
	 glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa );
	 glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular );
	 glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz2 );

	 // Habilita a definição da cor do material
	 // a partir da cor corrente
	 glEnable(GL_COLOR_MATERIAL);
	 //Habilita o uso de iluminação
	 glEnable(GL_LIGHTING);
	 // Habilita a luz de número 0
	 glEnable(GL_LIGHT0);
     // Habilita o depth-buffering
	 glEnable(GL_DEPTH_TEST);
	 
	 for(int i=saltoRaioFogo; i <= raioFogoMax; i = i + saltoRaioFogo){
	     rasterizacaoCircunferencia(eixoY, i);
     }
     for(int i=saltoRaioFogo; i <= raioFogoMax; i = i + saltoRaioFogo){
	     rasterizacaoCircunferencia(eixoZ, i);
     }
/*
	 for(int x = (-1*raioFogo); x <= distPontoFogo; x = x+distPontoFogo){
         for(int y = (-1*raioFogo); y <= distPontoFogo; y = y+distPontoFogo){
             for(int z = (-1*raioFogo); z <= distPontoFogo; z = z+distPontoFogo){
                 pontosFogo *p = new pontosFogo();
                 p->pontos.x = x;
                 p->pontos.y = y;
                 p->pontos.z = z;
                 if(listaPontosFogo != NULL){
                    p->prox = listaPontosFogo;
                    listaPontosFogo = p;
                 }else{
                     listaPontosFogo = p;
                     p->prox = NULL;
                 }
             }
         }
     }
*/

	 
	 
	 
     play = iniciar = false;
     fullScreen = false;
     camera = cameraF1;
     
     vida = 3;
     pontuacao = 0;
     invasoresVivos = 60;
     fase = 1;
     contDelayTiroNave = 0;
     contDelayTiroInvasores = 0;
     contTirosNave = 0;
     contTempo = 0;
     tremerCamera = false;
     contTremeCamera = 0;
	 
	 nave.anguloX = 0.0f;
	 nave.anguloY = 0.0f;
	 nave.anguloZ = 0.0f;
	 nave.situacao = VIVO;
	 nave.centro = * new ponto();
	 nave.translacao = * new ponto();
	 nave.translacao.x = nave.centro.x = 500;
	 nave.translacao.y = nave.centro.y = 50;
	 nave.translacao.z = nave.centro.z = 500;
	 
	 for(int x=1; x<=5; x++){
             for(int z=1; z<=6; z++){
                 invasores * in = new invasores();
                 in->translacao.x = x * distInvasorX;
                 in->translacao.y = 1000;
                 in->translacao.z = z * distInvasorZ;
                 in->situacao = VIVO;
                 addInvasores(in);
                 if(x==1 && z==1) limiteEsqInvasor = in->translacao.x;
                 if(x==5 && z==6) limiteDirInvasor = in->translacao.x;
             }
     }
     for(int x=1; x<=5; x++){
             for(int z=1; z<=6; z++){
                 invasores * in = new invasores();
                 in->translacao.x = x * distInvasorX;
                 in->translacao.y = 1000 + distInvasorY;
                 in->translacao.z = z * distInvasorZ;
                 in->situacao = VIVO;
                 addInvasores(in);
             }
     }
     return;
}






/**********************************************************************/
/*                                                                    */
/*                      Função para Criar o Menu                      */
/*                                                                    */
/**********************************************************************/
void criaMenu(void){
//     glutCreateMenu(executarMenu);
//     glutAddMenuEntry("Teste 1", 0);
//     glutAddMenuEntry("Teste 2", 1);
//     glutAddMenuEntry("Play/Pause", 2);
//     glutAddMenuEntry("Sair", 3);
//     glutAttachMenu(GLUT_RIGHT_BUTTON);
     return;
}






/**********************************************************************/
/*                                                                    */
/*             Função para Execução da Opção do Menu                  */
/*                                                                    */
/**********************************************************************/
void executarMenu(int opcao){
     switch(opcao){
              case 0:
                   break;
              case 1:
                   break;
              case 2:
                   play = !play;
                   break;
              case 3:
                   engine->drop(); // delete engine
                   exit(0);
                   break;
              default:
                   break;
    } 
    return;
}






/**********************************************************************/
/*                                                                    */
/*                      Função desenha tela                           */
/*                                                                    */
/**********************************************************************/
void desenha(void){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    contTempo++;
    vibrarCamera();
    if(camera == cameraF2) desenhaCubo(); 

    if(camera == cameraF1) desenhaTextosJogoF1();
    else desenhaTextosJogoF2();

    if(play) detectaColisao();
    
    desenhaPiso();
    
    desenhaNave();
    
    desenhaTiros();
    
    desenhaInvasores();

    glutSwapBuffers();
    return;
}






/**********************************************************************/
/*                                                                    */
/*                   Função Altera Tamanho Janela                     */
/*                                                                    */
/**********************************************************************/
void alteraTamanhoJanela(int w, int h){
    largura = w;
    altura = h;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    defineParametrosDeVisualizacao();
    return;
}






/**********************************************************************/
/*                                                                    */
/*                   Função                                           */
/*                                                                    */
/**********************************************************************/
void defineParametrosDeVisualizacao(void){
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)largura/(float)altura, visaoZMin, visaoZMax);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (cameraX, cameraY, cameraZ,   alvoCamX, alvoCamY, alvoCamZ, ladoX, ladoY, ladoZ);
    glutPostRedisplay(); // Manda o redesenhar o ecrã em cada frame
    return;
}






/**********************************************************************/
/*                                                                    */
/*                   Função                                           */
/*                                                                    */
/**********************************************************************/
void defineParametrosDeGluLookAt(GLsizei camX, GLsizei camY, GLsizei camZ, 
                                 GLsizei alvoX, GLsizei alvoY, GLsizei alvoZ, 
                                   int x, int y, int z){
    cameraX = camX;      cameraY = camY;      cameraZ = camZ;
    alvoCamX = alvoX;    alvoCamY = alvoY;    alvoCamZ = alvoZ;
    ladoX = x;           ladoY = y;           ladoZ = z;
    defineParametrosDeVisualizacao();
    return;
}






/**********************************************************************/
/*                                                                    */
/*                      Função para teclado                           */
/*                                                                    */
/**********************************************************************/
void teclado(unsigned char tecla, int x, int y){
     printf(">> tecla: %i\n",tecla);
     switch(tecla){
             case 27:
                  exit(0);
                  break;
             case 102:
                  if(iniciar){
                      if(fullScreen == false){
                          glutFullScreen();
                          fullScreen = true;
                      }
                      else{
                          glutReshapeWindow(800,600);
                          glutPositionWindow(100,100);
                          fullScreen = false;
                      }
                  }
                  break;
             case 70:
                  if(iniciar){
                      if(fullScreen == false){
                          glutFullScreen();
                          fullScreen = true;
                      }
                      else{
                          glutReshapeWindow(800,600);
                          glutPositionWindow(100,100);
                          fullScreen = false;
                      }
                  }
                  break;
             case 112:
                  if(iniciar) play = !play;
                  break;
             case 80:
                  if(iniciar) play = !play;
                  break;
             case 32:// tiro tecla espaço
                 if(contDelayTiroNave <= 0  &&  play == true  &&  vida > 0){
                      contDelayTiroNave = TEMPODELAYTIRONAVE;
                      tiros *t = new tiros();
                      t->translacao = nave.translacao;
                      t->tipo = TIRONAVE;
                      t->raioColisaoTiro = 15;
                      t->cor.r = 1.0;
                      t->cor.g = 1.0;
                      t->cor.b = 1.0;
                      addTirosJogo(t);
                      engine->play2D("som/explosion.wav");
                      contTirosNave++;
    //                  printf("Tiro Nave...\n");
                  }else{
                       contDelayTiroNave--;
                  }
                  break;
             case 13:
                  if(iniciar) play = !play;
                  else{
                      play = true;
                      iniciar = true;
                  }
                  break;
     }
     
     // Trata SHIFT, CTRL e ALT
//     int modificadores = glutGetModifiers();
//     if(modificadores & GLUT_ACTIVE_SHIFT) printf("(SHIFT pressionado)\n");
//     if(modificadores & GLUT_ACTIVE_CTRL) printf("(CTRL pressionado)\n");
//     if(modificadores & GLUT_ACTIVE_ALT) printf("(ALT pressionado)\n");


//     defineParametrosDeVisualizacao();
     return;
}






/**********************************************************************/
/*                                                                    */
/*                  Função para teclas especiais                      */
/*                                                                    */
/**********************************************************************/
void teclasEspeciais(int tecla, int x, int y){
     switch(tecla){
              case GLUT_KEY_F1: 
                   printf(">> Camera 1\n");
                   camera = cameraF1;
                   defineParametrosDeGluLookAt(500, 500, 1900,
                                 500, 500, 500,0,1,0);
                   break;
              case GLUT_KEY_F2: 
                   printf(">> Camera 2\n");
                   camera = cameraF2;
                   break;
              case GLUT_KEY_F3: 
                   printf(">> tecla F3 pressionada\n");
                   break;
              case GLUT_KEY_F4: 
                   printf(">> tecla F4 pressionada\n");
                   break;
              case GLUT_KEY_UP: 
//                  printf(">> tecla UP pressionada\n");
                    if(camera == cameraF1){
                        if(nave.translacao.z > 0  &&  play  &&  vida > 0){
                             nave.translacao.z = nave.translacao.z - saltoNave;
                             nave.centro.z = nave.centro.z - saltoNave;
                        }
                    }else{
                         if(nave.translacao.z < 955  &&  play == true  &&  vida > 0){
                              nave.translacao.z = nave.translacao.z + saltoNave;
                              nave.centro.z = nave.centro.z + saltoNave;
                         }
                    }
                   break;
              case GLUT_KEY_DOWN: 
//                  printf(">> tecla DOWN pressionada\n");
                    if(camera == cameraF1){
                         if(nave.translacao.z < 955  &&  play == true  &&  vida > 0){
                              nave.translacao.z = nave.translacao.z + saltoNave;
                              nave.centro.z = nave.centro.z + saltoNave;
                         }
                    }else{
                         if(nave.translacao.z > 0  &&  play  &&  vida > 0){
                              nave.translacao.z = nave.translacao.z - saltoNave;
                              nave.centro.z = nave.centro.z - saltoNave;
                         }
                    }
                   break;
              case GLUT_KEY_LEFT: 
                   if(nave.translacao.x > 20  &&  play == true  &&  vida > 0){
//                        printf(">> tecla LEFT pressionada\n");
                        nave.translacao.x = nave.translacao.x - saltoNave;
                        nave.centro.x = nave.centro.x - saltoNave;
                   }
                   break;
              case GLUT_KEY_RIGHT: 
                   if(nave.translacao.x < 980  &&  play == true  &&  vida > 0){
//                        printf(">> tecla RIGHT pressionada\n");
                        nave.translacao.x = nave.translacao.x + saltoNave;
                        nave.centro.x = nave.centro.x + saltoNave;
                   }
                   break;
              case GLUT_KEY_PAGE_UP: 
                   if(nave.translacao.y < 1000  &&  play == true  &&  vida > 0){
//                        printf(">> tecla PAGE_UP pressionada\n");
                        nave.translacao.y = nave.translacao.y + saltoNave;
                        nave.centro.y = nave.centro.y + saltoNave;
                   }
                   break;
              case GLUT_KEY_PAGE_DOWN: 
                   if(nave.translacao.y > 50  &&  play == true  &&  vida > 0){
//                        printf(">> tecla PAGE_DOWN pressionada\n");
                        nave.translacao.y = nave.translacao.y - saltoNave;
                        nave.centro.y = nave.centro.y - saltoNave;
                   }
                   break;
     }
     if(camera == cameraF2) defineParametrosDeGluLookAt(nave.translacao.x, nave.translacao.y-45, nave.translacao.z,
                                 nave.translacao.x, nave.translacao.y+1000, nave.translacao.z,0,0,1);
     return;
}






/**********************************************************************/
/*                                                                    */
/*                Função para Gerenciamento do Mouse                  */
/*    Trata os eventos de pressionamento e liberação dos botões       */
/*                                                                    */
/**********************************************************************/
void gerenciaMouse(int botao, int estado, int x, int y){
   switch (botao) {
        case GLUT_RIGHT_BUTTON:
              if (estado == GLUT_DOWN) {
              }
              break;
        case GLUT_MIDDLE_BUTTON:
              if (estado == GLUT_DOWN) {
              }
              break;
        case GLUT_LEFT_BUTTON:
              if (estado == GLUT_UP) {
                    if(fullScreen==false){
                        if(x > 335  &&  x < 470  &&  y > 260  &&  y < 300 ){
                             play = true;
                             iniciar = true;
                             botaoRGB.iniciar.r = 0.0;
                             botaoRGB.iniciar.g = 0.0;
                             botaoRGB.iniciar.b = 1.0;
                        }
                    }else{
                        if(x > 600  &&  x < 770  &&  y > 330  &&  y < 385 ){
                             play = true;
                             iniciar = true;
                             botaoRGB.iniciar.r = 0.0;
                             botaoRGB.iniciar.g = 0.0;
                             botaoRGB.iniciar.b = 1.0;
                        }
                    }
              }else{
                    botaoRGB.iniciar.r = 1.0;
                    botaoRGB.iniciar.g = 0.0;
                    botaoRGB.iniciar.b = 0.0;
              }
              break;
        default:
              break;
   }
   return;
}






/**********************************************************************/
/*                                                                    */
/*          Função para Gerenciamento do Movimento do Mouse           */
/*                     com um botão pressionado                       */
/*                                                                    */
/**********************************************************************/
void moveMouseBotaoPressionado(int x, int y){

    return;
}






/**********************************************************************/
/*                                                                    */
/*          Função para Gerenciamento do Movimento do Mouse           */
/*              quando nenhum botão estiver pressionado               */
/*                                                                    */
/**********************************************************************/
void moveMouse(int x, int y){

    return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para o Sistema Ocioso                    */
/*                                                                    */
/**********************************************************************/
void Idle(void){

    return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar nave                       */
/*                                                                    */
/**********************************************************************/
void desenhaNave(void){
//     glColor3f(1.0f, 0.0f, 0.0f); 
     glPushMatrix();
     glTranslatef(nave.translacao.x, nave.translacao.y, nave.translacao.z);
     
     if(nave.situacao >= 1  &&  nave.situacao != DESTRUIDO){
           desenhaFogo(nave.situacao);
           if(play ){
               nave.delay++;
               if(nave.delay >= 5){
                   nave.situacao = nave.situacao + 1;
                   nave.delay = 1;
               }
           }
     }
     if(vida > 0 && nave.situacao == DESTRUIDO) nave.situacao = VIVO;
     
     glRotatef(nave.anguloX,1,0,0);
     glRotatef(nave.anguloY,0,1,0);
     glRotatef(nave.anguloZ,0,0,1);
     
//     glColor3f(0.0f, 1.0f, 0.0f);
//     glutWireSphere(40,20,20);  //  (raio, linhasHor, linhasVer)
     
     glRotatef(270,1,0,0);
     if(vida == 0)glColor3f(0.5f, 0.0f, 0.0f);
     else glColor3f(1.0f, 0.0f, 0.0f); 
     if(camera == cameraF1)glutSolidSphere(30,20,20);
     else { //glutWireSphere(4,5,10);
     glPushAttrib(GL_LINE_BIT);
     glLineWidth(3);
     glBegin(GL_LINES);
         glColor3f(1.0f, 1.0f, 1.0f); 
         glVertex3i( -4, -4, 20);
         glVertex3i( -4, 4, 20);
     
         glVertex3i( -4, 4, 20);
         glVertex3i( 4, 4, 20);
        
         glVertex3i( 4, 4, 20);
         glVertex3i( 4, -4, 20);
         
         glVertex3i( 4, -4, 20);
         glVertex3i( -4, -4, 20);
         
         glVertex3i( -4, 0, 20);
         glVertex3i( -6, 0, 20);
     
         glVertex3i( 0, 4, 20);
         glVertex3i( 0, 6, 20);
        
         glVertex3i( 4, 0, 20);
         glVertex3i( 6, 0, 20);
         
         glVertex3i( 0, -4, 20);
         glVertex3i( 0, -6, 20);
     glEnd();
     glPopAttrib();
     }
     glColor3f(1.0f, 1.0f, 0.0f); 
     glTranslatef(25,0,-40);
     if(vida == 0){
          glRotatef(nave.situacao*10,0,1,0);
          glColor3f(0.0f, 0.0f, 0.0f);
          glutSolidCone(10,100,10,10);//glutSolidCone(base,altura,fatias,pilha);
          glTranslatef(-50,0,0);
          glRotatef(nave.situacao*(-20),0,1,0);
          glutSolidCone(10,100,10,10);
          if(nave.translacao.y > 50) nave.translacao.y = nave.translacao.y - 3;
     }else{
          glutSolidCone(10,100,10,10);//glutSolidCone(base,altura,fatias,pilha);
          glTranslatef(-50,0,0);
          glutSolidCone(10,100,10,10);
     }
     
     glPopMatrix();
     desenhaSombra(nave.translacao.x, nave.translacao.z);
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar invasores                  */
/*                                                                    */
/**********************************************************************/
void desenhaInvasores(void){
    invasores *in = listaInvasor;
    while(in != NULL){
         if(in->situacao != DESTRUIDO){
             glPushMatrix();
        
             glTranslatef(in->translacao.x, in->translacao.y, in->translacao.z);
             if(in->situacao >= 1  &&  in->situacao != DESTRUIDO){
                   desenhaFogo(in->situacao);
                   if(play ){
                       in->delay++;
                       if(in->delay >= 5){
                           in->situacao = in->situacao + 1;
                           in->delay = 1;
                       }
                   }
             }
             
             glRotatef(90,1,0,0);
 /*            glColor3f(0.0f, 0.6f, 0.0f); 
             if(in->situacao==VIVO)glutSolidSphere(30,20,20);
             else if(in->situacao <= FOGO6){
                  glColor3f(0.890f, 0.380f, 0.0f); 
                  glutWireSphere(15,5,5);
             }else{
                  
             }  */
             glColor3f(1.0f, 1.0f, 0.0f); 
             glPushMatrix();
                glTranslatef(25 + in->situacao*15,0,-40 + in->situacao*20);
                if(in->situacao!=VIVO)glRotatef(360-in->situacao*10,0,1,0);
                glutSolidCone(10,100,10,10);//glutSolidCone(base,altura,fatias,pilha);
             glPopMatrix();
             glPushMatrix();
                glTranslatef(-25 - in->situacao*15,0,-40 + in->situacao*20);
                glRotatef(in->situacao*10,0,1,0);
                glutSolidCone(10,100,10,10);
             glPopMatrix();
             
             
             glColor3f(0.0f, 0.6f, 0.0f); 
             if(in->situacao==VIVO )glutSolidSphere(30,20,20);
             else if(in->situacao <= FOGO6){
                  glColor3f(0.890f, 0.380f, 0.0f); 
                  glutWireSphere(15,5,5);
             }else{
                  remInvasores(in);
             }
             
             
             glPopMatrix();
             desenhaSombra(in->translacao.x, in->translacao.z);
         }
         in = in->prox;
    }
    if(play) movimentarInvasores();
    return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenho dos tiros                   */
/*                                                                    */
/**********************************************************************/ 
void desenhaTiros(void){
     tiros *t = listaTirosNave;
     while(t != NULL){
         if(t->translacao.y < 2000){  
             glPushMatrix();
             glColor3f(t->cor.r, t->cor.g, t->cor.b); 
             if(camera == cameraF1){
                   glTranslatef(t->translacao.x, t->translacao.y, t->translacao.z);
                   glutSolidSphere(t->raioColisaoTiro-5,10,10);
             }
             else {
                  glTranslatef(t->translacao.x, t->translacao.y, t->translacao.z);
                  if(t->translacao.y > 30) glutSolidSphere(t->raioColisaoTiro-10,10,10);
             }
             glPopMatrix();
             if( play ){
                      t->translacao.y = t->translacao.y + 50;
             }
         }
         t = t->prox;
     }
     
     
     t = listaTirosInvasor;
     while(t != NULL){
         if(t->translacao.y > 0){  
             glPushMatrix();
             glColor3f(t->cor.r, t->cor.g, t->cor.b); 
             glTranslatef(t->translacao.x, t->translacao.y, t->translacao.z);
             glutSolidSphere(t->raioColisaoTiro-5,10,10);
             glPopMatrix();
             if( play ){
                      t->translacao.y = t->translacao.y - 10;
             }
         }
         t = t->prox;
     }
/*     t = listaTirosNave;
     if(t != NULL){
         while(t->translacao.y >= 2000 && t->prox !=NULL){  
             t = t->prox;
         }
     }
     listaTirosNave = t; */
//--------------------------  desenha tiros dos invasores  -------     
/*     if(contDelayTiroInvasores == 0  &&  play == true){
          tiros *tir = new tiros();
          contDelayTiroInvasores = TEMPODELAYTIROINVASOR;
          invasores *invas = listaInvasor;
          int i = contTempo%60;
          while(i>1){
                invas = invas->prox;
                i--;
          } 
          tir->translacao = invas->translacao ;
          tir->cor.r = 0.0;
          tir->cor.g = 1.0;
          tir->cor.b = 0.0;
          tir->tipo = TIROINVASOR;
          tir->raioColisaoTiro = 15;
 //         addTirosJogo(t);  
      }  
*/
 
     
     if(contDelayTiroInvasores <= 0  &&  play == true ){
          contDelayTiroInvasores = TEMPODELAYTIROINVASOR;
          int i=0;
          invasores *invas = listaInvasor;

          if(invasoresVivos != 0) i = (int)contTempo%invasoresVivos;
          while(i>0 && invas !=NULL){
                invas = invas->prox;
                i--;
          } 
          if(invas !=NULL){
              tiros *t = new tiros();
              t->translacao = invas->translacao;
              t->tipo = TIROINVASOR;
              t->raioColisaoTiro = 15;
              t->cor.r = 0.2;
              t->cor.g = 1.0;
              t->cor.b = 0.2;
              addTirosJogo(t);
          }
     }else{
          contDelayTiroInvasores--;
     }
     
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenho dos textos                  */
/*                                                                    */
/**********************************************************************/ 
void desenhaTextosJogoF1(void){
     desenhaTextosString(-100, 980, 1000, "VIDAS", 0.2, 2, 1.0f, 0.0f, 0.0f); 
     desenhaTextosInt(-90, 940, 1000, vida, 0.2, 2, 1.0f, 0.0f, 0.0f); 
     
     desenhaTextosString(-100, 900, 1000, "INVASORES", 0.2, 2, 1.0f, 0.0f, 0.0f); 
     desenhaTextosInt(-90, 860, 1000,invasoresVivos, 0.2, 2, 1.0f, 0.0f, 0.0f); 
     
     desenhaTextosString(1000, 980, 1000, "PONTOS", 0.2, 2, 1.0f, 0.0f, 0.0f); 
     desenhaTextosInt(1010, 940, 1000, pontuacao, 0.2, 2, 1.0f, 0.0f, 0.0f); 
     
//     desenhaTextosString(1000, 900, 1000, "FASE", 0.2, 2, 1.0f, 0.0f, 0.0f); 
//     desenhaTextosInt(1010, 860, 1000, fase, 0.2, 2, 1.0f, 0.0f, 0.0f); 
     
     if(vida <= 0)desenhaTextosString(100,380,1000,"GAME OVER",1,2,0.0,0.0,0.0);
     
     if(vida > 0  &&  invasoresVivos == 0)  desenhaTextosString(90,380,1000,"VOCE VENCEU",1,2,0.0,0.0,0.0);
     
     if(iniciar == false){
        desenhaTextosString(400, 500, 1000, "INICIAR", 0.5, 3, botaoRGB.iniciar.r, botaoRGB.iniciar.g, botaoRGB.iniciar.b); 
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(4);
        glBegin(GL_LINES);
            if(contTempo%50 <= 24)glColor3f(1.0, 0.0, 0.0); 
            else glColor3f(0.0, 1.0, 0.0); 
            glVertex3i( 380, 480, 1000);
            glVertex3i( 380, 580, 1000);
            
            glVertex3i( 380, 580, 1000);
            glVertex3i( 630, 580, 1000);
            
            glVertex3i( 630, 580, 1000);
            glVertex3i( 630, 480, 1000);
            
            glVertex3i( 630, 480, 1000);
            glVertex3i( 380, 480, 1000);
        glEnd();
        glPopAttrib();
     }else{
          if(play == false ){
                desenhaTextosString(400, 500, 1000, "PAUSE", 0.5, 3, botaoRGB.playPause.r, botaoRGB.playPause.g, botaoRGB.playPause.b); 
          }
     }
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenho dos textos                  */
/*                                                                    */
/**********************************************************************/ 
void desenhaTextosJogoF2(void){
     desenhaTextosString(nave.translacao.x+30 , nave.translacao.y+5, nave.translacao.z+11, "VIDAS", 0.01, 2, 0.0f, 1.0f, 1.0f); 
     desenhaTextosInt(nave.translacao.x+30 , nave.translacao.y+5, nave.translacao.z+9, vida, 0.01, 2, 0.0f, 1.0f, 1.0f); 
     
     desenhaTextosString(nave.translacao.x-30 , nave.translacao.y+5, nave.translacao.z+14, "INVASORES", 0.01, 2, 0.0f, 1.0f, 1.0f); 
     desenhaTextosInt(nave.translacao.x-30 , nave.translacao.y+5, nave.translacao.z+12,invasoresVivos, 0.01, 2, 0.0f, 1.0f, 1.0f); 
     
     desenhaTextosString(nave.translacao.x-30 , nave.translacao.y+5, nave.translacao.z+10, "PONTOS", 0.01, 2, 0.0f, 1.0f, 1.0f); 
     desenhaTextosInt(nave.translacao.x-30 , nave.translacao.y+5, nave.translacao.z+8, pontuacao, 0.01, 2, 0.0f, 1.0f, 1.0f); 
     
//     desenhaTextosString(1000, 900, 1000, "FASE", 0.2, 2, 1.0f, 1.0f, 1.0f); 
//     desenhaTextosInt(1010, 860, 1000, fase, 0.2, 2, 1.0f, 1.0f, 1.0f); 
     
     if(vida <= 0);desenhaTextosString(nave.translacao.x-8 , nave.translacao.y+5, nave.translacao.z+22,"GAME OVER",0.03,4, 1.0f, 0.0f, 1.0f); 
     
     if(vida > 0  &&  invasoresVivos == 0) ; desenhaTextosString(nave.translacao.x-10 , nave.translacao.y+5, nave.translacao.z+18,"VOCE VENCEU",0.03,4, 1.0f, 0.0f, 1.0f); 
     
     if(iniciar == false){
        desenhaTextosString(400, 500, 1000, "INICIAR", 0.5, 3, botaoRGB.iniciar.r, botaoRGB.iniciar.g, botaoRGB.iniciar.b); 
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(4);
        glBegin(GL_LINES);
            if(contTempo%50 <= 24)glColor3f(1.0, 0.0, 0.0); 
            else glColor3f(0.0, 1.0, 0.0); 
            glVertex3i( 380, 480, 1000);
            glVertex3i( 380, 580, 1000);
            
            glVertex3i( 380, 580, 1000);
            glVertex3i( 630, 580, 1000);
            
            glVertex3i( 630, 580, 1000);
            glVertex3i( 630, 480, 1000);
            
            glVertex3i( 630, 480, 1000);
            glVertex3i( 380, 480, 1000);
        glEnd();
        glPopAttrib();
     }else{
          if(play == false ){
                desenhaTextosString(400, 500, 1000, "PAUSE", 0.5, 3, botaoRGB.playPause.r, botaoRGB.playPause.g, botaoRGB.playPause.b); 
          }
     }
     return;
}    






/**********************************************************************/
/*                                                                    */
/*                    Função para detectar colisões                   */
/*                                                                    */
/**********************************************************************/ 
void detectaColisao(void){
     int x,y,z;
     invasores *inv;
     tiros *tn = listaTirosNave;
     while(tn != NULL){  //  colisão entre tiro nave e invasores
           if(tn->translacao.y < 2000){  
                 inv = listaInvasor;
                 while(inv != NULL){
                        if(inv->situacao == VIVO){  
                              x = tn->translacao.x  -  inv->translacao.x;
                              y = tn->translacao.y  -  inv->translacao.y;
                              z = tn->translacao.z  -  inv->translacao.z;
                              if((raioColisaoInvasor + tn->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
                                  inv->situacao = FOGO1;
                                  tn->translacao.y = 2500;
                                  invasoresVivos--;
                                  pontuacao = pontuacao + 15;
                                  engine->play2D("som/cannon_x.wav");
                              }
                        }
                        inv = inv->prox;
                 }
           }
           tn = tn->prox;
     }
     //  colisão entre tiros dos invasore com a nave amiga
     tn = listaTirosInvasor;
     while(tn != NULL){ 
           if(tn->translacao.y > 0){  
                              x = tn->translacao.x  -  nave.translacao.x;
                              y = tn->translacao.y  -  nave.translacao.y;
                              z = tn->translacao.z  -  nave.translacao.z;
                              if((raioColisaoNave + tn->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
                                  vida = vida - 1;
                                  tremerCamera = true;
                                  contTremeCamera = 0;
                                  if(vida <= 0){
                                       // game over   chime_big_ben
                                       engine->play2D("som/buzzer_x.wav");
                                       nave.situacao = FOGO1;
                                       vida = 0;
                                  }else{
                                       if(vida > 0) nave.situacao = FOGO1;
                                       tn->translacao.y = 0;
                                  }
                                  engine->play2D("som/cannon_x.wav");
                              }
           }
           tn = tn->prox;
     } 
     //  colisão entre tiros dos invasore com tiro da nave amiga
     tiros *tiroInvasor = listaTirosInvasor;
     tiros *tirosNave = listaTirosNave;
     while(tirosNave != NULL){ 
           if(tirosNave->translacao.y < 2000){  
                 while(tiroInvasor != NULL){
        //                if(inv->situacao == VIVO){  
                              x = tirosNave->translacao.x  -  tiroInvasor->translacao.x;
                              y = tirosNave->translacao.y  -  tiroInvasor->translacao.y;
                              z = tirosNave->translacao.z  -  tiroInvasor->translacao.z;
                              if((tirosNave->raioColisaoTiro + tiroInvasor->raioColisaoTiro) >= ( sqrt(x*x + y*y + z*z) )){
        //                          inv->situacao = FOGO1;
                                  tirosNave->translacao.y = 2500;
                                  tiroInvasor->translacao.y = 0;
        //                          invasoresVivos--;
        //                          pontuacao = pontuacao + 15;
        //                          engine->play2D("som/cannon_x.wav");
                              }
        //                }
                        tiroInvasor = tiroInvasor->prox;
                 }
           }
           tirosNave = tirosNave->prox;
     }
     //  verifica se algun invasor toco no chao
     inv = listaInvasor;
     while(inv != NULL){
          if(inv->translacao.y <= 50  &&  vida > 0 && inv->situacao == VIVO){
              vida = 0;
              tremerCamera = true;
              contTremeCamera = 0;
              nave.situacao = FOGO1; 
              engine->play2D("som/buzzer_x.wav");
          }  
          inv = inv->prox;
     }  
     return;
}    






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar fogo                       */
/*                                                                    */
/**********************************************************************/ 
void desenhaFogo(int situacao){
     glPushAttrib(GL_LINE_BIT);
//     glLineWidth(1);
     if(situacao == FOGO1){
         glColor3f(1.0f, 0.958f, 0.527f);  // (0.890f, 0.380f, 0.0f); 
         glutWireSphere(45,15,15);  //  (raio, linhasHor, linhasVer)
     }else if(situacao == FOGO2){
         glColor3f(1.0f, 0.858f, 0.427f);  // (0.890f, 0.380f, 0.0f); 
         glutWireSphere(50,15,15);  //  (raio, linhasHor, linhasVer)
     }else if(situacao == FOGO3){
         glColor3f(0.941f, 0.662f, 0.101f); // (0.941f, 0.662f, 0.101f);
         glutWireSphere(55,15,15);  //  (raio, linhasHor, linhasVer)
     }else if(situacao == FOGO4){
         glColor3f(0.941f, 0.662f, 0.101f); // (0.941f, 0.662f, 0.101f);
         glutWireSphere(60,15,15);  //  (raio, linhasHor, linhasVer)
     }else if(situacao == FOGO5){
         glColor3f(0.890f, 0.380f, 0.0f); // (1.0f, 0.858f, 0.227f);
//         glutWireSphere(65,15,15);  //  (raio, linhasHor, linhasVer)
     }else if(situacao == FOGO6){
         glColor3f(0.490f, 0.180f, 0.0f); // (1.0f, 0.858f, 0.227f);
//         glutWireSphere(70,15,15);  //  (raio, linhasHor, linhasVer)
     }else{
           glColor3f(1.0f, 0.0f, 0.0f);//(0.341f, 0.392f, 0.454f);
     }
     desenhaPontosDeFogo(situacao);
     glPopAttrib();
     return;
}    






/**********************************************************************/
/*                                                                    */
/*               Função para desenhar pontos de fogo                  */
/*                                                                    */
/**********************************************************************/ 
void desenhaPontosDeFogo(int situacao){
     glPushMatrix();
     glPushAttrib(GL_LINE_BIT);
     glLineWidth(10);
     glColor3f(1.0f, 0.0f, 0.0f);
     pontosFogo *p = listaPontosFogo;
//     glTranslatef(10,0,10);
     while(p != NULL){
          glPushMatrix();
          glTranslatef(situacao*p->pontos.x , situacao*p->pontos.y , situacao*p->pontos.z);
          glutSolidSphere(3,3,3);
//          glutWireSphere(20,7,7);  //  (raio, linhasHor, linhasVer)
          glPopMatrix();
          p = p->prox;
     }


//     glBegin(GL_POINTS);
//         while(p != NULL){
//              glVertex3i(situacao*p->pontos.x, situacao*p->pontos.y, situacao*p->pontos.z);
//              p = p->prox;
//         }
//     glEnd();

     glPopAttrib();
     glPopMatrix();
     return;
}  






/**********************************************************************/
/*                                                                    */
/*                    Função para piso                                */
/*                                                                    */
/**********************************************************************/
void desenhaPiso(void){ 
     glPushMatrix();
     glColor3f(0.0f, 1.0f, 0.0f);
     glBegin(GL_QUADS);
          glVertex3f(0.0f, 0.0f, 0.0f);
          glVertex3f(0.0f, 0.0f, 1000.0f);
          glVertex3f(1000.0f, 0.0f, 1000.0f);
          glVertex3f(1000.0f, 0.0f, 0.0f);
     glEnd();

     glPopMatrix();
     return;
}






/**********************************************************************/
/*                                                                    */
/*             Função para marcação do campo de visão                 */
/*                                                                    */
/**********************************************************************/
void desenhaCubo(void){ 
     glColor3f(1.0f, 1.0f, 1.0f);
     glPushMatrix();
     
     glTranslatef(500,500,500);
     glutWireCube(1000);
     glPopMatrix();
     
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para empilhar tiros                      */
/*                                                                    */
/**********************************************************************/
void addTirosJogo(tiros *t){ 
     if(t !=NULL){
         if(t->tipo == TIRONAVE){
                 if(listaTirosNave != NULL){
                      t->prox = listaTirosNave;
                      listaTirosNave->anter = t;
                      t->anter = t;
                      listaTirosNave = t;
                 }else{
                      t->prox = NULL;
                      t->anter = t;
                      listaTirosNave = t;
                 }
         }else{// tipo == tiro invasor
                 if(listaTirosInvasor != NULL){
                      t->prox = listaTirosInvasor;
                      listaTirosInvasor->anter = t;
                      t->anter = t;
                      listaTirosInvasor = t;
                 }else{
                      t->prox = NULL;
                      t->anter = t;
                      listaTirosInvasor = t;
                 }
         }
     }

     
     
/*
     if(t->tipo == TIRONAVE){
           if(listaTirosNave != NULL){
                t->prox = listaTirosNave;
           }
           t->anter = t;
           listaTirosNave = t;
     }else{
           if(listaTirosInvasor != NULL){
                t->prox = listaTirosInvasor;
           }
           t->anter = t;
           listaTirosInvasor = t;
     }
*/
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenpilhar tiros                   */
/*                                                                    */
/**********************************************************************/
void remTirosJogo(tiros *t){   
     if(t != NULL){
         if(t->tipo == TIRONAVE){
              if(t->anter != t){
                   t->anter->prox = t->prox;
              }else{
                   listaTirosNave == t->prox;
                   listaTirosNave->anter = listaTirosNave;
              }
              free(t);
         }else{
              if(t->anter != t){
                   t->anter->prox = t->prox;
              }else{
                   listaTirosInvasor == t->prox;
                   listaTirosInvasor->anter = listaTirosInvasor;
              }
              free(t);
         }
     }
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para empilhar invasores                  */
/*                                                                    */
/**********************************************************************/
void addInvasores(invasores *in){ 
     if(listaInvasor != NULL){
          in->prox = listaInvasor;
          listaInvasor->anter = in;
          in->anter = in;
          listaInvasor = in;
     }else{
          in->prox = NULL;
          in->anter = in;
          listaInvasor = in;
     }
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenpilhar invasores               */
/*                                                                    */
/**********************************************************************/
void remInvasores(invasores *in){   
     if(in != NULL){
          if(listaInvasor == in){
               listaInvasor = in->prox;
               if(listaInvasor != NULL) listaInvasor->anter = listaInvasor;
          }else if(in->prox == NULL){
               in->anter->prox = NULL;
          }else{
               in->anter->prox = in->prox;
               in->prox->anter = in->anter;
          }
          in->anter = NULL;
          in->prox = NULL;
          free(in);
     }
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para movimento dos invasores             */
/*                                                                    */
/**********************************************************************/ 
void movimentarInvasores(void){
    invasores *begin = listaInvasor;
//    invasores *end = listaInvasor;
//    while(end->prox != NULL){
//         end = end->prox;
//    }
    if(direcaoMovimento == ESQUERDA){
        if(limiteEsqInvasor > -30){
             limiteEsqInvasor = limiteEsqInvasor - movInvasorX;
             limiteDirInvasor = limiteDirInvasor - movInvasorX;
             invasores *aux = begin;
             while(aux != NULL){
                   aux->translacao.x = aux->translacao.x - movInvasorX;
                   aux = aux->prox;
             }
        }else{
             invasores *aux = begin;
             while(aux != NULL  &&  vida > 0){
                   aux->translacao.y = aux->translacao.y - movInvasorY;
                   aux = aux->prox;
             }
             direcaoMovimento = DIREITA;
        }
    }else{ // movimento para a direita
         if(limiteDirInvasor < 1030){
             limiteEsqInvasor = limiteEsqInvasor + movInvasorX;
             limiteDirInvasor = limiteDirInvasor + movInvasorX;
             invasores *aux = begin;
             while(aux != NULL){
                   aux->translacao.x = aux->translacao.x + movInvasorX;
                   aux = aux->prox;
             }
        }else{
             invasores *aux = begin;
             while(aux != NULL  &&  vida > 0){
                   aux->translacao.y = aux->translacao.y - movInvasorY;
                   aux = aux->prox;
             }
             direcaoMovimento = ESQUERDA;
        }
    }
    return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função necessária para a animação               */
/*                                                                    */
/**********************************************************************/ 
/*void animate_callback(void){
    glutPostRedisplay(); // Manda o redesenhar o ecrã em cada frame
    return;
}  */






/**********************************************************************/
/*                                                                    */
/*                    Função necessária para a animação               */
/*                                                                    */
/**********************************************************************/ 
void timer_callback(int value){
    glutTimerFunc(value, timer_callback, value);
    glutPostRedisplay(); // Manda o redesenhar o ecrã em cada frame
    return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar sombra                     */
/*                                                                    */
/**********************************************************************/
void desenhaSombra(int x, int z){ 
     glPushMatrix();
     glColor3f(0.0f, 0.0f, 0.0f);
     glTranslatef(x,0,z);
     glRotatef(270,1,0,0);
     glutSolidCone(40,1,8,1);
     glPopMatrix();
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar textos                     */
/*                                                                    */
/**********************************************************************/
void desenhaTextosString(int x, int y, int z, char *text, float scala, int linha, float r, float g, float b){ 
     glPushMatrix();
     glPushAttrib(GL_LINE_BIT);
     glLineWidth(linha);
     glTranslatef(x, y, z);   
     glScalef(scala,scala,scala);
     glColor3f(r, g, b);
     if(camera == cameraF2)glRotatef(90,1,0,0);
     while(*text) glutStrokeCharacter(GLUT_STROKE_ROMAN, *text++); // (font, string)
     
//     glRasterPos3i(x, y, z);  
//     while(*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++); // (font, string)
     glPopAttrib();
     glPopMatrix();
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar inteiros                   */
/*                                                                    */
/**********************************************************************/
void desenhaTextosInt(int x, int y, int z, int numero, float scala, int linha, float r, float g, float b){ 
     static bool f = true;
     char *c0 = "0";
     char *c1 = "1";
     char *c2 = "2";
     char *c3 = "3";
     char *c4 = "4";
     char *c5 = "5";
     char *c6 = "6";
     char *c7 = "7";
     char *c8 = "8";
     char *c9 = "9";
     glPushMatrix();
     glPushAttrib(GL_LINE_BIT);
//     glLineWidth(linha);
     glColor3f(r, g, b);
//     glTranslatef(x, y, z);   
//     glScalef(scala,scala,scala);
     glRasterPos3i(x, y, z);  
//     while(*text) glutStrokeCharacter(GLUT_STROKE_ROMAN, *text++); // (font, string)
     int res[10], i;
     for(i=0; numero > 0; i++){
          res[i] = numero%10;
          numero = numero/10;
          
     }
     if(i > 0){
          for(i=i-1; i>=0; i--){
              if(res[i] == 0)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c0); // (font, string)
              if(res[i] == 1)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c1); // (font, string)
              if(res[i] == 2)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c2); // (font, string)
              if(res[i] == 3)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c3); // (font, string)
              if(res[i] == 4)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c4); // (font, string)
              if(res[i] == 5)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c5); // (font, string)
              if(res[i] == 6)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c6); // (font, string)
              if(res[i] == 7)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c7); // (font, string)
              if(res[i] == 8)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c8); // (font, string)
              if(res[i] == 9)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c9); // (font, string)
          }
     }else{
         char *c = "0";
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); // (font, string)
     }
     
     f=false;
     
     glPopAttrib();
     glPopMatrix();
     return;
}






/**********************************************************************/
/*                                                                    */
/*                    Função para desenhar textos                     */
/*                                                                    */
/**********************************************************************/
/*void desenhaTextosChar(int x, int y, int z, char c){ 
//     glRasterPos2f(x,y);   
     glutStrokeCharacter(GLUT_STROKE_ROMAN, c); // (font, string)
     return;
}*/






/**********************************************************************/
/*                                                                    */
/*                    Função converte int para string                 */
/*                                                                    */
/**********************************************************************/
/*char * converteIntParaChar(int x){ 
     char *text = "0123456";
     
     char t1 = '1';
     char *t2 = "2";
     char *t3 = "3";
     char *t4 = "4";
     
     
     *++text;
     *--text;
  
     
     return text;
} */






/**********************************************************************/
/*                                                                    */
/*    Algoritmo de Bresenham para rasterização de circunferências     */
/*         usado para fazer as circunferencias das explosões          */
/*                                                                    */
/**********************************************************************/
void rasterizacaoCircunferencia(int eixoZero, int raio){
     int x, y, d, deltaE, deltaSE;
     
     // calculo da circunferencia na origem
     
     // calculo
     d = 1 - raio;
     deltaE = 3;
     deltaSE = -2*raio + 5;
     x = 0;
     y = raio;
     
     // marcar os quatro pixels

     addPontosDeFogo(0, raio, eixoZero);
     addPontosDeFogo(raio, 0, eixoZero);
     addPontosDeFogo(0, -raio, eixoZero);
     addPontosDeFogo(-raio, 0, eixoZero);

     while(y > x){
          if(d < 0){
               d = d + deltaE;
               deltaE = deltaE + 2;
               deltaSE = deltaSE + 2;
          }
          else{
               d = d + deltaSE;
               deltaE = deltaE + 2;
               deltaSE = deltaSE + 4;
               y = y - 1;
          }
          
          // incrementar X e marcar os 8 pixels
          x = x + 1;
          addPontosDeFogo( x,  y, eixoZero);
          addPontosDeFogo(-x, -y, eixoZero);
          addPontosDeFogo( x, -y, eixoZero);
          addPontosDeFogo(-x,  y, eixoZero);
          
          addPontosDeFogo( y,  x, eixoZero);
          addPontosDeFogo(-y, -x, eixoZero);
          addPontosDeFogo( y, -x, eixoZero);
          addPontosDeFogo(-y,  x, eixoZero);  
     }
}






/**********************************************************************/
/*                                                                    */
/*         Adiciona pontos na lista de pontos para explosão           */
/*                                                                    */
/**********************************************************************/
void addPontosDeFogo(int a, int b, int eixoZero){
     pontosFogo *p = new pontosFogo();
     if(eixoZero == eixoX ){
         p->pontos.x = 0;
         p->pontos.y = a;
         p->pontos.z = b;
     }else if(eixoZero == eixoY ){
         p->pontos.x = a;
         p->pontos.y = 0;
         p->pontos.z = b;
     }else{ // eixoZero == eixoZ
         p->pontos.x = a;
         p->pontos.y = b;
         p->pontos.z = 0;
     }                                  
     if(listaPontosFogo != NULL){
        p->prox = listaPontosFogo;
        listaPontosFogo = p;
     }else{
         listaPontosFogo = p;
         p->prox = NULL;
     }
}






/**********************************************************************/
/*                                                                    */
/*     Função que faz tremer a camera quando a nave é atingida        */
/*                                                                    */
/**********************************************************************/
void vibrarCamera(){
     
     if(tremerCamera  &&  camera == cameraF1){
         contTremeCamera++;
         if(contTremeCamera >= 16){
              tremerCamera = false;
              if(camera == cameraF1){
                   cameraX = 500;
                   cameraY = 500;
                   cameraZ = 1900;
              }else{
                   cameraX = nave.translacao.x;
                   cameraY = nave.translacao.y;
                   cameraZ = nave.translacao.z;
              }
         }else{
             if(contTempo%3 == 0){
                 if(flagTremerDirEsq){
                      cameraX = cameraX + 20;
                      alvoCamX = alvoCamX + 20;
                      flagTremerDirEsq = false;
                 }else{
                      cameraX = cameraX - 20;
                      alvoCamX = alvoCamX - 20;
                      flagTremerDirEsq = true;
                 }
                 defineParametrosDeVisualizacao();
             }
         }
     
     }
     
/*     if(tremerCamera){
     
             if(delay == 0){
                 int opcao = contTremeCamera % 4;
                 switch(opcao){
                       case 0:
                            cameraX = cameraX + 500;
                            break;
                       case 1:
                            cameraY = cameraY + 500;
                            break;
                       case 2:
                            cameraX = cameraX - 500;
                            break;
                       case 3:
                            cameraY = cameraY - 500;
                            break;
                       default:
                            break;
                 }
                 contTremeCamera++;
                 defineParametrosDeVisualizacao();
             }else{
                 if(delay >= 10){
                      delay = 0;
                      if(camera == cameraF1){
                           cameraX = 500;
                           cameraY = 500;
                           cameraZ = 1900;
                      }else{
                           cameraX = nave.translacao.x;
                           cameraY = nave.translacao.y;
                           cameraZ = nave.translacao.z;
                      }
                      defineParametrosDeVisualizacao();
                 }
             }
     delay++;
     }          */
}







