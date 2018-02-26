#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h> 
#define PORT 2098
extern int errno;
char message[50];
char history[500];
static int call(void *data,int argc, char **argv, char **azColName) // nothting to do 
{ 
  if (argv==NULL)
     return 0;
  else
     return 4;
}

static int callback(void *data, int argc, char **argv, char **azColName)
{   
   bzero(message,50);
   int i;
   for(i=0; i<argc; i++)
   { 
     if(argv[i])
     {
        strcat(message,argv[i]);
     }
   }
   return 0;
}
static int callback2(void *data, int argc, char **argv, char **azColName)
{   
   int i;
   for(i=0; i<argc; i++)
   { 
     if(argv[i])
     {
        strcat(history,argv[i]);
     }
   }
   return 0;
}
int makeUserOnline(char userName[30])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"INSERT into online_users (username) values('");
    strcat(sql,userName);
    strcat(sql,"')");
    rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
    if( rc != SQLITE_OK && rc!=4)
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } 
    else 
    {
      fprintf(stdout, "Adaugat la lista utilizatorilor conectati \n");
    }
    return rc;
    
}
int makeUserOffline(char userName[30])
{  
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
     strcpy(sql,"DELETE from online_users WHERE username='");
    strcat(sql,userName);
    strcat(sql,"'");
   rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
   if( rc != SQLITE_OK && rc!=4)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Adaugat la lista utilizatorilor deconectati \n");
   }  
   return rc;
  }

void clearTable(char tableName[30])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"DELETE from ");
    strcat(sql,tableName);
   rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
   if( rc != SQLITE_OK && rc!=4)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } 
  
}
void querryAnswer(int rc, char answer[100],char*zErrMsg)
{
   if( rc != SQLITE_OK && rc!=4) 
     {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
     }
     else 
     {
       fprintf(stdout,"%s\n", answer);
     }    
}
int checkAlreadyOnline(char userName[30])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"Select username from  online_users where username='");
    strcat(sql,userName);
    strcat(sql,"'");
    rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
    if( rc != SQLITE_OK && rc!=4)
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }   
    return rc;
}
void addMessage(char userName[30], char toUser[30], char mesaj[50])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"INSERT into mesaje(de_la,mesaj,catre) values('");
    strcat(sql,userName);
    strcat(sql,"','");
    strcat(sql,mesaj);
    strcat(sql,"','");
    strcat(sql,toUser);
    strcat(sql,"')");
    rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
    if( rc != SQLITE_OK && rc!=4)
    {
      fprintf(stderr, "SQL error mesaj: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } 
}
  
void sentMessage(char userName[30],int client)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"SELECT de_la || ':' || mesaj from mesaje where catre ='");
    strcat(sql,userName);
    strcat(sql,"'");
    rc = sqlite3_exec(db, sql, callback,NULL, &zErrMsg);
    if( rc != SQLITE_OK && rc!=4)
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    if(strlen(message) == 0)
    { 
      if( write(client,"Nu aveti niciun mesaj!\n",50)<=0)
      {

       perror ("[client]Eroare la write sentMessage | no message spre server.\n");
       exit(-1);
      }
     
    }
   else  
   if (write (client,message,50) <= 0)
   { 
       perror ("[client]Eroare la write sentMessage spre server.\n");
       exit(-1);
   }

}      
    
void sentHistory(char userName[30],int client)
{ 
    bzero(history,500);
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("baza.db", &db);
    sql=(char*)malloc(100);
    bzero(sql,100);
    strcpy(sql,"SELECT de_la || '->'|| catre || ':' || mesaj from mesaje where (catre ='");
    strcat(sql,userName);
    strcat(sql,"'");
    strcat(sql,"OR de_la ='");
    strcat(sql,userName);
    strcat(sql,"')");
    rc = sqlite3_exec(db, sql, callback2,NULL, &zErrMsg);
    if( rc != SQLITE_OK && rc!=4)
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
   if(strlen(history) == 0)
    { 
      if(write(client,"Nu aveti niciun mesaj in istoric!\n",500) <=0)
      {
        perror ("[client]Eroare la write no history () spre server.\n");
        exit(-1);
      }
    }
   else  
   if (write (client,history,500) <= 0)
   { 
       perror ("[client]Eroare la write history spre server.\n");
       exit(-1);
   }
}  
  
void selectOption(int client,int optiune)
{  
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   rc = sqlite3_open("baza.db", &db);
   if(rc) 
   {
      fprintf(stderr, "Can't open bazabase: %s\n", sqlite3_errmsg(db));
      exit(-1);
   } 
  else 
   {
      fprintf(stderr, "Opened database successfully\n");
   }
    
  if (read (client, &optiune,sizeof(optiune)) <= 0)
  {
    perror ("[server]Eroare la read() optiune de la client.\n");
    close (client); 
  }

  // Pregatim structurile si descriptorii pentru a accesa baza de date
  char userName[30];
  char password[30];
  char answer[150];
  int online_status;
  
  switch(optiune)
  {
     case 1:  // LOGIN  
     if (read (client, userName, 30) <= 0)
     {
        perror ("[server]Eroare la read userName de la client.\n");
        close (client); 
     }
     if (read (client, password, 30) <= 0)
     {
         perror ("[server]Eroare la read parola de la client.\n");
         close (client); 
     }
    
     // Verificam daca userul exista in baza de date
     sql=(char*)malloc(100);
     bzero(sql,100);
     strcat(sql,"SELECT * from USERS WHERE userName='");
     strcat(sql,userName);
     strcat(sql,"' AND password='");
     strcat(sql,password);
     strcat(sql,"'");
     rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
     
     //Verificam daca nu exista deja un user cu acelasi  nume conectat
     if(checkAlreadyOnline(userName) !=  0)
     { 

       bzero(answer,150);
       strcat(answer,"Exista deja un utilizator cu acest nume conectat");
       if (write (client,answer,150) <= 0)
       {
            perror ("[client]Eroare la write aff() spre server.\n");
            exit(-1);
       }
        exit(-1);  
        close(client);  
      }
    
     if(rc ==4 ) //USER LOGAT CU SUCCES 
     { 
       int online_status=makeUserOnline(userName); 
       bzero(answer,150);
       strcat(answer,"Te-ai logat cu succes!\nDoriti sa consultati meniul?");
       if (write (client,answer,150) <= 0)
       {
            perror ("[client]Eroare la write() spre server.\n");
            exit(-1);
       }
       
      // Clientul alege daca sa folosesca meniul sau nu 
      int meniu_accesat;
      if (read (client, &meniu_accesat,sizeof(meniu_accesat)) <= 0)
      {
         perror ("[server]Eroare la read() de la client333.\n");
         close (client);  
      }
      if(meniu_accesat == 0) 
      { 
        printf("Un client a ales deconectarea \n");
        online_status=makeUserOffline(userName);
        close(client);
      }
      else // acceseaza meniul 
      { 
        int optiuneMeniu=1; 
        while(optiuneMeniu != 0)
        {
         fflush(stdout);
         fflush(stdin);
         bzero(message,50);
         bzero(history,100);
        if (read (client, &optiuneMeniu,sizeof(optiuneMeniu)) <= 0)
        {
           perror ("[server]Eroare la read() de la client.\n");
           close (client);  
        }
        
        if(optiuneMeniu == 0) 
           makeUserOffline(userName);
        if(optiuneMeniu == 1) // vrea sa vb cu cineva
        {
          char toUser[30];
          bzero(toUser,30);
          if (read (client, toUser, 30) <= 0)
          {
            perror ("[server]Eroare la read() de la client.\n");
            close (client); 
          }
         
          if(checkAlreadyOnline(toUser) != 0) // userul cu care vrea sa  vb  e online
          { 
            bzero(answer,150);
            strcpy(answer,"Userul ales este conectat!\n");
            if (write (client,answer,150) <= 0)
            {
              perror ("[client]Eroare la write() spre server.\n");
              exit(-1);
            }

          }
          else // este offline
          { 
            //addMessage(toUser,userName,"Im away");
            bzero(answer,150);
            strcpy(answer,"Userul ales este deconectat!\n");
            if (write (client,answer,150) <= 0)
            {
              perror ("[client]Eroare la write() spre server.\n");
              exit(-1);
            }
          }

          // citim mesajul si il inseram in baza de date
          char mesaj[50];
          bzero(mesaj,50);
          if (read (client, mesaj,50) <= 0)
          {
            perror ("[server]Eroare la read() de la client.\n");
            close (client); 
          }
          if(strcmp(mesaj,"Mesajul este prea mare! \n") == 0)
          { 
            online_status=makeUserOffline(userName);
            exit(-1);
            close(client);
          }
          else
          {
           addMessage(userName, toUser, mesaj);
          }
  

        } // optiuneMeniu=1
       else if(optiuneMeniu==2) // HYSTORY
        { 
          sentHistory(userName,client);
        }
        else 
        if(optiuneMeniu == 3) // NOTIFICATION
        {     
             sentMessage(userName,client);
             
             if(strcmp(message,"Nu aveti niciun mesaj!\n")!=0)
            { 
              int reply;
             if (read (client, &reply,sizeof(reply)) <= 0)
             {
               perror ("[server]Eroare la read() de la client12.\n");
               close (client); 
             }
             else 
             { 
             if(reply ==1) 
             { 
               char toUser[30]; 
               bzero(toUser,30);
               int i=0;
            
              while(( message[i] != ':') && i <strlen(message) )
              {   
                  strncat(toUser,(message+i),1);
                  i++;
              }
              
              char mesajReply[50];
              bzero(mesajReply,50);
              if (read (client, mesajReply,50) <= 0)
              {
               perror ("[server]Eroare la read() de la client.\n");
               close (client); 
              }
             if(strcmp(mesajReply,"Mesajul este prea mare! \n") == 0)
              { 
                 online_status=makeUserOffline(userName);
                 exit(-1);
                 close(client);
              }
              else
              {
                addMessage(userName, toUser, mesajReply);
              } 
            }
            
          }
        }
      } 
     // optiuneMeniu=3
      }// daca a select notificaton
     } // access Meniu
     }
    else  if(rc ==0)
    {  
        bzero(answer,50);
        strcat(answer,"Nu exista un user cu acest nume/parola!");
        if (write (client,answer,50) <= 0)
          {
            perror ("[client]Eroare la write() spre server.\n");
            exit(-1);
          } 
          close(client);
    }   
    break;



    case 2: // INREGISTRARE
       
       if (read (client, userName, 30) <= 0)
      {
        perror ("[server]Eroare la read() de la client123.\n");
        close (client); 
      }
    
      if (read (client, password, 30) <= 0)
      {
         perror ("[server]Eroare la read() de la client.\n");
         close (client);  
      }
    
     // Verific daca user-ul este deja folosit
     sql=(char*)malloc(100);
     bzero(sql,100);
     strcat(sql,"SELECT * from USERS WHERE userName='");
     strcat(sql,userName);
     strcat(sql,"'");
     rc = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
     if(rc ==4)
     {
       bzero(answer,50);
       strcat(answer,"Exista un alt user cu acest nume!\n");
       if (write (client,answer,50) <= 0)
          {
            perror ("[client]Eroare la write() spre server.\n");
            exit(-1);
          } 
     }
     
     else if(rc==0)
     { 
        sql=(char*)malloc(100);
        bzero(sql,100);
        strcpy(sql,"INSERT into USERS(userName,password) values('");
        strcat(sql,userName);
        strcat(sql,"','");
        strcat(sql,password);
        strcat(sql,"')");
        int rc2;
        rc2 = sqlite3_exec(db, sql, call, NULL, &zErrMsg);
        if( rc2 != SQLITE_OK && rc2!=4) 
       {
           fprintf(stderr, "SQL error: %s\n", zErrMsg);
           sqlite3_free(zErrMsg);
       }
       else 
       {
          fprintf(stdout, "Am inregistrat un nou user !\n");
          bzero(answer,50);
          strcat(answer,"Ai fost inregistrat !\n");
          if (write (client,answer,50) <= 0)
          {
            perror ("[client]Eroare la write() spre server.\n");
            exit(-1);
          } 

       }    
        
     }
        
    break;
    case 3:
       printf("Un client a ales deconectarea \n");
       online_status=makeUserOffline(userName);
       // trebuie scos si de pe lista cu descriptori 
       close(client);
       exit(-1);
       break;
    default:
       printf("Optiune introdusa gresit !\n");
       close(client);
       exit(-1);
       break;

      }
    }
    



int main ()
{
  struct sockaddr_in server;
  struct sockaddr_in from;  
  int sd;     
  clearTable("online_users");
  clearTable("users");
  clearTable("mesaje");
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  server.sin_family = AF_INET;  
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  

  while (1)
    {
      int client;
      int length = sizeof (from);
      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);
      client = accept (sd, (struct sockaddr *) &from, &length);
      if (client < 0)
      {
        perror ("[server]Eroare la accept().\n");
        continue;
      }
      pid_t pd;
      pd = fork();

      if(pd == 0)
      {
       fflush (stdout);  
       int optiune=0;
       while(optiune!=1 && optiune!=2 && optiune!=3)
       { 
         selectOption(client,optiune);
       }

      } 
    }
  
}       