#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include "strukruty.h"


void disable_waiting_for_enter(void);


void* vlakno1(void* atr){
    DATA *data = (DATA*)atr;

        while(1){
            pthread_mutex_lock(data->mutex); // lockne sa aby sa náhodou nezmenila premenná klaves.
                while(data->vygenerovane == false){ // ak ešte neni vygenerované bude sa čakať teda nić sa nepošle
                
                    pthread_cond_wait(data->condZadane,data->mutex);
                }
               
            char znak =  data->klaves; 
            data->vygenerovane = false; // nemusí byť volatile lebo je locknutá všade kde sa použiva.
            write(data->serverSock ,&znak, sizeof(char));
            pthread_mutex_unlock(data->mutex); // už získal zadaný znak zatiaľ ho pošle kym sa príjme ďalší
            pthread_cond_signal(data->condPoslane); // poslal vlaknu2 echo že už to poslal. čaká na ďalšiu voľbu
                
            pthread_mutex_lock(data->mutex);
            
                if(data->koniec == true){
                    pthread_mutex_unlock(data->mutex);
                    return NULL;  // ak nastal koniec // nastaví sa to v inom vlákne kde príjme stav konca zo servera.. pravdepodobne vlákno mainu kde príjma vykreslenie hry od servera.
                }
                  
            pthread_mutex_unlock(data->mutex); 
        }

    return NULL;
}

//sluzi na nacitavanie znakov z klavesnice
void* vlakno2(void* atr){
    DATA *data = (DATA*)atr;
     
    disable_waiting_for_enter();

    /* Key reading loop */
    while (1) {
        pthread_mutex_lock(data->mutex); 
            while(data->vygenerovane == true){
                pthread_cond_wait(data->condPoslane,data->mutex);
            }
            if(data->koniec == true)
            {
                pthread_mutex_unlock(data->mutex);
                return NULL;   // ak nastal koniec // nastaví sa to v inom vlákne kde príjme stav konca zo servera.. pravdepodobne vlákno mainu kde príjma vykreslenie hry od servera.
            }
        data->klaves = getchar();
        data->vygenerovane = true;
        pthread_mutex_unlock(data->mutex);
        
        pthread_cond_signal(data->condZadane); 
        pthread_mutex_lock(data->mutex);
        
        pthread_mutex_unlock(data->mutex);
    }
    return NULL;  
}