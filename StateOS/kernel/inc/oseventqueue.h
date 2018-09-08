/******************************************************************************

    @file    StateOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    08.09.2018
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOS_EVQ_H
#define __STATEOS_EVQ_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evq evq_t, * const evq_id;

struct __evq
{
	obj_t    obj;   // object header

	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	unsigned*data;  // data buffer
};

/******************************************************************************
 *
 * Name              : _EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : event queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVQ_INIT( _limit, _data ) { _OBJ_INIT(), 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVQ_DATA
 *
 * Description       : create an event queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _EVQ_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVQ
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVQ( evq, limit )                                \
                       unsigned evq##__buf[limit];                       \
                       evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                       evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : static_EVQ
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVQ( evq, limit )                                \
                static unsigned evq##__buf[limit];                       \
                static evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                static evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_INIT( limit ) \
                      _EVQ_INIT( limit, _EVQ_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVQ_CREATE
 * Alias             : EVQ_NEW
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_CREATE( limit ) \
           (evq_t[]) { EVQ_INIT  ( limit ) }
#define                EVQ_NEW \
                       EVQ_CREATE
#endif

/******************************************************************************
 *
 * Name              : evq_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_init( evq_t *evq, unsigned *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : evq_create
 * Alias             : evq_new
 *
 * Description       : create and initialize a new event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object (event queue successfully created)
 *   0               : event queue not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

evq_t *evq_create( unsigned limit );

__STATIC_INLINE
evq_t *evq_new( unsigned limit ) { return evq_create(limit); }

/******************************************************************************
 *
 * Name              : evq_kill
 *
 * Description       : reset the event queue object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_kill( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_delete
 *
 * Description       : reset the event queue object and free allocated resource
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void evq_delete( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_waitFor
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait for given duration of time while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is empty)
 *                     IMMEDIATE: don't wait if the event queue object is empty
 *                     INFINITE:  wait indefinitely while the event queue object is empty
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_waitFor( evq_t *evq, unsigned *data, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_waitUntil
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait until given timepoint while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_waitUntil( evq_t *evq, unsigned *data, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_STOPPED       : event queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evq_wait( evq_t *evq, unsigned *data ) { return evq_waitFor(evq, data, INFINITE); }

/******************************************************************************
 *
 * Name              : evq_take
 * ISR alias         : evq_takeISR
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_TIMEOUT       : event queue object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_take( evq_t *evq, unsigned *data );

__STATIC_INLINE
unsigned evq_takeISR( evq_t *evq, unsigned *data ) { return evq_take(evq, data); }

/******************************************************************************
 *
 * Name              : evq_sendFor
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait for given duration of time while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *   delay           : duration of time (maximum number of ticks to wait while the event queue object is full)
 *                     IMMEDIATE: don't wait if the event queue object is full
 *                     INFINITE:  wait indefinitely while the event queue object is full
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_sendFor( evq_t *evq, unsigned data, cnt_t delay );

/******************************************************************************
 *
 * Name              : evq_sendUntil
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait until given timepoint while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : event queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned evq_sendUntil( evq_t *evq, unsigned data, cnt_t time );

/******************************************************************************
 *
 * Name              : evq_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_STOPPED       : event queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned evq_send( evq_t *evq, unsigned data ) { return evq_sendFor(evq, data, INFINITE); }

/******************************************************************************
 *
 * Name              : evq_give
 * ISR alias         : evq_giveISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : event queue object is full
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_give( evq_t *evq, unsigned data );

__STATIC_INLINE
unsigned evq_giveISR( evq_t *evq, unsigned data ) { return evq_give(evq, data); }

/******************************************************************************
 *
 * Name              : evq_push
 * ISR alias         : evq_pushISR
 *
 * Description       : try to transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_TIMEOUT       : there are tasks waiting for writing to the event queue object
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned evq_push( evq_t *evq, unsigned data );

__STATIC_INLINE
unsigned evq_pushISR( evq_t *evq, unsigned data ) { return evq_push(evq, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : EventQueueT<>
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned limit_>
struct EventQueueT : public __evq
{
	 EventQueueT( void ): __evq _EVQ_INIT(limit_, data_) {}
	~EventQueueT( void ) { assert(__evq::obj.queue == nullptr); }

	void     kill     ( void )                          {        evq_kill     (this);                }
	unsigned waitFor  ( unsigned *_data, cnt_t _delay ) { return evq_waitFor  (this, _data, _delay); }
	unsigned waitUntil( unsigned *_data, cnt_t _time )  { return evq_waitUntil(this, _data, _time);  }
	unsigned wait     ( unsigned *_data )               { return evq_wait     (this, _data);         }
	unsigned take     ( unsigned *_data )               { return evq_take     (this, _data);         }
	unsigned takeISR  ( unsigned *_data )               { return evq_takeISR  (this, _data);         }
	unsigned sendFor  ( unsigned  _data, cnt_t _delay ) { return evq_sendFor  (this, _data, _delay); }
	unsigned sendUntil( unsigned  _data, cnt_t _time )  { return evq_sendUntil(this, _data, _time);  }
	unsigned send     ( unsigned  _data )               { return evq_send     (this, _data);         }
	unsigned give     ( unsigned  _data )               { return evq_give     (this, _data);         }
	unsigned giveISR  ( unsigned  _data )               { return evq_giveISR  (this, _data);         }
	unsigned push     ( unsigned  _data )               { return evq_push     (this, _data);         }
	unsigned pushISR  ( unsigned  _data )               { return evq_pushISR  (this, _data);         }

	private:
	unsigned data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_EVQ_H
