#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DISPOSITIVO "/dev/chardev_leds"

int main(int argc, char *argv[]){
    FILE * file;
    char cadena[] = "0";
    int leds = 1;
    int k = 0;
    while(k < 20){
        sprintf(cadena, "%d", leds);
        file = fopen(DISPOSITIVO, "r+");
        fwrite(cadena, sizeof(char), strlen(cadena), file);
        fclose(file);
        leds = (leds % 2) +1;
        usleep(500000);
        ++k;
    }
    return 0;
}