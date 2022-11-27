#include "include.h"

// void *threadFuncShopAssitant(void *arg){
//     printf("shop assitant created\n");

//     while(1){

//     }
// }

// void *threadFuncSofa(void *arg){
//     printf("shop assitant created\n");

//     while(1){

//     }
// }

// void *threadFuncShopAssitant(void *arg){
//     printf("shop assitant created\n");

//     while(1){

//     }
// }

// int main(int argc, char const *argv[])
// {
//     //Had to change stack size for each thread. Otherwize, id be getting a segementation fault error on my system.
//     int stackSize = PTHREAD_STACK_MIN * 15;
//     pthread_attr_t att;
//     pthread_attr_init(&att);
//     pthread_attr_setstacksize(&att, stackSize);

//     int shopAssitants = strtol(argv[1], NULL, 10);
//     int sofaAmount = strtol(argv[2], NULL, 10);
//     int waitingSpots = strtol(argv[3], NULL, 10);
//     int threadPoolSize = shopAssitants + sofaAmount + waitingSpots;

//     pthread_t threadPool [threadPoolSize];

//     for(int i = 0; i < shopAssitants; i++){
//         pthread_create(&threadPool[i], &att, threadFuncShopAssitant, NULL);
//     }

//     //creates threads based on the amount of sofas
//     for(int i = shopAssitants; i < shopAssitants + sofaAmount; i++){
//         pthread_create(&threadPool[i], &att, threadFuncSofa, NULL);
//     }

//     //creates threads based on the amount of shop assitants
//     for(int i = shopAssitants + sofaAmount; i < threadPoolSize; i++){
//         pthread_create(&threadPool[i], &att, threadFuncShopAssitant, NULL);
//     }

//     while(1){

//     }
//     return 0;
// }
