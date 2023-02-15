#ifndef NVS_H
#define NVS_H

void nvsInit();

int32_t nvsGetValue(char *vkey);

void nvsWriteValue(char *vkey, int32_t valor);

#endif