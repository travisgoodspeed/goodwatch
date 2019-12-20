/* \file rtcasm.S
   \brief Hand assembly to set the RTC.

   I've modified this code from the CCE example to work with GCC,
   which mostly involved correcting the calling convention.  (The first
   parameter is R12 in CCE and GCC8 but R15 in GCC4.)

   This module ought to be 32-bit aligned, and every NOP counts.  I've
   not yet verified that GNU LD isn't screwing up the alignment.

   --Travis
*/

;******************************************************************************
;  MSP430FG5438 - Real Time Clock Workaround
;
;  Description: This program demonstrates the RTC Workaround for the RTC
;  Errata deailed in the XMS '5438 Errata under RTC3
;  Assembly functions for the RTC workaround
;  Allows safe access to the RTC registers that potentially have a write issue
;  Code is aligned so that the lower 5 bits of the address of the instruction
;  fetch following the RTC register write are equal
;  Do not change the code in the ASM file (.s43) all nops in there are needed.
;
;  Texas Instruments Inc.
;  October 2008
;  Built with CCE version 3.2.2
;
;*******************************************************************************
; !!!!!! Do not change the code. Every nop has a function !!!!!!
;*******************************************************************************

;	.cdecls C,LIST,"msp430x54x.h" ; Include device header file
#include <msp430.h>	
;-------------------------------------------------------------------------------
            .text                           ; Program Start
;-------------------------------------------------------------------------------
				;            .align  32
.balign 4
;-------------------------------------------------------------------------------
            .global SetRTCYEAR
; int SetRTCYEAR(int year)
; year will be set in the RTCYEAR register
; return value is read back from the RTCYEAR register
; Works only in calendar mode (RTCMODE = 1)
; argument year is in R15
; return value is in R15
SetRTCYEAR: nop
            nop
            nop
            nop
            dint
            nop
            nop
            nop
            mov.w   R15,&RTCYEAR            ; Set RTCYEAR to year 		
            nop
            nop
            mov.w   &RTCYEAR,R15            ; Return value
            eint
            ret

            
            
;-------------------------------------------------------------------------------
			.global	SetRTCMON
; int SetRTCMON(int month)
; month will be set in the RTCMON register
; Workaround requires to write 16 bits into RTCDATE
; return value is read back from the RTCMON register
; argument month is in R15
; return value is in R15
SetRTCMON:  push.w  R5                      ; push R5
            mov.b   &RTCDAY,R5              ; read RTCDAY into R5
            swpb    R15                     ; R15 holds month - swap lower and upper byte
            bis.w   R15,R5                  ; combine read RTCDAY and new RTCMON
            dint
            nop
            mov.w   R5,&RTCDATE             ; write to RTCDATE		
            nop
            nop
            nop
            nop
            eint
            mov.b   &RTCMON,R15             ; Read RTCMON register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------
			.global	SetRTCDAY
; int SetRTCDAY(int day)
; day will be set in the RTCDAY register
; Workaround requires to write 16 bits into RTCDATE
; return value is read back from the RTCDAY register
; argument day is in R15
; return value is in R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
SetRTCDAY:                                  ; R15 holds day in lower byte
            push.w  R5                      ; push R5
            mov.b   &RTCMON,R5              ; read RTCMON into lower byte of R5
            swpb    R5                      ; month is now in higher byte of R5
            bic.w   #0x00FF,R5               ; clear lower byte of R5
            bis.w   R15,R5                  ; combine read RTCDAY and new RTCMON
            dint
            nop
            mov.w   R5,&RTCDATE             ; write to RTCDATE		
            nop
            eint
            mov.b   &RTCDAY,R15             ; Read RTCDAY register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------
            .global	SetRTCDOW
; int SetRTCDOW(int dow)
; dow will be set in the RTCDOW register
; Workaround requires to write 16 bits into RTCTIM1
; return value is read back from the RTCDOW register
; argument dow is in R15
; return value is in R15
SetRTCDOW:  push.w  R5                      ; push R5
            mov.b   &RTCHOUR,R5             ; read RTCHOUR into R5
            swpb    R15                     ; R15 holds dow - swap lower and upper byte
            bis.w   R15, R5                 ; combine read RTCHOUR and new RTCDOW
            dint
            nop
            mov.w   R5,&RTCTIM1             ; write to RTCTIM1		
            nop
            eint
            mov.b   &RTCDOW,R15             ; Read RTCDOW register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------
            .global	SetRTCHOUR
; int SetRTCHOUR(int hour)
; hour will be set in the RTCHOUR register
; Workaround requires to write 16 bits into RTCTIM1
; return value is read back from the RTCHOUR register
; argument hour is in R15
; return value is in R15
SetRTCHOUR:                                 ; R15 holds day in lower byte
            push.w  R5                      ; push R5
            mov.b   &RTCDOW,R5              ; read RTCDOW into lower byte of R5
            swpb    R5                      ; dow is now in higher byte of R5
            bis.w   R15,R5                  ; combine read RTCDOW and new RTCHOUR
            dint
            nop
            nop
            mov.w   R5,&RTCTIM1             ; write to RTCTIM1		
            nop
            eint
            mov.b   &RTCHOUR,R15            ; Read RTCHOUR register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------
            .global	SetRTCMIN
; int SetRTCMIN(int min)
; min will be set in the RTCMIN register
; Workaround requires to write 16 bits into RTCTIM0
; return value is read back from the RTCDOW register
; argument min is in R15
; return value is in R15
SetRTCMIN:  push.w  R5                      ; push R5
            mov.b   &RTCSEC, R5             ; read RTCSEC into R5
            swpb    R15                     ; R15 holds min - swap lower and upper byte
            bis.w   R15,R5                  ; combine read RTCMIN and new RTCSEC
            dint
            nop
            mov.w   R5,&RTCTIM0             ; write to RTCTIM0
            nop
            eint
            mov.b   &RTCMIN,R15             ; Read RTCDOW register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------
            .global	SetRTCSEC
; int SetRTCSEC(int sec)
; sec will be set in the RTCSEC register
; Workaround requires to write 16 bits into RTCTIM0
; return value is read back from the RTCSEC register
; argument sec is in R15
; return value is in R15
SetRTCSEC:                                  ; R15 holds day in lower byte
            push.w  R5                      ; push R5
            mov.b   &RTCMIN, R5             ; read RTCMIN into lower byte of R5
            swpb    R5                      ; min is now in higher byte of R5
            bis.w   R15, R5                 ; combine read RTCMIN and new RTCSEC
            dint
            nop
            nop
            mov.w   R5,&RTCTIM0             ; write to RTCTIM0		
            nop
            eint
            mov.b   &RTCSEC, R15            ; Read RTCSEC register
            pop.w   R5                      ; pop R5
            ret

;-------------------------------------------------------------------------------	

            .global GetRTCTIM0
; Read the RTC registers RTCTIM0
; return the values 
GetRTCTIM0:
            mov.w   &RTCTIM0,R15
            ret


            .global GetRTCTIM1
; Read the RTC register RTCTIM1
; return the values 
GetRTCTIM1:
            mov.w   &RTCTIM1,R15
            ret


            .global GetRTCDATE
; Read the RTC register RTCDATE
; return the values 
GetRTCDATE:
            mov.w   &RTCDATE,R15
            ret


            .global GetRTCYEAR
; Read the RTC registers RTCYEAR
; return the values 
GetRTCYEAR:
            mov.w   &RTCYEAR, R15
            ret


            .global GetRTCMON
; Read the RTC register RTCMON
; return the values 
GetRTCMON:
            mov.b   &RTCMON, R15
            ret


            .global GetRTCDOW
; Read the RTC register RTCDOW
; return the values 
GetRTCDOW:
            mov.b   &RTCDOW, R15
            ret


            .global GetRTCDAY
; Read the RTC register RTCDOW
; return the values 
GetRTCDAY:
            mov.b   &RTCDAY, R15
            ret


            .global GetRTCHOUR
; Read the RTC register RTCDOW
; return the values 
GetRTCHOUR:
            mov.b   &RTCHOUR, R15
            ret


            .global GetRTCMIN
; Read the RTC register RTCDOW
; return the values 
GetRTCMIN:
            mov.b   &RTCMIN, R15
            ret


            .global GetRTCSEC
; Read the RTC register RTCDOW
; return the values 
GetRTCSEC:
            mov.b   &RTCSEC, R15
            ret


