/* z80.h: z80 emulation core
   Copyright (c) 1999-2003 Philip Kendall

   $Id: z80.h 4640 2012-01-21 13:26:35Z pak21 $

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#ifndef FUSE_Z80_H
#define FUSE_Z80_H

#include "shared.h"

/* Union allowing a register pair to be accessed as bytes or as a word */
typedef union {
#ifdef MSB_FIRST
  struct { uint8 h,l; } b;
#else
  struct { uint8 l,h; } b;
#endif
  uint16 w;
} regpair;

/* What's stored in the main processor */
typedef struct {
  regpair af,bc,de,hl;
  regpair af_,bc_,de_,hl_;
  regpair ix,iy;
  uint8 i;
  uint16 r;	/* The low seven bits of the R register. 16 bits long
			   so it can also act as an RZX instruction counter */
  uint8 r7;	/* The high bit of the R register */
  regpair sp,pc;
  uint8 iff1, iff2, im;
  int halted;
  int after_ei;

} processor;

extern unsigned char *z80_readmap[64];
extern unsigned char *z80_writemap[64];

extern void (*z80_writemem)(unsigned int address, unsigned char data);
extern unsigned char (*z80_readmem)(unsigned int port);
extern void (*z80_writeport)(unsigned int port, unsigned char data);
extern unsigned char (*z80_readport)(unsigned int port);

void z80_init( void );
void z80_reset( void );

int z80_interrupt( void );
void z80_nmi( void );

int z80_do_opcode(void);

void z80_enable_interrupts( void );

extern processor z80;
static inline uint16 z80_getpc(void) { return z80.pc.w; }
extern const uint8 halfcarry_add_table[];
extern const uint8 halfcarry_sub_table[];
extern const uint8 overflow_add_table[];
extern const uint8 overflow_sub_table[];
extern uint8 sz53_table[];
extern uint8 sz53p_table[];
extern uint8 parity_table[];

extern uint32 z80_tstates;

//Write mem
static inline void Z80_WB_MACRO(uint16 A, uint8 V)
{ 
 z80_tstates += 3; 
 z80_writemem(A, V);
}

// Write port
static inline void Z80_WP_MACRO(uint16 A, uint8 V)
{ 
 z80_tstates += 4; 
 z80_writeport(A, V); 
}

// Read mem
static inline uint8 Z80_RB_MACRO(uint16 A)
{ 
 z80_tstates += 3; 
 return(z80_readmem(A));
}

// Read port
static inline uint8 Z80_RP_MACRO(uint16 A)
{ 
 z80_tstates += 4; 
 return(z80_readport(A));
}

extern int z80_int_line;
void z80_set_interrupt(int set);

#endif			/* #ifndef FUSE_Z80_H */
