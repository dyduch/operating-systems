#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Funkcja get_atam otwiera bibliotekę libm.so
 * , szuka w niej symbolu 'atan'
 * i zwraca wskaźnik na ten sybol
 *
 *
 */

double (*get_atan(void))(double) {
    void* handle = dlopen("libm.so", RTLD_LAZY);
    if(!handle){
        exit(EXIT_FAILURE);
    }
    double (*_atan)(double);
    *(void**) (&_atan) = dlsym(handle, "atan");

    dlclose(handle);
    return *_atan;
}

int yay[2];

void setup_loop(void){
    pipe(yay);
    dup2(yay[1], STDOUT_FILENO);
    dup2(yay[0], STDIN_FILENO);
}


int main() {


    return 0;
}