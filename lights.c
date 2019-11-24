#include "lights.h"

unsigned long long millis(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
}

void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    // abort();
}


void lights_setAll(int r, int g, int b);

int initLights() {
    bcm2835_init();
    bcm2835_spi_begin();
    bcm2835_spi_set_speed_hz(SPI_SPEED_HZ);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    int x;
    for(x = 0; x < BUF_SIZE; x++){
        buf[x] = 0;
    }

    for(x = BUF_SIZE - SOF_BYTES; x < BUF_SIZE; x++){
        buf[x] = 255;
    }
    lights_setAll(0,0,0);
    return 0;
}

void lights_setPixel(int x, int r, int g, int b){
    int offset = SOF_BYTES + (x * 4);
    buf[offset + 0] = 0b11100011;
    buf[offset + 1] = b;
    buf[offset + 2] = g;
    buf[offset + 3] = r;
}

void lights_setAll(int r, int g, int b){
    int x;
    for(x = 0; x < 12; x++){
        lights_setPixel(x, r, g, b);
    }
}

void lights_show(){
    bcm2835_spi_writenb(buf, BUF_SIZE);
    usleep(MIN_DELAY_US);
}

void lights_cleanup(){
    bcm2835_spi_end();
    bcm2835_close();
}

