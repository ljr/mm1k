/* ------------------------------------------------------------------------ */
/* Programa: MYSOCK.H                                                       */
/* Autor   : Mario Meireles                                                 */
/* Data    : Abr, 2003                                                      */
/* Objetivo:                                                                */
/*      Definicoes para mysock.cpp                                          */
/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* Constantes */
#define MAXHOSTNAME      30
#define BUFSIZE        4096
#define SERV_UDP_PORT  6188
#define SERVIDOR          1
#define CLIENTE           2

/* Codigos de erro */
#define OK   0
#define ERR -1

/* Formato da mensagem */
struct mensagem {
   struct sockaddr_in  cliente;
   struct sockaddr_in  servidor;
   char                buf[BUFSIZE];
};

/* Prototipos de funcao */
int  init_cli  (char hostname[MAXHOSTNAME], struct mensagem *msg);
int  init_serv (struct mensagem *msg);
void send_msg  (int sockfd, struct mensagem msg, int quem);
void recv_msg  (int sockfd, struct mensagem *msg);
