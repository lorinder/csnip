#ifndef CSNIP_CORO_H
#define CSNIP_CORO_H

/**	@file coro.h
 *	@brief				Coroutines
 *	@defgroup coro			Coroutines
 *	@{
 *
 *	@brief Lightweight coroutines
 *
 *	This module provides lightweight and very efficient coroutines,
 *	based on [an idea by Simon Tatham](https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html).
 *	These coroutines execute in the same stack as the main program
 *	flow.  They are implemented in portable C, and have no
 *	dependencies such as special APIs.
 *
 *	In order to use them effectively and correctly, it is useful to
 *	understand how they work.  We only explain the very basics here,
 *	referring the reader to Simon Tatham's text for further details.
 *
 *	Coroutines have the same prototypes and are invoked in the same
 *	way as standard C functions.  The implementor of a coroutine is
 *	responsible for maintaining and using a state that survives the
 *	time between when a coroutine yields and resumes.  The
 *	state must contain an integer variable that represents the
 *	next exection location.  In this documentation, we name this
 *	variable @a pc, since it is similar, in concept, to the program
 *	counter that is part of coroutines implemented in assembly.  The
 *	state must also include any local variables that are supposed to
 *	maintain their values across yield-resume boundaries.  Other
 *	local variables don't maintain their values through
 *	yield-resume.
 *
 *	The cleanest and recommended way to implement the state is to
 *	define a struct with the necessary variables, and pass that as
 *	an argument to the coroutine whenever it is called or resumed.
 *	For a given state, the first call must have pc == 0, whereas
 *	subsequent calls have pc > 0.  The value pc == -1 is reserved
 *	for the case when the coroutine has terminated.
 *
 *	The implementor then places a CSNIP_CORO_START() call near the
 *	top of the coroutine; this causes a jump to the point where
 *	execution should be resumed (i.e., where the last yield happen).
 *	Code appearing before CSNIP_CORO_START() is executed at every
 *	invocation of the coroutine, including resumptions.
 *
 *	A CSNIP_CORO_END() needs to appear at the same lexical nesting
 *	level.  A yield (csnip_coro_Yield() call) may only appear
 *	between @a CSNIP_CORO_START() and @a CSNIP_CORO_END(); thus once
 *	the execution flows through a @a CSNIP_CORO_END(), that means
 *	that the coroutine can no longer be resumed from the next call.
 *
 *	Some important implementation notes:
 *
 *	1.  The jumps to resume are implemented using Duff's device;
 *	    each csnip_coro_Yield() contains a `case' label behind the
 *	    scenes; thus it is not possible to yield from within a
 *	    switch block.  If necessary, use  if() / else if() constructs
 *	    instead.
 *
 *	2.
 *
 *	\include coro.c
 */

#include <assert.h>

/** Preamble of a coroutine.
 *
 *  In the case of resumption, this macro causes a jump to the last
 *  point of execution.  (For the initial invocation, when pc == 0, no
 *  jump is performed.)
 *
 *  This macro must be matched by a corresponding CSNIP_CORO_END() at
 *  the same nesting level.
 *
 *  Behind the scenes this is the opening part of a switch() statement.
 *
 *  @param	pc
 *		Integer label where to jump to, in the initial
 *		invocation, when pc == 0, no jump is performed.
 *
 */
#define CSNIP_CORO_START(pc) \
		assert(pc != -1); \
		switch(pc) { \
		case 0: ;

/** Epilogue of a coroutine.
 *
 *  This marks the end of the resumable part of a coroutine.  For every
 *  CSNIP_CORO_START(), there must be exactly one CSNIP_CORO_END().
 */
#define CSNIP_CORO_END(pc) \
		break; \
		default: \
			assert(0); \
		} \
		pc = -1; \

/** Yield control to the caller.
 *
 *  Updates the @a pc to the value @a label and then returns to the
 *  caller.  On the next invocation with the given value of @a pc, the
 *  @a CSNIP_CORO_START() will jump back to this location.
 *
 *  @param	pc
 *		integer "program counter", must be assignable and part
 *		of the state.
 *
 *  @param	label
 *		Positive integer literal to use to label this specific
 *		yield statement.  Every csnip_coro_Yield() in a given
 *		coroutine must have a unique integer label.  Otherwise
 *		there are no constraints on the labels; in particular,
 *		if all the yield statements appear on separate code
 *		lines, __LINE__ can be used to have to compiler assign a
 *		unique value itself.
 *
 *  @param	r
 *		The value to return from the coroutine.  If the
 *		return type of the coroutine is void, this must be empty or
 *		alternatively, contain a corresponding comment, such as,
 *		e.g., \/\* void \*\/.
 */
#define csnip_coro_Yield(pc, label, r) \
		do { \
			pc = label; \
			return r; \
			case label: ; \
		} while(0)

/** Terminate the coroutine.
 *
 *  Terminate the coroutine in a non-resumable fashion.
 *
 *  @param	pc
 *		integer "program counter", must be assignable and part
 *		of the state.
 *
 *  @param	r
 *		the value to return.
 */
#define csnip_coro_Return(pc, r) \
		do { \
			pc = -1; \
			return r; \
		} while(0)

/** @} */

#endif /* CSNIP_CORO_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_CORO_HAVE_SHORT_NAMES)
#define CORO_START			CSNIP_CORO_START
#define CORO_END			CSNIP_CORO_END
#define coro_Yield			csnip_coro_Yield
#define coro_Return			csnip_coro_Return
#endif /* CSNIP_SHORT_NAMES && !CSNIP_CORO_HAVE_SHORT_NAMES */
