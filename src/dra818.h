#ifndef dra818_h
#define dra818_h

#include "Arduino.h"
#include <Stream.h>

// ERROR DEFINITION

#define DRA818_ERR_FREC 1
#define DRA818_ERR_VOL 2
#define DRA818_ERR_CTCSS 3
#define DRA818_ERR_SQL 4
#define DRA818_CONF_OK 0
#define default_timeout_ms 1000
#define DRA818_ON HIGH
#define DRA818_OF LOW
#define PTT_ON LOW
#define PTT_OFF HIGH

class dra818 {
public:
    dra818(uint8_t ptt_pin, uint8_t pd_pin);

    // Initialize Internal values
    uint8_t configure(uint8_t tx_ctcss, uint8_t rx_ctcss, uint8_t sql, uint8_t out_vol, uint8_t mic_vol, bool preemph, bool highpass, bool lowpass, uint8_t pd_pin);
    void start(Stream* serial, uint8_t PTT);
    void ptt(uint8_t state);
    void module_power(uint8_t power_state);
    bool module_present();

private:
    Stream* serial;
    uint8_t ptt_pin;
    uint8_t tx_ctcss;
    uint8_t rx_ctcss;
    float tx_freq;
    float rx_freq;
    uint8_t out_vol;
    uint8_t mic_vol;
    uint8_t squelch;
    uint8_t preemph;
    uint8_t highpass;
    uint8_t lowpass;
    void delay_ms(int delay_ms);
    char buffer[60];
    char replybuffer[255];
    unsigned long timer;
    bool sendCommand(char* command, char* reply, uint16_t timeout = default_timeout_ms);
    bool writeFreq();
};
#endif
