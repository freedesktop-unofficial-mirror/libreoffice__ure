/*************************************************************************
 *
 *  $RCSfile: interlck_sparc.s,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-09 11:45:50 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/


/*
 * Implements osl_[increment|decrement]InterlockedCount in two ways:
 * sparcv8 architecture:                use the "swap" instruction
 * sparcv9/sparcv8plus architecture:    use the "cas"  instruction
 * 
 * 32 bit mode with v8 and v8plus support:
 * Initialize once with osl_InterlockedCountSetV9(int bv9) if you want to
 * use the "cas" instruction, which is faster (no spinlock needed)
 * Default is to use the "swap" instruction, which works on all supported
 * SPARC cpu's
 * 
 * osl_InterlockedCountSetV9(int bv9)
 *    bv9 = 0   use sparcv8 "swap" (spinlock)
 *    bv9 = 1   use sparcv9/sparcv8plus "cas" (no spinlock)
 *
 * 32 bit mode without v8 support (implies v8plus) or 64 bit mode:
 * No need (nor the possibilty) to call osl_InterlockedCountSetV9(), 
 * sparcv9 mode is implied. Assemble with -xarch=v8plus (32 bit) or
 * -xarch=v9 (64 bit).
 * 
 */

#if !defined(__sparcv8plus) && !defined(__sparcv9) && !defined(__sparc_v9__)

.section ".data"
.align 4
osl_incrementInterLockCountFuncPtr:
.word osl_incrementInterlockedCountV8
.type osl_incrementInterLockCountFuncPtr,#object
.size osl_incrementInterLockCountFuncPtr,4

.align 4
osl_decrementInterLockCountFuncPtr:
.word osl_decrementInterlockedCountV8
.type osl_decrementInterLockCountFuncPtr,#object
.size osl_decrementInterLockCountFuncPtr,4

.section   ".text"

#if defined(NETBSD) || defined(LINUX)
/* add the address of the calling "call" instruction (stored in %o7) to
 * %o5 which contains _GLOBAL_OFFSET_TABLE_
 */
.Laddoseven:
        retl
        add %o7, %o5, %o5
#endif

 .global   osl_incrementInterlockedCount
 .align   4

osl_incrementInterlockedCount:

#if defined(NETBSD) || defined(LINUX)
        mov     %o7, %g1
        sethi   %hi(_GLOBAL_OFFSET_TABLE_-4), %o5
        call    .Laddoseven
        add     %o5, %lo(_GLOBAL_OFFSET_TABLE_+4), %o5
        mov     %g1, %o7
#endif
        set     osl_incrementInterLockCountFuncPtr, %o1
#if defined(NETBSD)
        ld      [%o1 + %o5], %o1
#endif
        ld      [%o1], %o1
        jmp     %o1
        nop                                             ! delay slot
 .type  osl_incrementInterlockedCount,#function
 .size  osl_incrementInterlockedCount,.-osl_incrementInterlockedCount

.section   ".text"
 .global   osl_decrementInterlockedCount
 .align   4

osl_decrementInterlockedCount:

#if defined(NETBSD) || defined(LINUX)
        mov     %o7, %g1
        sethi   %hi(_GLOBAL_OFFSET_TABLE_-4), %o5
        call    .Laddoseven
        add     %o5, %lo(_GLOBAL_OFFSET_TABLE_+4), %o5
        mov     %g1, %o7
#endif
        set     osl_decrementInterLockCountFuncPtr, %o1
#if defined(NETBSD) || defined(LINUX)
        ld      [%o1 + %o5], %o1
#endif
        ld      [%o1], %o1
        jmp     %o1
        nop                                             ! delay slot
 .type  osl_decrementInterlockedCount,#function
 .size  osl_decrementInterlockedCount,.-osl_decrementInterlockedCount

.section   ".text"
 .global   osl_InterlockedCountSetV9
 .align   4

osl_InterlockedCountSetV9:

#if defined(NETBSD) || defined(LINUX)
        mov	    %o7, %g1
        sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %o5
        call	.Laddoseven
        add	    %o5, %lo(_GLOBAL_OFFSET_TABLE_+4), %o5
        mov	    %g1, %o7
#endif
        set     osl_incrementInterLockCountFuncPtr, %o1
        set     osl_decrementInterLockCountFuncPtr, %o2
        cmp     %o0, %g0
        bnz     1f
        nop                                             ! delay slot
        set     osl_incrementInterlockedCountV8, %o0
        set     osl_decrementInterlockedCountV8, %o3
#if defined(NETBSD) || defined(LINUX)
        ld      [%o0 + %o5], %o0
        ld      [%o1 + %o5], %o1
        ld      [%o2 + %o5], %o2
        ld      [%o3 + %o5], %o3
#endif
        st      %o3,[%o2]
        retl
        st      %o0,[%o1]
1:      set     osl_incrementInterlockedCountV9, %o0
        set     osl_decrementInterlockedCountV9, %o3
#if defined(NETBSD) || defined(LINUX)
        ld      [%o0 + %o5], %o0
        ld      [%o1 + %o5], %o1
        ld      [%o2 + %o5], %o2
        ld      [%o3 + %o5], %o3
#endif
        st      %o3,[%o2]
        retl
        st      %o0,[%o1]

 .type  osl_InterlockedCountSetV9,#function
 .size  osl_InterlockedCountSetV9,.-osl_InterlockedCountSetV9
 

.section   ".text"
 .local   osl_incrementInterlockedCountV8
 .align   4

! Implements osl_[increment|decrement]InterlockedCount with sparcv8 "swap" instruction.
! Uses -4096 as lock value for spinlock to allow for small negative counts.

osl_incrementInterlockedCountV8:             

1:      ld      [%o0], %o1
        cmp     %o1, -4096          ! test spinlock
        be      1b
        mov     -4096, %o1          ! delay slot
        swap    [%o0], %o1
        cmp     %o1, -4096
        be      1b
        inc     %o1                 ! delay slot, if we got spinlock, increment count
        st      %o1, [%o0]
        retl
        mov     %o1, %o0            ! delay slot

 .type  osl_incrementInterlockedCountV8,#function
 .size  osl_incrementInterlockedCountV8,.-osl_incrementInterlockedCountV8


.section   ".text"
 .local osl_decrementInterlockedCountV8
 .align   4

osl_decrementInterlockedCountV8:             

1:      ld      [%o0], %o1
        cmp     %o1, -4096          ! test spinlock
        be      1b
        mov     -4096, %o1          ! delay slot
        swap    [%o0], %o1
        cmp     %o1, -4096
        be      1b
        dec     %o1                 ! delay slot, if we got spinlock, decrement count
        st      %o1, [%o0]          ! delay slot
        retl
        mov     %o1, %o0            ! delay slot

 .type  osl_decrementInterlockedCountV8,#function
 .size  osl_decrementInterlockedCountV8,.-osl_decrementInterlockedCountV8

#endif /* !__sparcv8plus && !__sparcv9 && !_sparcv9__ */

.section   ".text"
#if defined(__sparcv8plus) || defined(__sparcv9) || defined(__sparc_v9__)
#define   osl_incrementInterlockedCountV9 osl_incrementInterlockedCount
 .global  osl_incrementInterlockedCountV9
#else
 .local   osl_incrementInterlockedCountV9
#endif
 .align   8

!   Implements osl_[increment|decrement]InterlockedCount with sparcv9(sparcv8plus) "cas" 
!   instruction.

osl_incrementInterlockedCountV9:             

1:      ld      [%o0], %o1
        add     %o1, 1, %o2
!       allow linux to build for v8
        .word 0xD5E21009 
!       cas     [%o0], %o1, %o2
        cmp     %o1, %o2
        bne     1b
        nop                         ! delay slot
        retl
        add     %o2, 1, %o0         ! delay slot

 .type  osl_incrementInterlockedCountV9,#function
 .size  osl_incrementInterlockedCountV9,.-osl_incrementInterlockedCountV9


.section   ".text"
#if defined(__sparcv8plus) || defined(__sparcv9) || defined(__sparc_v9__)
#define   osl_decrementInterlockedCountV9 osl_decrementInterlockedCount
 .global  osl_decrementInterlockedCountV9
#else
 .local   osl_decrementInterlockedCountV9
#endif
 .align   8

osl_decrementInterlockedCountV9:             

1:      ld      [%o0], %o1
        sub     %o1, 1, %o2
!       allow linux to build for v8
        .word 0xD5E21009            
!       cas     [%o0], %o1, %o2
        cmp     %o1, %o2
        bne     1b
        nop                         ! delay slot
        retl
        sub     %o2, 1, %o0         ! delay slot

 .type  osl_decrementInterlockedCountV9,#function
 .size  osl_decrementInterlockedCountV9,.-osl_decrementInterlockedCountV9
