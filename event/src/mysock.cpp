/* ------------------------------------------------------------------------ */
/* Programa: MYSOCK.CPP                                                     */
/* Autor   : Mario Meireles                                                 */
/* Data    : Abr, 2003                                                      */
/* Funcoes : init_cli(), init_serv(), send_msg(), recv_msg()                */
/* Objetivo:                                                                */
/*      Implementa funcoes do tipo send/receive que escondem dos            */
/*      usuarios a complexidade de utilizacao dos sockets                   */
/* ------------------------------------------------------------------------ */

#include "mysock.h"
#include "iostream.h"

/*  Inicializa o socket no lado do cliente, preenchendo a
 *  "struct mensagem" com os enderecos apropriados do cliente
 *  e do servidor
 */

int init_cli(char hostname[MAXHOSTNAME], struct mensagem *msg)
{
   int sockfd;
   struct hostent *hostp;

   /* --- Abre um UDP socket --- */

   if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
      printf("Nao foi possivel abrir UDP socket no cliente\n");  exit(1);
   }

   /* --- Faz o bind do endereco local (cliente) --- */

   bzero((char *) &msg->cliente,sizeof(msg->cliente));
   msg->cliente.sin_family      = AF_INET;
   msg->cliente.sin_addr.s_addr = htonl(INADDR_ANY);
   msg->cliente.sin_port        = htons(0);

   if (bind(sockfd,(struct sockaddr *) &msg->cliente,
            sizeof(msg->cliente)) < 0) {
      printf("Nao foi possivel fazer o bind no cliente\n");  exit(1);
   }

   /* --- Preenche a estrutura com o endereco do servidor com
          o qual o cliente deseja se comunicar                --- */

   hostp = gethostbyname(hostname);
   if (hostp == 0) {
      printf("Host \'%s\' desconhecido\n",hostname);  exit(1);
   }
   bzero((char *) &msg->servidor,sizeof(msg->servidor));
   bcopy(hostp->h_addr,&msg->servidor.sin_addr,hostp->h_length);
   msg->servidor.sin_family = AF_INET;
   msg->servidor.sin_port   = htons(SERV_UDP_PORT);

   /* --- Retorna o identificador do socket --- */

   return (sockfd);

}


/*
 *  Inicializa o socket do lado do servidor
 */

int init_serv(struct mensagem *msg)
{
   int sockfd;

   /* --- Abre um UDP socket --- */

   if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
      printf("Nao foi possivel abrir UDP socket no servidor\n");  exit(1);
   }

   /* --- Faz o bind do endereco do servidor para que o cliente
          possa se comunicar com ele                            --- */

   bzero((char *) &msg->servidor,sizeof(msg->servidor));
   msg->servidor.sin_family      = AF_INET;
   msg->servidor.sin_addr.s_addr = htonl(INADDR_ANY);
   msg->servidor.sin_port        = htons(SERV_UDP_PORT);

   if (bind(sockfd,(struct sockaddr *) &msg->servidor,
            sizeof(msg->servidor)) < 0) {
      printf("Nao foi possivel fazer o bind no servidor\n");  exit(1);
   }

   /* --- Retorna o identificador do socket --- */

   return(sockfd);

}


/*
 *  Envia a mensagem para o destino especificado
 */

void send_msg(int sockfd, struct mensagem msg, int quem)
{
   socklen_t tam;

   if (quem == SERVIDOR) {
      tam = sizeof(msg.servidor);
      sendto(sockfd,(char *) &msg,sizeof(struct mensagem),0,
                    (struct sockaddr *) &msg.servidor,tam);
   }
   else {   /* CLIENTE */
      tam = sizeof(msg.cliente);
      sendto(sockfd,(char *) &msg,sizeof(struct mensagem),0,
                    (struct sockaddr *) &msg.cliente,tam);
   }
}


/*
 *  Recebe uma mensagem
 */

void recv_msg(int sockfd, struct mensagem *msg)
{
   socklen_t tam;

   recvfrom(sockfd,(char *) msg,sizeof(struct mensagem),0,
                   (struct sockaddr *) &msg->cliente,&tam);
   //cout << "tamlen: " << tam << "\n";
}



