/*
 * @File name (temp_sensor.c)
 * @File Description: (Application code for i2c based TMP 102 temperature sensor)
 * @Author Name (AYSWARIYA KANNAN)
 * @Date (04/23/2023)
 * @Attributions : referred Project to understand
 *                 how tmp 102 is interfaced https://github.com/cu-ecen-aeld/final-project-Amey2904dash/wiki/Project-Overview
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
/*
 * @function	: main.c for reading data through i2c bus
 *
 * @param		: NULL
 * @return		: RETURN  0 on success
 *
 */
#define FILE_PATH "/etc/tmpdir/tempdata.txt"
#define SLAVE_ADDR (0x48)
#define BUFFER_SIZE (50)
#define LOW (0x00)

int main()
{
    int file_tmp;
    int file_fd; // to open the file for saving temp data
    unsigned char tmp_data[2] = {0};
    char *i2c_bus = "/dev/i2c-1"; // bus Path in RPI we have configured for bus -1
    int temp_value;
    file_tmp = open(i2c_bus, O_RDWR);
    // open the file device
    if (file_tmp < 0)
    {
        syslog(LOG_ERR, "Error opening i2c_bus");
        exit(1);
    }

    // ioctl call for I2C
    if (ioctl(file_tmp, I2C_SLAVE, SLAVE_ADDR) < 0)
    {
        syslog(LOG_ERR, "Error while setting i2c slave device");
        exit(1);
    }

    while (1)
    {

        file_fd = open(FILE_PATH, O_WRONLY | O_APPEND); // in write only and append at the end of file
        if (file_fd < 0)
        {
            syslog(LOG_ERR, "Error opening tempdata.txt file");
            syslog(LOG_ERR, "%s", strerror(errno));
        }
        // hold line low for starting communication
        write(file_tmp, LOW, 1);

        // read after that from the slave
        read(file_tmp, tmp_data, 2);

        if (read < 0)
        {
            syslog(LOG_ERR, "error while reading data");
            printf("error while reading data\n");
            exit(1);
        }
        else
        {
            // tmp_sensor value is of 12 bits with  1st byte MSB and 2nd byte only the  d[7] to d[4] are part of the tmp reading
            temp_value = ((tmp_data[0] << 4) | (tmp_data[1] >> 4)) * (0.0625); // converting to celcius

            // printf("temperature value is %d C\n", temp_value);
            char send_buffer[BUFFER_SIZE];
            memset(&send_buffer[0], 0, sizeof(send_buffer));
            sprintf(send_buffer, "Temperature is %d Deg C\n\r", temp_value);
            syslog(LOG_DEBUG, "Temperature is %d Deg C ", temp_value);
            write(file_fd, send_buffer, sizeof(send_buffer));
            usleep(1000000);
        }
        close(file_fd);
    }
    return 0;
}
