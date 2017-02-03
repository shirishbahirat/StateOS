/******************************************************************************

    @file    StateOS: osbase.h
    @author  Rajmund Szymanski
    @date    31.01.2017
    @brief   This file contains basic definitions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __STATEOSBASE_H
#define __STATEOSBASE_H

#include <stdbool.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define USEC       (unsigned)(OS_FREQUENCY)/1000000U
#define MSEC       (unsigned)(OS_FREQUENCY)/1000U
#define  SEC       (unsigned)(OS_FREQUENCY)
#define  MIN       (unsigned)(OS_FREQUENCY)*60U
#define HOUR       (unsigned)(OS_FREQUENCY)*3600U

/* -------------------------------------------------------------------------- */

#define ID_STOPPED ( 0U) // task or timer stopped
#define ID_READY   ( 1U) // task ready to run
#define ID_DELAYED ( 2U) // task waiting or suspended
#define ID_TIMER   ( 3U) // timer in the countdown state
#define ID_IDLE    ( 4U) // idle process

/* -------------------------------------------------------------------------- */

#define E_SUCCESS  ( 0U) // process released by taking the supervising object
#define E_STOPPED  (~0U) // process released by killing the supervising object
#define E_TIMEOUT  (~1U) // process released by the end of the timer countdown

/* -------------------------------------------------------------------------- */

#define IMMEDIATE  ( 0U) // no waiting
#define INFINITE   (~0U) // infinite waiting

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
 (((unsigned)( size )+(sizeof(stk_t)-1))/sizeof(stk_t))

/* -------------------------------------------------------------------------- */

#define UMIN( a, b ) \
 ((unsigned)( a )<(unsigned)( b )?(unsigned)( a ):(unsigned)( b ))

/* -------------------------------------------------------------------------- */

typedef   void       fun_t(); // timer/task procedure

typedef struct __que que_t; // queue
typedef struct __mtx mtx_t; // mutex
typedef struct __tmr tmr_t; // timer
typedef struct __tsk tsk_t; // task

/* -------------------------------------------------------------------------- */

// queue

struct __que
{
	que_t  * next;
};

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj
{
	tsk_t  * queue; // next process in the DELAYED queue
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER, ID_IDLE
	void   * prev;  // previous object (timer, task) in the READY queue
	void   * next;  // next object (timer, task) in the READY queue

}	obj_t;

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
#if OS_TIMER == 0
	volatile
	unsigned cnt;   // system timer counter
#if OS_ROBIN
	unsigned dly;   // task switch counter
#endif
#endif
}	sys_t;

/* -------------------------------------------------------------------------- */

// task context

typedef struct __ctx
{
	// context saved by the software
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	unsigned lr;  // EXC_RETURN
	// context saved by the hardware
	unsigned r0, r1, r2, r3;
	unsigned r12; // ip
	unsigned r14; // lr
	fun_t  * pc;
	unsigned psr;

}	ctx_t;

#define _CTX_INIT( pc ) { 0, 0, 0, 0, 0, 0, 0, 0, 0xFFFFFFFD, 0, 0, 0, 0, 0, 0, pc, 0x01000000 }

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, fun_t *pc )
{
	ctx->lr  = 0xFFFFFFFD; // EXC_RETURN: return from psp
	ctx->pc  = pc;
	ctx->psr = 0x01000000;
}

/* -------------------------------------------------------------------------- */

// procedure inside ISR?
__STATIC_INLINE
unsigned port_isr_inside( void )
{
	return __get_IPSR();
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSBASE_H
