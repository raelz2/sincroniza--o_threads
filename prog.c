#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define tam 7

pthread_mutex_t trava = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_barrier_t barrier;

typedef struct FilaCircular{
    int pri,ult;
    int buffer[tam];//numero do aluno
    int priority[tam];
}FilaCircular;

typedef struct{
    int num;
    char nome[10];
}Aluno;

FilaCircular fc={0};
void iniciafc(){
    fc.pri=0;
    fc.ult=0;
    fc.priority[1] = 1;
    fc.priority[2] = 1;
    fc.priority[3] = 2;
    fc.priority[4] = 2;
    fc.priority[5] = 3;
    fc.priority[6] = 3;
}


void adicionafila(Aluno *al){
    
    fc.buffer[fc.ult] = al->num;
    fc.ult = (fc.ult + 1)%tam;
    printf("fila: ");
    for(int i=fc.pri;i!=fc.ult;i = (i+1)%tam){
        printf("%d ", fc.buffer[i]);
    }
    printf("\n");
}

void removefila(Aluno *al){
    int pri = fc.pri,ult = fc.ult;
    while(pri!=ult){
        if(fc.buffer[pri] == al->num){
            //fc.buffer[pri] = fc.buffer[(pri+1)%tam];
            break;
        }
        pri = (pri+1)%tam;
    }
    while(pri!=ult){
        fc.buffer[pri] = fc.buffer[(pri+1)%tam];
        pri = (pri+1)%tam;
    }
    fc.ult = (fc.ult+6)%tam;
    printf("fila: ");
    for(int i=fc.pri;i!=fc.ult;i = (i+1)%tam){
        printf("%d ", fc.buffer[i]);
    }
    printf("\n");
}

int sou_eu(Aluno *al){
    int prior = 3,num = al->num;
    int pri = fc.pri,ult = fc.ult;
    while(pri!=ult){
        if(fc.priority[fc.buffer[pri]] < prior){
            prior = fc.priority[fc.buffer[pri]];
            num = fc.buffer[pri];
        }
        pri = (pri+1)%tam;
    }
    //pri = maior prioridade(menor numero)
    //num = numero do aluno
    if(num == al->num)
        return 1;
    else
        return 0;
}

void* pessoa(void* args){
    pthread_barrier_wait(&barrier);
    Aluno* al = (Aluno*) args;
    int c=0,cont;
    double tmp;
    unsigned int t;
    srand(time(NULL)+al->num);
    while(c < 3){
        
        tmp = (double)rand()/((double) RAND_MAX/(5-0));
        //printf("tempo: %lf\n", tmp);
        sleep(tmp);
        pthread_mutex_lock(&trava);
            printf("%s esta esperando para usar o forno\n", al->nome);
            adicionafila(al);
            
        pthread_mutex_unlock(&trava);
        tmp = (double)rand()/((double) RAND_MAX/(5-0));
        //printf("tempo: %lf\n", tmp);
        sleep(tmp);
        pthread_mutex_lock(&trava);
            cont=0;
            while(1){
                if(sou_eu(al)){
                    printf("%s esta usando o forno\n", al->nome);
                    //tmp = rand()%3;
                    //sleep(tmp);
                    tmp = (double)rand()/((double) RAND_MAX/(3-0));
                    //printf("tempo: %lf\n", tmp);
                    sleep(tmp);
                    removefila(al);
                    pthread_cond_broadcast(&cond);
                    break;
                }else{
                    pthread_cond_wait(&cond,&trava);
                    cont++;
                    printf("%s: %d\n", al->nome,cont);
                }
            }
        pthread_mutex_unlock(&trava);
        
        c++;
    }
}


int main(void){

    iniciafc();
    Aluno al[tam];
    strcpy(al[1].nome,"Saulo");
    //al[1].priority = 1;
    al[1].num = 1;
    strcpy(al[2].nome,"Samir");
    //al[2].priority = 1;
    al[2].num = 2;
    strcpy(al[3].nome,"Vitor");
    //al[3].priority = 2;
    al[3].num = 3;
    strcpy(al[4].nome,"Vani");
    //al[4].priority = 2;
    al[4].num = 4;
    strcpy(al[5].nome,"Kelvin");
    //al[5].priority = 3;
    al[5].num = 5;
    strcpy(al[6].nome,"Kamila");
    //al[6].priority = 3;
    al[6].num = 6;

    pthread_t thread[tam];

    pthread_barrier_init(&barrier,NULL,6);

    for(int i=1;i<tam;i++){
        pthread_create(&thread[i],NULL,pessoa,(void*)&al[i]);
    }

    for(int i=1;i<tam;i++){
        pthread_join(thread[i],NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}