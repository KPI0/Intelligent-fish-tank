/*--------------------------------------------------------------------------
INTRINS.H

Intrinsic functions for C51.
Copyright (c) 1988-2010 Keil Elektronik GmbH and ARM Germany GmbH
All rights reserved.
--------------------------------------------------------------------------*/

#ifndef __INTRINS_H__
#define __INTRINS_H__

#pragma SAVE

#if defined (__CX2__)
#pragma FUNCTIONS(STATIC)
/* intrinsic functions are reentrant, but need static attribute */
#endif

extern void          _nop_     (void);
extern bit           _testbit_ (bit);
extern unsigned char _cror_    (unsigned char, unsigned char);
extern unsigned int  _iror_    (unsigned int,  unsigned char);
extern unsigned long _lror_    (unsigned long, unsigned char);
extern unsigned char _crol_    (unsigned char, unsigned char);
extern unsigned int  _irol_    (unsigned int,  unsigned char);
extern unsigned long _lrol_    (unsigned long, unsigned char);
extern unsigned char _chkfloat_(float);
#if defined (__CX2__)
extern int           abs       (int);
#endif
#if !defined (__CX2__)
extern void          _push_    (unsigned char _sfr);
extern void          _pop_     (unsigned char _sfr);
#endif

#pragma RESTORE

#endif

