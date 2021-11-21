#include "dra818.h"
#include "Arduino.h"
#include <Stream.h>
#include <stdio.h>

dra818::dra818(uint8_t ptt_pin, uint8_t pd_pin)
{
    this->ptt_pin = ptt_pin;
    this->pd_pin = pd_pin;
}

//    void start(Stream* serial, uint8_t PTT);
//    void set_ptt(bool state);
//    Stream* serial;
//    uint8_t ptt_pin;
//    uint8_t tx_ctcss;
//    uint8_t rx_ctcss;
//    float tx_freq;
//    float rx_freq;
//    uint8_t out_vol;
//    uint8_t mic_vol;
//    uint8_t squelch;
//    uint8_t preemph;
//    uint8_t highpass;
//    uint8_t lowpass;
//    void delay_ms(int delay_ms);
//#define DRA818_ERR_FREC 1
//#define DRA818_ERR_VOL 2
//#define DRA818_ERR_CTCSS 3
//#define DRA818_ERR_SQL 4
//#define DRA818_CONF_OK 0

uint8_t dra818::configure(Stream* serial, uint8_t tx_ctcss, uint8_t rx_ctcss, uint8_t sql, uint8_t out_vol, uint8_t mic_vol, bool preemph, bool highpass, bool lowpass)
{
    this->serial = serial;
    //Initialize the PTT and PD pins
    pinMode(this->ptt_pin, OUTPUT);
    pinMode(this->pd_pin, OUTPUT);

    // ToDo: Add better validation of tx/rx frequency, tx and rx should be in the same band.
    if ((tx_freq > 136.0 && tx_freq < 174.00) || (tx_freq > 410.0 && tx_freq < 480.000)) {
	this->tx_freq = tx_freq;
    } else {
	return DRA818_ERR_FREC;
    }
    if ((rx_freq > 136.0 && rx_freq < 174.00) || (rx_freq > 410.0 && rx_freq < 480.000)) {
	this->rx_freq = rx_freq;
    } else {
	return DRA818_ERR_FREC;
    }
    if (tx_ctcss <= 38) {
	this->tx_ctcss = tx_ctcss;
    } else {
	return DRA818_ERR_CTCSS;
    }
    if (rx_ctcss <= 38) {
	this->rx_ctcss = rx_ctcss;
    } else {
	return DRA818_ERR_CTCSS;
    }
    if (sql <= 8) {
	this->squelch = sql;
    } else {
	return DRA818_ERR_SQL;
    }
    if (out_vol >= 1 || out_vol <= 8) {
	this->out_vol = out_vol;
    } else {
	return DRA818_ERR_VOL;
    }
    if (mic_vol >= 1 || mic_vol <= 8) {
	this->mic_vol = mic_vol;
    } else {
	return DRA818_ERR_VOL;
    }
    this->preemph = preemph ? 0 : 1;
    this->highpass = highpass ? 0 : 1;
    this->lowpass = lowpass ? 0 : 1;
    return DRA818_CONF_OK;
}

void dra818::ptt(uint8_t state)
{
    digitalWrite(this->ptt_pin, state);
}

bool dra818::writeFreq()
{
    ptt(PTT_OFF);
    delay_ms(100);
    char rx_freq_buff[10];
    char tx_freq_buff[10];
    dtostrf(this->rx_freq, 8, 4, freq_buff);
    dtostrf(this->tx_freq, 8, 4, tx_freq_buff);
    sprintf(this->buffer, "AT+DMOSETGROUP=0,%s,%s,%04d,%1d,%04d\r\n", tx_freq_buff, rx_freq_buff, this->tx_ctcss, this->squelch, this->rx_ctcss);
    return sendCommand(this->buffer, "+DMOSETGROUP:0", 100);
}

bool dra818::sendCommand(char* command, char* reply, uint16_t timeout = default_timeout_ms)
{
    while (serial->available()) {
	serial->read();
    }
    serial->write(command);

    uint8_t idx = 0;
    bool replyMatch = false;
    timer = millis();

    while (!replyMatch && millis() - timer < timeout) {
	if (serial->available()) {
	    replybuffer[idx] = serial->read();
	    idx++;
	    // if (strstr(replybuffer, reply) != NULL) replyMatch = true; // Only checks if desired reply is inside replybuffer
	    if (strcmp(replybuffer, reply) == 0)
		replyMatch = true; // This means the reply must start with the desired reply to be successful
	}
    }
    return replyMatch
}

void dra818::delay_ms(int delay_ms)
{
    unsigned long x = 0;
    while (x < delay_ms) {
	delay(1);
	x += 1;
    }
}
