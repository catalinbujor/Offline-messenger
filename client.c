#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
extern int errno;
int port;

void sendServer(int sd,int optiune)
{  
    fflush(stdout);
    if(write(sd,&optiune,sizeof(optiune))<=0)
    {
        perror("Eroare la write catre server\n");
        exit(-1);
    }
}
void receiveServer(int sd, int answer)
{ 
  if(read(sd,&answer,sizeof(int))<=0)
  {
      perror("Eroare la read de la server\n");
      exit(-1);
   }
}

int optiuneUser(int sd)
{
    int optiune;
    optiune=0;
    printf("Pentru logare tastati 1\n");
    printf("Pentru inregistrare tastati 2\n");
    printf("Pentru deconectare tastati 3\n");
    scanf("%d",&optiune);
    sendServer(sd,optiune);
    return optiune;
}
void meniuOnline(int sd)
{ 
   int optiuneMeniu=1;
   while(optiuneMeniu == 1 || optiuneMeniu==2 || optiuneMeniu==3)
   {
   printf("Tastati @nume_user@ pentru a trimite mesaj (50 caractere maxim) \n");
   printf("Tastati history pentru a vizualiza istoricul mesajelor\n");
   printf("Tastati notification pentru a vedea lista mesajelor primite\n");
   fflush(stdout);
   fflush(stdin);
   char comanda[30];
   bzero(comanda,30);
   scanf("%s",comanda);
  
   //INTREPRETAREA MENIULUI
  
  if(comanda[0] == '@' && comanda[strlen(comanda)-1] == '@')
  {  
      optiuneMeniu=1;
      sendServer(sd,optiuneMeniu);

      char toUser[30]; 
      bzero(toUser,30);
      int i=1;
      while(strcmp(comanda+i,"@")!=0)
      {
            strncat(toUser,(comanda+i),1);
            i++;
      }
        
      if (write (sd,toUser,30) <= 0)
      {
         perror ("[client]Eroare la write() spre server.\n");
         exit(-1);
      } 
     
      char answer[150];
      bzero(answer,150);
      if (read (sd, answer, 150) <= 0)
       {
           perror ("[server]Eroare la read() de la client11.\n");
           exit(-1);
       }
        
      printf("%s\n",answer);
      char mesaj[50];
      bzero(mesaj,50);
      read(0,mesaj,50);
      if(strlen(mesaj) >45)
      {  
         strcpy(mesaj,"Mesajul este prea mare! \n");
         printf("Mesajul este prea mare! \n");
         if (write (sd,mesaj,50) <= 0)
         { 
          perror ("[client]Eroare la write() spre server.\n");
          exit(-1);
         }
         exit(-1);
      }
      else  
     {
      if (write (sd,mesaj,50) <= 0)
      { 
         perror ("[client]Eroare la write() spre server.\n");
         exit(-1);
      }
      printf("Sent!\n");
     }
  }
  
  else 
  if(strcmp(comanda,"history") == 0)
  { 
    optiuneMeniu=2;
    sendServer(sd,optiuneMeniu);
    char history[500];
    bzero(history,500);
    if (read (sd,history, 500) <= 0)
    {
      perror ("[server]Eroare la read() de la client11.\n");
      exit(-1);
    }
    printf("%s\n",history);

  }
  else 
  if(strcmp(comanda,"notification") == 0)
  {
    optiuneMeniu=3;
    sendServer(sd,optiuneMeniu);

    char mesaj[50];
    bzero(mesaj,50);
    if (read (sd,mesaj, 50) <= 0)
    {
      perror ("[server]Eroare la read() de la client11.\n");
      exit(-1);
    }
    printf("%s\n",mesaj);

    // REPLY
    if(strcmp(mesaj,"Nu aveti niciun mesaj!\n") !=0)
    { 
    printf("Doriti sa raspundeti ? 1/da\n");
    int reply;
    scanf("%d",&reply);
    sendServer(sd,reply);

    if(reply == 1)
    {
      char mesajReply[50];
      bzero(mesajReply,50);
      read(0,mesajReply,50);
      if(strlen(mesajReply) >45)
      {  
         strcpy(mesajReply,"Mesajul este prea mare! \n");
         printf("Mesajul este prea mare! \n");
         fflush(stdout);
         if (write (sd,mesajReply,50) <= 0)
         { 
          perror ("[client]Eroare la write() spre server.\n");
          exit(-1);
         }
         exit(-1);
      }
      else  
      {fflush(stdout);
      if (write (sd,mesajReply,50) <= 0)
      { 
         perror ("[client]Eroare la write() spre server.\n");
         exit(-1);
      }
      printf("Sent!\n");
      } 
    
     }
    
    }

  }
else 
 {
   optiuneMeniu=0;
   sendServer(sd,optiuneMeniu);

 }
  }
}


int main (int argc, char *argv[])
{
  int sd;         
  struct sockaddr_in server;  
  char msg[100];     
  if (argc != 3)
  {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
  }
  
  port = atoi (argv[2]);
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror ("Eroare la socket().\n");
      return errno;
   }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
     perror ("[client]Eroare la connect().\n");
     return errno;
  }
  fflush (stdout);  
  
  char userName[30];
  char password[30];
  char answer[150];
  while(1)
  {
    int optiune=0;
    while(optiune!=1 && optiune!=2 && optiune!=3)
    {
        optiune=optiuneUser(sd); //trimit optiunea la server
        switch(optiune)
       {
        case 1:  // LOGIN
  
          printf("Introduceti username (maxim 30 de caractere) \n");
          scanf("%s",userName);
          while(strlen(userName) > 30) 
          {
            printf("User-ul ales contine prea multe caractere !\nIncercati inca o data:\n");
            scanf("%s",userName);
          }
          if (write (sd,userName,30) <= 0)
          {
               perror ("[client]Eroare la write() spre server.\n");
               exit(-1);
          }
              
          printf("Introduceti parola (maxim 30 de caractere) \n");
          scanf("%s",password);
          while(strlen(password) > 30) 
          {
            printf("User-ul ales contine prea multe caractere !\nIncercati inca o data:\n");
            scanf("%s",password);
          }
          if (write (sd,password,30) <= 0)
          {
               perror ("[client]Eroare la write() spre server.\n");
               exit(-1);
          }
              
          // Urmeaza read si apoi mesaj in functie de rasapuns
          bzero(answer,150);
          if (read (sd, answer, 150) <= 0)
          {
            perror ("[server]Eroare la read() de la client11.\n");
            exit(-1);
           
          }
          
           printf("%s\n",answer);
           if((strcmp(answer,"Nu exista un user cu acest nume/parola!") == 0)
           || (strcmp(answer,"Exista deja un utilizator cu acest nume conectat") == 0))
           {
              printf("Urmeaza deconectarea...\n");
              exit(-1);
           }
           

           char raspuns[1];
           int meniu_accesat;
           bzero(raspuns,1);
           scanf("%s",raspuns);
           if(strcmp(raspuns,"da") == 0) 
           { 
             meniu_accesat=1;
             sendServer(sd,meniu_accesat);
             meniuOnline(sd);              
             
           }
           else
            { 
              meniu_accesat=0;
              sendServer(sd,meniu_accesat);
              close(sd);
              exit(-1);
            }
     
           // Odata logat trebuie sa creez meniu pentru celelte actiuni
           // Aici trebuie adaugate si mesajele primite 
           
           break;
           
        case 2: // INREGISTRARE
          printf("Alege username (maxim 30 de caractere) \n");
          scanf("%s",userName);
          while(strlen(userName) > 30) 
          {
            printf("User-ul ales contine prea multe caractere !\nIncercati inca o data:\n");
            scanf("%s",userName);
          }
          if (write (sd,userName,30) <= 0)
          {
               perror ("[client]Eroare la write() spre server.\n");
               exit(-1);
          }
              
          printf("Alege parola (maxim 30 de caractere) \n");
          scanf("%s",password);
          while(strlen(password) > 30) 
          {
            printf("User-ul ales contine prea multe caractere !\nIncercati inca o data:\n");
            scanf("%s",password);
          }
          if (write (sd,password,30) <= 0)
          {
               perror ("[client]Eroare la write() spre server.\n");
               exit(-1);
          }
              
          // urmeaza read si apoi mesaj in functie de rasapuns
          bzero(answer,50);
          if (read (sd, answer, 50) <= 0)
         {
            perror ("[server]Eroare la read() de la client105.\n");
            exit(-1);
           
          }
  
           printf("%s\n",answer);
           break;

        case 3: // DECONECTARE
          printf("Urmeaza deconectarea...\n");
          exit(-1);
          break;
        default:
          printf("Optiune gresita aplicatia se va inchide..\n");
          close(sd);
          exit(-1);
          break;

      }
         
  }
}
}