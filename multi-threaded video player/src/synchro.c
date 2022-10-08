#include "synchro.h"
#include "ensitheora.h"
#include <pthread.h>

extern bool fini;

/* les variables pour la synchro, ici */
bool taillesent = false;
bool windowReady = false;
int nbrTextures = 0;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&mutexVideo);
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;
    taillesent = true;
    pthread_cond_signal(&condTaille);
    pthread_mutex_unlock(&mutexVideo);
}

void attendreTailleFenetre() {
    pthread_mutex_lock(&mutexVideo);
    while(!taillesent)
        pthread_cond_wait(&condTaille, &mutexVideo);
    pthread_mutex_unlock(&mutexVideo);
}

void signalerFenetreEtTexturePrete() {
    pthread_mutex_lock(&mutexVideo);
    windowReady = true;
    pthread_cond_signal(&condFenetre);
    pthread_mutex_unlock(&mutexVideo);
}

void attendreFenetreTexture() {
    pthread_mutex_lock(&mutexVideo);
    while(!windowReady)
        pthread_cond_wait(&condFenetre, &mutexVideo);
    pthread_mutex_unlock(&mutexVideo);
}

void debutConsommerTexture() {
    pthread_mutex_lock(&mutexVideo);
    while (nbrTextures == 0)
        pthread_cond_wait(&condTexture, &mutexVideo);
    pthread_mutex_unlock(&mutexVideo);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutexVideo);
    nbrTextures--;
    pthread_cond_signal(&condTexture);
    pthread_mutex_unlock(&mutexVideo);

}


void debutDeposerTexture() {
    pthread_mutex_lock(&mutexVideo);
    while(nbrTextures==NBTEX)
        pthread_cond_wait(&condTexture, &mutexVideo);
    pthread_mutex_unlock(&mutexVideo);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutexVideo);
    nbrTextures++;
    pthread_cond_signal(&condTexture);
    pthread_mutex_unlock(&mutexVideo);
}
