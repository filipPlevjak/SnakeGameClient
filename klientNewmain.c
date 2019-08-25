///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////Klient.c////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <termios.h>

#include "strukruty.h"
#include "CMemLeak.h"

void* vlakno1(void* atr);
void* vlakno2(void* atr);

static struct termios oldt;
void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
    struct termios newt;
    // Make terminal read 1 char at a time 
    tcgetattr(0, &oldt);  // Save terminal settings 
    newt = oldt;  // Init new settings 
    newt.c_lflag &= ~(ICANON | ECHO);  // Change settings 
    tcsetattr(0, TCSANOW, &newt);  // Apply settings 
    atexit(restore_terminal_settings); // Make sure settings will be restored when program ends  
}



char* prijmiPoleHry(DATA* data){

    int polePoctov[2];
    read(data->serverSock,polePoctov,sizeof(int)*2);
    
    char *buffer = malloc(polePoctov[0] * sizeof(int));
    read(data->serverSock, buffer, polePoctov[0]*sizeof(char));
    
    if(buffer[0] == '1'){
    //nastal koniec hry
        data->velkostHada = polePoctov[1];
        
        printf("ukoncila sa hra\n");
        pthread_mutex_lock(data->mutex);
        data->koniec = true;
        pthread_mutex_unlock(data->mutex);
    }
    
    
    return buffer;
}

 void vykresliPole(char* buffer){//bude prijimat char, printf- om vykresli celu plochu   
     //printf("\033");
     system("cls");
     printf("%s\n",buffer+1);
     free(buffer);
     buffer = NULL;
}

void hra(DATA* data){
    while(!(data->koniec)){
        vykresliPole(prijmiPoleHry(data));
    }          
    
} 

int pripojNaServer(){

    struct sockaddr_in serverAddr;
    int socketServera,ret;
    
   socketServera = socket(AF_INET, SOCK_STREAM, 0);
        if(socketServera < 0){
            printf("[-] Error in connection\n");
            exit(1);
        }
    
    printf("[+] Client Socket is created\n");
            
            
    memset(&serverAddr,'\0',sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port=htons(10000);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    ret = connect(socketServera, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
        if(ret <0){
            printf("[-] Error in connection\n");
            exit(1);
        }
    printf("[+] Connected to Server\n");

    return socketServera;
}

int main(int argc, char *argv[]){

    pthread_t thread_1, thread_2;
    pthread_cond_t cond1,cond2;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1,NULL);
    pthread_cond_init(&cond2,NULL);
    
  
    DATA data = {0,0,{0},false,false,0,&mutex,&cond1,&cond2};  
    data.serverSock =  pripojNaServer();
    

    pthread_create(&thread_1,NULL, vlakno1,&data);
    pthread_create(&thread_2,NULL, vlakno2,&data);

    hra(&data);
    
    pthread_join(thread_1,NULL);
    pthread_join(thread_2,NULL);

    printf("Velkost hada %d\n", data.velkostHada);
    sleep(5);
   // system("pause");
}