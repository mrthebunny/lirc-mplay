/*      $Id: ir_remote_types.h,v 5.2 2005/03/27 11:55:07 lirc Exp $      */

/****************************************************************************
 ** ir_remote_types.h *******************************************************
 ****************************************************************************
 *
 * ir_remote_types.h - describes and decodes the signals from IR remotes
 *
 * Copyright (C) 1996,97 Ralph Metzler <rjkm@thp.uni-koeln.de>
 * Copyright (C) 1998 Christoph Bartelmus <lirc@bartelmus.de>
 *
 */ 

#ifndef IR_REMOTE_TYPES_H
#define IR_REMOTE_TYPES_H

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#ifdef LONG_IR_CODE
typedef unsigned long long ir_code;
#else
typedef unsigned long ir_code;
#endif

struct ir_code_node
{
	ir_code code;
	struct ir_code_node *next;
};

/*
  Code with name string
*/

struct ir_ncode {
	char *name;
	ir_code code;
        int length;
        lirc_t *signals;
	struct ir_code_node *next;
	struct ir_code_node *current;
};

/*
  struct ir_remote
  defines the encoding of a remote control 
*/

/* definitions for flags */

/* Don't forget to take a look at config_file.h when adding new flags */

#define RC5             0x0001    /* IR data follows RC5 protocol */

/* Hm, RC6 protocols seem to have changed the biphase semantics so
   that lircd will calculate the bit-wise complement of the codes. But
   this is only a guess as I did not have a datasheet... */

#define RC6             0x0002    /* IR data follows RC6 protocol */
#define RCMM            0x0004    /* IR data follows RC-MM protocol */
#define SPACE_ENC	0x0008	  /* IR data is space encoded */
#define REVERSE		0x0010
#define NO_HEAD_REP	0x0020	  /* no header for key repeats */
#define NO_FOOT_REP	0x0040	  /* no foot for key repeats */
#define CONST_LENGTH    0x0080    /* signal length+gap is always constant */
#define RAW_CODES       0x0100    /* for internal use only */
#define REPEAT_HEADER   0x0200    /* header is also sent before repeat code */
#define GOLDSTAR        0x0400    /* encoding found on Goldstar remote */
#define GRUNDIG         0x0800    /* encoding found on Grundig remote */
#define SERIAL          0x1000    /* serial protocol */
#define SPACE_FIRST     0x2000    /* bits are encoded as space+pulse */

#define SHIFT_ENC	   RC5    /* IR data is shift encoded (name obsolete) */
#define COMPAT_REVERSE  0x8000    /* compatibility mode for REVERSE flag */

/* stop repeating after 600 signals (approx. 1 minute) */
/* update technical.html when changing this value */
#define REPEAT_MAX 600

#define DEFAULT_FREQ 38000

struct ir_remote 
{
	char *name;                 /* name of remote control */
	struct ir_ncode *codes;
	int bits;                   /* bits (length of code) */
	int flags;                  /* flags */
	int eps;                    /* eps (_relative_ tolerance) */
	int aeps;                   /* detecing _very short_ pulses is
				       difficult with relative tolerance
				       for some remotes,
				       this is an _absolute_ tolerance
				       to solve this problem
				       usually you can say 0 here */
#       ifdef DYNCODES
	char *dyncodes_name;         /* name for unknown buttons */
	int dyncode;                 /* last received code */
	struct ir_ncode dyncodes[2]; /* helper structs for unknown buttons*/
#       endif
	
	/* pulse and space lengths of: */
	
	lirc_t phead,shead;         /* header */
	lirc_t pthree,sthree;       /* 3 (only used for RC-MM) */
	lirc_t ptwo,stwo;           /* 2 (only used for RC-MM) */
	lirc_t pone,sone;           /* 1 */
	lirc_t pzero,szero;         /* 0 */
	lirc_t plead;		    /* leading pulse */
	lirc_t ptrail;              /* trailing pulse */
	lirc_t pfoot,sfoot;         /* foot */
	lirc_t prepeat,srepeat;	    /* indicate repeating */

	int pre_data_bits;          /* length of pre_data */
	ir_code pre_data;           /* data which the remote sends before
				       actual keycode */
	int post_data_bits;         /* length of post_data */
	ir_code post_data;          /* data which the remote sends after
				       actual keycode */
	lirc_t pre_p,pre_s;         /* signal between pre_data and keycode */
	lirc_t post_p, post_s;      /* signal between keycode and post_code */

	lirc_t gap;                 /* time between signals in usecs */
	lirc_t repeat_gap;          /* time between two repeat codes
				       if different from gap */
	int toggle_bit;             /* 1..bits */
	int min_repeat;             /* code is repeated at least x times
				       code sent once -> min_repeat=0 */
	unsigned int freq;          /* modulation frequency */
	unsigned int duty_cycle;    /* 0<duty cycle<=100 */
	ir_code toggle_mask;        /* Sharp (?) error detection scheme */
	ir_code rc6_mask;           /* RC-6 doubles signal length of
				       some bits */
	
	/* serial protocols */
	unsigned int baud;          /* can be overridden by [p|s]zero,
				       [p|s]one */
	unsigned int bits_in_byte;  /* default: 8 */
	unsigned int parity;        /* currently unsupported */
	unsigned int stop_bits;     /* mapping: 1->2 1.5->3 2->4 */
	
	/* end of user editable values */
	
        int repeat_state;
	int toggle_mask_state;
	int repeat_countdown;
	struct ir_ncode *last_code; /* code received or sent last */
	struct ir_ncode *toggle_code; /* toggle code received or sent last */
	int reps;
	struct timeval last_send;   /* time last_code was received or sent */
	lirc_t remaining_gap;       /* remember gap for CONST_LENGTH remotes */
        struct ir_remote *next;
};

#endif