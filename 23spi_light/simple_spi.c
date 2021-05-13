#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

unsigned char spi_bpw = 8;

int transfer_spi(int spi_fd, unsigned char *tbuf, unsigned char *rbuf, int len){
    struct spi_ioc_transfer spi_msg;

    memset(&spi_msg, 0, sizeof(spi_msg));

    spi_msg.tx_buf = (unsigned long) tbuf;
    spi_msg.rx_buf = (unsigned long) rbuf;
    spi_msg.len = len;

    return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_msg);
}

void setup_spi(int spi_fd, int speed, int mode){
    if(ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0){
        printf("setup_spi : MODE setup error \n");
    }

    if(ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0){
        printf("setup_spi : BPW setup error \n");
    }
    
    if(ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0){
        printf("setup_spi : SPEED setup error \n");
    }
}

int main(int argc, char *argv[]){
    int spi_fd = 0;
    unsigned char tbuf[3], rbuf[3], channel = 0;
    int light_value = 0;

    if((spi_fd = open("/dev/spidev0.0", O_RDWR)) < 0){
        printf("open /dev/spidev error \n");
        exit(1);
    }

    setup_spi(spi_fd, 1000000, 0);

    tbuf[0] = 0x06 | ((channel & 0x07) >> 2);
    tbuf[1] = ((channel & 0x07) << 6);
    tbuf[2] = 0x00;

    if(transfer_spi(spi_fd, tbuf, rbuf, 3) < 0){
        printf("get data from spi error \n");
    }

    rbuf[1] = 0x0F & rbuf[1];
    light_value = (rbuf[1] << 8) | rbuf[2];

    printf("light sensor data = %d \n", light_value);
    printf("tbuf : %02x %02x %02x\n", tbuf[0],tbuf[1],tbuf[2]);
    printf("rbuf : %02x %02x %02x\n", rbuf[0],rbuf[1],rbuf[2]);

    return 0;
}
