#ifndef MC145158_H
#define MC145158_H
#include "Arduino.h"

class MC145158 {
	
		volatile unsigned long int time = 0;
		
		typedef struct {
			uint8_t clk = 0, enb = 0, data = 0, ld = 0;	
		}PLLPIN;
		PLLPIN pin;
		
	public:
		typedef struct {
			int32_t ifreq = 0;
			uint32_t ref = 0;
			uint8_t divp = 0;
		}CFG;
	
	private:
		CFG cfg = { 214000, 144000, 128 };
		
		void drive_bus( bool );
		void emit_byte( byte );
		void pulse_enb( void );
	
	public:
		void begin( uint8_t clk, uint8_t data, uint8_t enb, uint8_t ld );
		void set_config( const CFG* );
		bool set_pll( uint32_t, bool, uint16_t=12500 );
};
#endif