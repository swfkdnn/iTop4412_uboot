#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
    将key编码：
    0 - GPX1_1 - HOME
    1 - GPX1_2 - BACK
    2 - GPX3_3 - SLEEP
    3 - GPX2_0 - VOL-
    4 - GPX2_1 - VOL+
*/
#define KEY_HOME 0
#define KEY_BACK 1
#define KEY_SLEEP 2
#define KEY_VOLUME_REDUCE 3
#define KEY_VILUME_PLUS 4

void key_init(void);

int get_key_value(int key_id);

void put_key_value(int key_id);

#ifdef __cplusplus
}
#endif

#endif