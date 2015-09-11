#ifndef __PARAMS_H__
#define __PARAMS_H__

void init_table(void);
void tab_write(int pos, uint32_t value);
uint32_t tab_read(int pos);
void tab_write_16b(int pos16, uint16_t val16);
uint16_t tab_read_16b(int pos16);

#endif // __PARAMS_H__
