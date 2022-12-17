#include "MC145158.h"

void MC145158::begin( uint8_t clk, uint8_t data, uint8_t enb, uint8_t ld ){
	this->pin.clk = clk;
	this->pin.data = data;
	this->pin.enb = enb;
	this->pin.ld = ld;
	
	pinMode( this->pin.enb, OUTPUT );
	pinMode( this->pin.data, OUTPUT );
	pinMode( this->pin.clk, OUTPUT );
	pinMode( this->pin.ld, INPUT );
	
	this->drive_bus( 0 );
	digitalWrite( this->pin.enb, LOW );
	digitalWrite( this->pin.clk, LOW );
}

void MC145158::set_config( const MC145158::CFG* cfg ){
		if( cfg->ifreq ) this->cfg.ifreq = cfg->ifreq;
		if( cfg->ref ) this->cfg.ref = cfg->ref;
		if( cfg->divp ) this->cfg.divp = cfg->divp;
}

bool MC145158::set_pll( uint32_t freq, bool rx, uint16_t step ){


    if( rx ) freq += this->cfg.ifreq;
  
  freq *= 100UL;
  freq /= step;
  int8_t fa = this->cfg.divp == 40 ? 0 : -1;
  uint32_t N = freq / (this->cfg.divp+fa);  // 127 for UHF
  uint8_t A = freq - (N * (this->cfg.divp+fa));  // 127 for UHF

  this->drive_bus(1);
  /* Send "(n << 8)|(a << 1)", 24 bits, MSB first, LSB always zero */
  this->emit_byte( (N >> 8) & 0xFF );  /* N high byte */
  this->emit_byte( N & 0xFF );   /* N low byte */ 
  this->emit_byte( A << 1 );   /* A and LSB 0 */
  this->pulse_enb();     /* Latch it */
  
  uint16_t Rs = (this->cfg.ref*100UL)/step; //Obliczanie wartości rastra
  Rs <<= 1;
  Rs |= 1;
  this->emit_byte( (Rs >> 8) & 0xFF ); //Wysyłanie wartości rastra do PLL
  this->emit_byte( Rs & 0xFF );
  this->pulse_enb();
  this->drive_bus (0);

  static int8_t c = 0;
  bool unlock = digitalRead( this->pin.ld );
  c = 0;

    if( unlock ){
      
          while( unlock && c < 3 ){
            unlock = digitalRead( this->pin.ld );
            c++;
            delay( 100 );
          }

          if( c == 3 ){
            c++;
            return false;
          } 
    }

  return true;
}

void MC145158::drive_bus( bool enable ){
  
    if( enable ){
      pinMode( this->pin.data, OUTPUT );
      pinMode( this->pin.clk, OUTPUT ) ;
      digitalWrite ( this->pin.data, HIGH) ;
    } else {
      pinMode( this->pin.data, INPUT );
      pinMode( this->pin.clk, INPUT );
      digitalWrite( this->pin.data, LOW );
    }
}

void MC145158::emit_byte( byte b ){

    for ( uint8_t i = 0; i < 8; i++ ){
      digitalWrite( this->pin.data, (b & 0x80) ? HIGH : LOW );
      time++;
      time++; 
      digitalWrite( this->pin.clk, HIGH ); /* rising edge latches data */
      b <<= 1;      
      time++;
      time++;
      time++;
      digitalWrite( this->pin.clk, LOW );
    }

  digitalWrite( this->pin.data, HIGH );
}

void MC145158::pulse_enb( void ){
  digitalWrite ( this->pin.enb, HIGH );
  time++;     
  time++;
  digitalWrite( this->pin.enb, LOW );
}