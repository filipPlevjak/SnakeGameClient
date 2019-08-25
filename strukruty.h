/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   strukruty.h
 * Author: Filip
 *
 * Created on Štvrtok, 2019, januára 17, 21:43
 */

#ifndef STRUKRUTY_H
#define STRUKRUTY_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* STRUKRUTY_H */

#define MAX_HRACIA_PLOCHA 15


typedef struct Data{
    int velkostHada;
    int serverSock;
    char pole[MAX_HRACIA_PLOCHA][MAX_HRACIA_PLOCHA];
    bool vygenerovane;
    bool koniec;
    volatile char klaves;
    pthread_mutex_t *mutex;
    pthread_cond_t *condZadane;
    pthread_cond_t *condPoslane;
}DATA;