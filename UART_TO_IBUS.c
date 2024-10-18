#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool CORRECT = false;

unsigned short calculate_crc16(const unsigned char *data, unsigned int length) {

    unsigned short crc = 0xFFFF;

    while(length != 0)
    {
        crc ^= *data;

        for(int i = 0; i < 8; i++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        data++;
        length--;
    }
    return crc & 0xFFFF;
}

uint16_t* process_ibus_frame(const unsigned char *data, unsigned int length) {

    uint16_t *channel = (uint16_t *)malloc(((length-4)/2) * sizeof(uint16_t));

    if (channel == NULL) {
        CORRECT = false;
        return NULL;
    }

    if (data[0] != 0x20 || data[1] != 0x40) {
        CORRECT = false;
        free(channel);
        return NULL;
    }

    for (int i = 2; i < length - 3; i += 2) {
        channel[(i - 2) / 2] = (data[i + 1] << 8) | data[i];
    }

    unsigned short calculated_crc = calculate_crc16(data, length - 2);

    unsigned short received_crc = (data[length - 1] << 8) | data[length - 2];

    if (calculated_crc == received_crc)
    {
        CORRECT = true;
    } else
    {
        CORRECT = false;
        free(channel);
        return NULL;
    }
    return channel;
}

int main()
{
    unsigned char ibus_data[] =
        {
        0x20, 0x40,
        0x58, 0x02,
        0x7A, 0x01,
        0x3C, 0x02,
        0x29, 0x01,
        0x46, 0x02,
        0x14, 0x01,
        0x70, 0x01,
        0x0F, 0x02,
        0x88, 0x01,
        0x52, 0x02,
        0x61, 0x01,
        0x93, 0x01,
        0xA2, 0x02,
        0xB1, 0x01,
        0x13, 0x02,
        0x20, 0x89
    };

    uint16_t *channels = process_ibus_frame(ibus_data, 34);

    if (CORRECT) {

        unsigned int num_channels = 15;

        for (int i = 0; i < num_channels; i++)
        {
            printf("Channel %d: %04X\n", i, channels[i]);
        }
        free(channels);
    }
    else
    {
        printf("Error\n");
    }
    return 0;
}
