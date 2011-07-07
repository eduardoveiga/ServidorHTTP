#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>//open
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>

#define  N 5
#define  MSG_TAM 1025
#define  TAXA_USU 1

// printf("\n\nISSO É UM TESTE MULA!!!!\n\n");

void enviaArquivo (char *arq, FILE *sp,int sock_c, int taxa,char* dados) 
{
  FILE *fp;
  fp = fopen(arq,"r+");
  if(fp == NULL) 
  {
    printf("\nME - Erro! Arquivo não encontrado !");
    printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    exit(1);
  } else 
  {
    printf("  MT - Arquivo encontrado \n");
    printf("  MT - Arquivo: %s \n",arq);
    char *buffer = (char*)malloc(taxa*sizeof(char));
    int result;
    int total=0;
    int flag=1;
//     char c;
    rewind(sp);
    rewind(fp);
    while(flag)
    {
      result = fread(buffer,1,taxa,fp);
      printf("\nLido:\n%s\n\n",buffer);
      if(result <= 0)
      {
	flag=0;
      }
      if(result > 0 && flag ==1) 
      {
	write(sock_c,buffer,result);
//         printf("\n%d\n",result);
      }
      total=total+result;
      printf("\n%d\n",result);
//       c=fgetc(fp);
//       printf("%s",buffer);
//       fputc(c,sp);
    }
    printf("\n\n  MT - Lidos: %d \n",total);
    fclose(fp);
  }
}


char * trataTipoArquivo (char *tipo,struct stat fileinfo) 
{
  time_t rawtime;
  struct tm * timeinfo;
  time_t rawtime2;
  struct tm * timeinfo2;
  char *temp;
  char *retorno;
  char *hora;
  char *tamanho;  
  if(!strcmp(tipo,"png\0")) 
  {
    temp = "image/png";
  } else if (!strcmp(tipo,"pdf\0")) 
  {
    temp = "application/pdf";
  } else if (!strcmp(tipo,"txt\0")) 
  {
    temp = "text/txt";
  } else if (!strcmp(tipo,"gif\0")) 
  {
    temp = "image/gif";
  } else if (!strcmp(tipo,"jpeg\0")) 
  {
    temp = "image/jpeg";
  } else if (!strcmp(tipo,"jpg\0")) 
  {
    temp = "image/jpeg";
  }  else if (!strcmp(tipo,"html\0")) 
  {
    temp = "text/html";
  } else if (!strcmp(tipo,"htm\0")) 
  {
    temp = "text/html";
  }   
  int tam = fileinfo.st_size;
  printf("  MT - TAM: %d \n",tam);
  char  *tamtemp = (char*)malloc(10*sizeof(char));;
  sprintf(tamtemp,"%d",tam);
  char  *date = (char*)malloc(50*sizeof(char));
  char  *LM   = (char*)malloc(50*sizeof(char));
  char *linha1;
  char *linha2;
  char *linha3;
  char *linha4;
  char *linha5;
  char *linha6;
  char *linha7;
  
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime (date,50,"Date: %a, %d %b %Y %X GMT\0",timeinfo);
  printf("\n%s\n",date);
  printf("------------------------------------------------------------------------------------\n");
  
  rawtime2=  fileinfo.st_mtime;  
  timeinfo2 = localtime ( &rawtime2 );
  strftime (LM,50,"Last-Modified: %a, %d %b %Y %X GMT\0",timeinfo2);
  printf("\n%s\n",LM);
  printf("------------------------------------------------------------------------------------\n");
  linha1 = "HTTP/1.1 200 OK\r\n";
  linha2 = "Connection: close\r\n";
  linha4 = "Server: Bizucaserver \r\n";
  retorno = (char*)malloc(500*sizeof(char));
  sprintf(retorno, "%s%s%s%s%s%s%s%s%s%s%s%s%s"  , linha1 , linha2, date, "\r\n", linha4, LM, "\r\n", "Content-Length: ",tamtemp, "\r\n", "Content-Type: ",temp,"\r\n\r\n");
  return retorno;
}

void trataArquivo (char *arq, FILE *sp, int sock_c) 
{
  
  struct stat fileinfo;
  char *tipoarquivo;
  char *linha1;
  char *linha2;
  char *linha3;
  char *linha4;
  char *dados;
  int  taxa = 0;
  taxa = (TAXA_USU*1024);
  dados = (char*)malloc(taxa*sizeof(char));
  char *arquivo = (char*)malloc(100*sizeof(char));
  strcpy(arquivo,arq);
  if (stat(arq,&fileinfo ) == -1 )
  {
    printf("\nME - Erro! Criação stat() !");
    printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    exit(1);
  }
  tipoarquivo = strtok(arq,".");
  tipoarquivo = strtok(NULL,"\r");
  printf("  MT - Tipo Arquivo: %s \n",tipoarquivo);
  dados = trataTipoArquivo(tipoarquivo,fileinfo);
  printf("  MT - DADOS: \n\n");
  printf("%s",dados);
  printf("\n\n------------------------------------------------------------------------------------\n");
  write(sock_c,dados,strlen(dados));
  enviaArquivo(arquivo,sp,sock_c,taxa,dados);
  printf(" \n\n");
}

void trataMensagem (char *mensagem,FILE *sp,int sock_c) 
{
  char *metodo;
  char *arquivo;
  char *protocolo;
  char *tempoVivo;
  char *retorno;
  printf("MR - %s",mensagem);
  metodo = strtok(mensagem," ");      
  if(!strcmp(mensagem,"GET\0")) 
  {
    printf("---------------------- %s ---------------------- \n",metodo);
    printf(" MT - Método: %s \n",metodo);
    arquivo = strtok(NULL, " ");
    printf(" MT - Arquivo: %s \n",arquivo);
    protocolo = strtok(NULL, "\r");
    printf(" MT - Protocolo: %s \n\n",protocolo);
    if(!strcmp(protocolo,"HTTP/1.1\0")) 
    {
      printf("  MT - Protocolo Aceito\n",protocolo);
      trataArquivo(arquivo,sp,sock_c);
    } else 
    {
      printf("\nME - Erro! Protocolo não Suportado!");
      printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      exit(1);
    }
  } else
  {
    printf("\n---------------------- %s ---------------------- \n",metodo);
  }
}



int main(int argc, char  *argv[])
{
  printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
  printf("XXXXXX Servidor HTTP - Thiago Martins e Eduardo Veiga - Redes de Computadores 2011/01 XXXXXX\n\n");
  /* Variáveis */
  int socket_c, cliente_s;
  int PORTA;
  int TAXA_MAX;
  char *porta;
  char *taxa_max;
  int TAM_CLI;
  struct sockaddr_in cliente;
  TAM_CLI = sizeof(cliente);
  char *MSG_REC;
  MSG_REC = (char*)malloc(MSG_TAM*sizeof(char));
  
  if(argc == 3) {
    porta    = argv[1];
    taxa_max = argv[2];
    PORTA    = atoi(porta);
    TAXA_MAX = atoi(taxa_max);
    printf("MS - Status      : Servidor ON\n");
    printf("MS - Taxa Maxima : %d \n",TAXA_MAX);
    printf("MS - Porta       : %d \n",PORTA);
    /* Cria socket */
    socket_c = socket(AF_INET,SOCK_STREAM,0);
    if(socket_c < 0) 
    {
      printf("\nME - Erro! Criação do socket!");
      printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      exit(1);
    }
    bzero(&cliente, sizeof(cliente));
    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(PORTA);
    cliente.sin_addr.s_addr = INADDR_ANY;
    if(bind(socket_c, (struct sockaddr*)&cliente, TAM_CLI) == -1) {
      printf("\n ME - Erro! Bind!");
      printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      exit(1);    
    }
    
    if(listen(socket_c,N) == -1) {
      printf("\n ME - Erro! Listen!");
      printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      exit(1); 
    }
    
    
    while (1) {
      cliente_s = accept(socket_c, (struct sockaddr*)&cliente, &TAM_CLI);
      FILE *fp = fdopen(cliente_s,"r+");
      printf("MS - Cliente Conectado : %d \n",cliente_s);
      printf("---------------------- Tratamento ---------------------- \n");
      do {
        fgets(MSG_REC,MSG_TAM,fp);
	trataMensagem(MSG_REC, fp, cliente_s);
      } while (strcmp(MSG_REC,"\r\n\0"));
      fclose(fp);
    }
    close(socket_c);
    
    
    
  } else {
    printf("\n ME - Erro! Falta Parâmetros!");
  }
  printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
  printf("\n\n");
  
  return 0;
}