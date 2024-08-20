union {
    uint8_t raw;
    struct {
        uint16_t rest_pos;
        
    };
} key_config_t;

union {
    uint8_t raw;
    key_config_t key[KEY_NUM];
} kb_config_t;
