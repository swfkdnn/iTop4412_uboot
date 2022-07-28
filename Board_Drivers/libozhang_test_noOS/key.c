#include "key.h"
#include "s3c4412_gpio.h"
#include "stdio.h"

void key_init(void)
{
    GPX1CON = 0x00000000;
    GPX2CON = 0x00000000;
    GPX3CON = 0x00000000;

    return;
}

int get_key_value(int key_id)
{
    int key_value = 0;

    switch (key_id) {
        case KEY_HOME:
        {
            key_value = GPX1DAT & 0x2;
            break;
        }
        case KEY_BACK:
        {
            key_value = GPX1DAT & 0x4;
            break;
        }
        case KEY_SLEEP:
        {
            key_value = GPX3DAT & 0x8;
            break;
        }
        case KEY_VOLUME_REDUCE:
        {
            key_value = GPX2DAT & 0x1;
            break;
        }
        case KEY_VILUME_PLUS:
        {
            key_value = GPX2DAT & 0x2;
            break;
        }
        default:
        {
            key_value = 0;
            break;
        }
    }

    return key_value;
}

void put_key_value(int key_id)
{
    switch (key_id) {
        case KEY_HOME:
        {
            printf("Home\n");
            break;
        }
        case KEY_BACK:
        {
            printf("Back\n");
            break;
        }
        case KEY_SLEEP:
        {
            printf("Sleep\n");
            break;
        }
        case KEY_VOLUME_REDUCE:
        {
            printf("Vol-\n");
            break;
        }
        case KEY_VILUME_PLUS:
        {
            printf("Vol+\n");
            break;
        }
        default:
        {
            printf("Invalid key id = %d\n", key_id);
            break;
        }
    }

    return;
}
