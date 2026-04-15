#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_STRINGS 20
#define STR_LEN 10

char strings[NUM_STRINGS][STR_LEN + 1]; 
sem_t sem;


char* reverse(char *str){
    int len = 0;
    while(str[len] != '\0'){
        len++;
    }
    char *rev = (char *)malloc(len + 1);
    if (!rev) return NULL; 
    
    for(int i = 0; i < len; i++){
        rev[i] = str[len - i - 1];
    }
    rev[len] = '\0';
    return rev;
}

int palindrome(char *str){
    int len = 0;
    while(str[len] != '\0'){
        len++;
    }
    for(int i = 0; i < len / 2; i++){
        if(str[i] != str[len - i - 1]){
            return 0;
        }
    }
    return 1;
}

int* frequency(char *str){
    int *freq = (int *)calloc(26, sizeof(int));
    if (!freq) return NULL;

    for(int i = 0; str[i] != '\0'; i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            freq[str[i] - 'a']++;
        }
    }
    return freq;
}

void *thread1(void *arg) {
    for (int i = 0; i < NUM_STRINGS; i++) {
        char *rev = reverse(strings[i]);
        sem_wait(&sem);
        printf("[Thread 1] Original: %s | Reversed: %s\n", strings[i], rev);
        sem_post(&sem);
        
        free(rev); 
    }
    return NULL;
}

void *thread2(void *arg) {
    for (int i = 0; i < NUM_STRINGS; i++) {
        int isPal = palindrome(strings[i]);
        sem_wait(&sem);
        printf("[Thread 2] String: %s | Is Palindrome: %s\n", strings[i], isPal ? "Yes" : "No");
        sem_post(&sem);
    }
    return NULL;
}
void *thread3(void *arg) {
    for (int i = 0; i < NUM_STRINGS; i++) {
        int *freq = frequency(strings[i]);
        sem_wait(&sem);
        printf("[Thread 3] String: %s | Max Freq Letter: ", strings[i]);
        int max = 0;
        char maxChar = '?';
        for(int k=0; k<26; k++){
            if(freq[k] > max) {
                max = freq[k];
                maxChar = (char)('a' + k);
            }
        }
        printf("%c (%d)\n", maxChar, max);
        sem_post(&sem);

        free(freq); 
    }
    return NULL;
}


int main(){
    srand(time(NULL)); 
    pthread_t t1, t2, t3;

    printf("Generating %d random strings...\n", NUM_STRINGS);
    for(int i = 0; i < NUM_STRINGS; i++){
        for(int j = 0; j < STR_LEN; j++){
            char r = (rand() % 26) + 'a'; 
            strings[i][j] = r;
        }
        strings[i][STR_LEN] = '\0'; 
    }
    sem_init(&sem, 0, 1); 

    if(pthread_create(&t1, NULL, thread1, NULL) != 0) { perror("Failed to create t1"); }
    if(pthread_create(&t2, NULL, thread2, NULL) != 0) { perror("Failed to create t2"); }
    if(pthread_create(&t3, NULL, thread3, NULL) != 0) { perror("Failed to create t3"); }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    sem_destroy(&sem);

    return 0;
}

