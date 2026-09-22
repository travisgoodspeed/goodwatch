
void flash_unlock();
void flash_lock();
bool flash_write(uint16_t dst,  uint8_t *src, uint8_t size);
void flash_erase(void);
