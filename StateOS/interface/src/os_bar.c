/******************************************************************************

    @file    StateOS: os_bar.c
    @author  Rajmund Szymanski
    @date    24.02.2017
    @brief   This file provides set of functions for StateOS.

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

#include <os.h>

/* -------------------------------------------------------------------------- */
bar_t *bar_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	bar_t *bar;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	bar = core_sys_alloc(sizeof(bar_t));

	if (bar)
	{
		bar->count = limit;
		bar->limit = limit;
	}

	port_sys_unlock();

	return bar;
}

/* -------------------------------------------------------------------------- */
void bar_kill( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(bar);
	
	port_sys_lock();

	bar->count = bar->limit;

	core_all_wakeup(bar, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_bar_wait( bar_t *bar, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside());
	assert(bar);

	port_sys_lock();

	if (--bar->count > 0)
	{
		event = wait(bar, time);
	}
	else
	{
		bar->count = bar->limit;

		core_all_wakeup(bar, E_SUCCESS);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitUntil( bar_t *bar, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitFor( bar_t *bar, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
