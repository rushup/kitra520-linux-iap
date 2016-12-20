#ifndef KITRA_SMALL_UTLITY_H
#define KITRA_SMALL_UTLITY_H

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define K_SET_BIT(var,pos) ((var) |= (1<<(pos)))

#define K_CLEAR_BIT(var,pos) ((var) &= ~(1<<(pos)))

#define K_SET_BIT_VAL(var,pos,val) val==0?K_CLEAR_BIT(var,pos):K_SET_BIT(var,pos)

static void k_get_checksum(char* packet, uint32_t len, char* chks) {
  unsigned char n_chks = 0;
  uint32_t c = 0;
  for ( c = 0; c < len; c++) {
    n_chks = (char)(n_chks ^ packet[c]);
  }
  sprintf(chks, "%02X", n_chks);
}
#endif
