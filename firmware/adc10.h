/* ADC10 definitions for the CC430F6147, so that we can target both
   the 6137 and the 6147 with a single firmware image.  periph.x was
   also modified.`
 */

/************************************************************
* ADC10_A
************************************************************/
#define __MSP430_HAS_ADC10_A__        /* Definition to show that Module is available */
#define __MSP430_BASEADDRESS_ADC10_A__ 0x0740

#define ADC10CTL0_            0x0740    /* ADC10 Control 0 */
sfrb(ADC10CTL0_L , ADC10CTL0_);
sfrb(ADC10CTL0_H , ADC10CTL0_+1);
sfrw(ADC10CTL0, ADC10CTL0_);
#define ADC10CTL1_            0x0742    /* ADC10 Control 1 */
sfrb(ADC10CTL1_L , ADC10CTL1_);
sfrb(ADC10CTL1_H , ADC10CTL1_+1);
sfrw(ADC10CTL1, ADC10CTL1_);
#define ADC10CTL2_            0x0744    /* ADC10 Control 2 */
sfrb(ADC10CTL2_L , ADC10CTL2_);
sfrb(ADC10CTL2_H , ADC10CTL2_+1);
sfrw(ADC10CTL2, ADC10CTL2_);
#define ADC10LO_              0x0746    /* ADC10 Window Comparator High Threshold */
sfrb(ADC10LO_L , ADC10LO_);
sfrb(ADC10LO_H , ADC10LO_+1);
sfrw(ADC10LO, ADC10LO_);
#define ADC10HI_              0x0748    /* ADC10 Window Comparator High Threshold */
sfrb(ADC10HI_L , ADC10HI_);
sfrb(ADC10HI_H , ADC10HI_+1);
sfrw(ADC10HI, ADC10HI_);
#define ADC10MCTL0_           0x074A    /* ADC10 Memory Control 0 */
sfrb(ADC10MCTL0_L , ADC10MCTL0_);
sfrb(ADC10MCTL0_H , ADC10MCTL0_+1);
sfrw(ADC10MCTL0, ADC10MCTL0_);
#define ADC10MEM0_            0x0752    /* ADC10 Conversion Memory 0 */
sfrb(ADC10MEM0_L , ADC10MEM0_);
sfrb(ADC10MEM0_H , ADC10MEM0_+1);
sfrw(ADC10MEM0, ADC10MEM0_);
#define ADC10IE_              0x075A    /* ADC10 Interrupt Enable */
sfrb(ADC10IE_L , ADC10IE_);
sfrb(ADC10IE_H , ADC10IE_+1);
sfrw(ADC10IE, ADC10IE_);
#define ADC10IFG_             0x075C    /* ADC10 Interrupt Flag */
sfrb(ADC10IFG_L , ADC10IFG_);
sfrb(ADC10IFG_H , ADC10IFG_+1);
sfrw(ADC10IFG, ADC10IFG_);
#define ADC10IV_              0x075E    /* ADC10 Interrupt Vector Word */
sfrb(ADC10IV_L , ADC10IV_);
sfrb(ADC10IV_H , ADC10IV_+1);
sfrw(ADC10IV, ADC10IV_);

/* ADC10CTL0 Control Bits */
#define ADC10SC             (0x0001)  /* ADC10 Start Conversion */
#define ADC10ENC            (0x0002)  /* ADC10 Enable Conversion */
#define ADC10ON             (0x0010)  /* ADC10 On/enable */
#define ADC10MSC            (0x0080)  /* ADC10 Multiple SampleConversion */
#define ADC10SHT0           (0x0100)  /* ADC10 Sample Hold Select Bit: 0 */
#define ADC10SHT1           (0x0200)  /* ADC10 Sample Hold Select Bit: 1 */
#define ADC10SHT2           (0x0400)  /* ADC10 Sample Hold Select Bit: 2 */
#define ADC10SHT3           (0x0800)  /* ADC10 Sample Hold Select Bit: 3 */

/* ADC10CTL0 Control Bits */
#define ADC10SC_L           (0x0001)  /* ADC10 Start Conversion */
#define ADC10ENC_L          (0x0002)  /* ADC10 Enable Conversion */
#define ADC10ON_L           (0x0010)  /* ADC10 On/enable */
#define ADC10MSC_L          (0x0080)  /* ADC10 Multiple SampleConversion */

/* ADC10CTL0 Control Bits */
#define ADC10SHT0_H         (0x0001)  /* ADC10 Sample Hold Select Bit: 0 */
#define ADC10SHT1_H         (0x0002)  /* ADC10 Sample Hold Select Bit: 1 */
#define ADC10SHT2_H         (0x0004)  /* ADC10 Sample Hold Select Bit: 2 */
#define ADC10SHT3_H         (0x0008)  /* ADC10 Sample Hold Select Bit: 3 */

#define ADC10SHT_0          (0x0000) /* ADC10 Sample Hold Select 0 */
#define ADC10SHT_1          (0x0100) /* ADC10 Sample Hold Select 1 */
#define ADC10SHT_2          (0x0200) /* ADC10 Sample Hold Select 2 */
#define ADC10SHT_3          (0x0300) /* ADC10 Sample Hold Select 3 */
#define ADC10SHT_4          (0x0400) /* ADC10 Sample Hold Select 4 */
#define ADC10SHT_5          (0x0500) /* ADC10 Sample Hold Select 5 */
#define ADC10SHT_6          (0x0600) /* ADC10 Sample Hold Select 6 */
#define ADC10SHT_7          (0x0700) /* ADC10 Sample Hold Select 7 */
#define ADC10SHT_8          (0x0800) /* ADC10 Sample Hold Select 8 */
#define ADC10SHT_9          (0x0900) /* ADC10 Sample Hold Select 9 */
#define ADC10SHT_10         (0x0A00) /* ADC10 Sample Hold Select 10 */
#define ADC10SHT_11         (0x0B00) /* ADC10 Sample Hold Select 11 */
#define ADC10SHT_12         (0x0C00) /* ADC10 Sample Hold Select 12 */
#define ADC10SHT_13         (0x0D00) /* ADC10 Sample Hold Select 13 */
#define ADC10SHT_14         (0x0E00) /* ADC10 Sample Hold Select 14 */
#define ADC10SHT_15         (0x0F00) /* ADC10 Sample Hold Select 15 */

/* ADC10CTL1 Control Bits */
#define ADC10BUSY           (0x0001)    /* ADC10 Busy */
#define ADC10CONSEQ0        (0x0002)    /* ADC10 Conversion Sequence Select 0 */
#define ADC10CONSEQ1        (0x0004)    /* ADC10 Conversion Sequence Select 1 */
#define ADC10SSEL0          (0x0008)    /* ADC10 Clock Source Select 0 */
#define ADC10SSEL1          (0x0010)    /* ADC10 Clock Source Select 1 */
#define ADC10DIV0           (0x0020)    /* ADC10 Clock Divider Select 0 */
#define ADC10DIV1           (0x0040)    /* ADC10 Clock Divider Select 1 */
#define ADC10DIV2           (0x0080)    /* ADC10 Clock Divider Select 2 */
#define ADC10ISSH           (0x0100)    /* ADC10 Invert Sample Hold Signal */
#define ADC10SHP            (0x0200)    /* ADC10 Sample/Hold Pulse Mode */
#define ADC10SHS0           (0x0400)    /* ADC10 Sample/Hold Source 0 */
#define ADC10SHS1           (0x0800)    /* ADC10 Sample/Hold Source 1 */

/* ADC10CTL1 Control Bits */
#define ADC10BUSY_L         (0x0001)    /* ADC10 Busy */
#define ADC10CONSEQ0_L      (0x0002)    /* ADC10 Conversion Sequence Select 0 */
#define ADC10CONSEQ1_L      (0x0004)    /* ADC10 Conversion Sequence Select 1 */
#define ADC10SSEL0_L        (0x0008)    /* ADC10 Clock Source Select 0 */
#define ADC10SSEL1_L        (0x0010)    /* ADC10 Clock Source Select 1 */
#define ADC10DIV0_L         (0x0020)    /* ADC10 Clock Divider Select 0 */
#define ADC10DIV1_L         (0x0040)    /* ADC10 Clock Divider Select 1 */
#define ADC10DIV2_L         (0x0080)    /* ADC10 Clock Divider Select 2 */

/* ADC10CTL1 Control Bits */
#define ADC10ISSH_H         (0x0001)    /* ADC10 Invert Sample Hold Signal */
#define ADC10SHP_H          (0x0002)    /* ADC10 Sample/Hold Pulse Mode */
#define ADC10SHS0_H         (0x0004)    /* ADC10 Sample/Hold Source 0 */
#define ADC10SHS1_H         (0x0008)    /* ADC10 Sample/Hold Source 1 */

#define ADC10CONSEQ_0        (0x0000)      /* ADC10 Conversion Sequence Select: 0 */
#define ADC10CONSEQ_1        (0x0002)      /* ADC10 Conversion Sequence Select: 1 */
#define ADC10CONSEQ_2        (0x0004)      /* ADC10 Conversion Sequence Select: 2 */
#define ADC10CONSEQ_3        (0x0006)      /* ADC10 Conversion Sequence Select: 3 */

#define ADC10SSEL_0          (0x0000)      /* ADC10 Clock Source Select: 0 */
#define ADC10SSEL_1          (0x0008)      /* ADC10 Clock Source Select: 1 */
#define ADC10SSEL_2          (0x0010)      /* ADC10 Clock Source Select: 2 */
#define ADC10SSEL_3          (0x0018)      /* ADC10 Clock Source Select: 3 */

#define ADC10DIV_0           (0x0000)   /* ADC10 Clock Divider Select: 0 */
#define ADC10DIV_1           (0x0020)   /* ADC10 Clock Divider Select: 1 */
#define ADC10DIV_2           (0x0040)   /* ADC10 Clock Divider Select: 2 */
#define ADC10DIV_3           (0x0060)   /* ADC10 Clock Divider Select: 3 */
#define ADC10DIV_4           (0x0080)   /* ADC10 Clock Divider Select: 4 */
#define ADC10DIV_5           (0x00A0)   /* ADC10 Clock Divider Select: 5 */
#define ADC10DIV_6           (0x00C0)   /* ADC10 Clock Divider Select: 6 */
#define ADC10DIV_7           (0x00E0)   /* ADC10 Clock Divider Select: 7 */

#define ADC10SHS_0           (0x0000)  /* ADC10 Sample/Hold Source: 0 */
#define ADC10SHS_1           (0x0400)  /* ADC10 Sample/Hold Source: 1 */
#define ADC10SHS_2           (0x0800)  /* ADC10 Sample/Hold Source: 2 */
#define ADC10SHS_3           (0x0C00)  /* ADC10 Sample/Hold Source: 3 */

/* ADC10CTL2 Control Bits */
#define ADC10REFBURST       (0x0001)    /* ADC10 Reference Burst */
#define ADC10SR             (0x0004)    /* ADC10 Sampling Rate */
#define ADC10DF             (0x0008)    /* ADC10 Data Format */
#define ADC10RES            (0x0010)    /* ADC10 Resolution Bit */
#define ADC10PDIV0          (0x0100)    /* ADC10 predivider Bit: 0 */
#define ADC10PDIV1          (0x0200)    /* ADC10 predivider Bit: 1 */

/* ADC10CTL2 Control Bits */
#define ADC10REFBURST_L     (0x0001)    /* ADC10 Reference Burst */
#define ADC10SR_L           (0x0004)    /* ADC10 Sampling Rate */
#define ADC10DF_L           (0x0008)    /* ADC10 Data Format */
#define ADC10RES_L          (0x0010)    /* ADC10 Resolution Bit */

/* ADC10CTL2 Control Bits */
#define ADC10PDIV0_H        (0x0001)    /* ADC10 predivider Bit: 0 */
#define ADC10PDIV1_H        (0x0002)    /* ADC10 predivider Bit: 1 */

#define ADC10PDIV_0         (0x0000)    /* ADC10 predivider /1 */
#define ADC10PDIV_1         (0x0100)    /* ADC10 predivider /2 */
#define ADC10PDIV_2         (0x0200)    /* ADC10 predivider /64 */
#define ADC10PDIV_3         (0x0300)    /* ADC10 predivider reserved */

#define ADC10PDIV__1        (0x0000)    /* ADC10 predivider /1 */
#define ADC10PDIV__4        (0x0100)    /* ADC10 predivider /2 */
#define ADC10PDIV__64       (0x0200)    /* ADC10 predivider /64 */

/* ADC10MCTL0 Control Bits */
#define ADC10INCH0          (0x0001)    /* ADC10 Input Channel Select Bit 0 */
#define ADC10INCH1          (0x0002)    /* ADC10 Input Channel Select Bit 1 */
#define ADC10INCH2          (0x0004)    /* ADC10 Input Channel Select Bit 2 */
#define ADC10INCH3          (0x0008)    /* ADC10 Input Channel Select Bit 3 */
#define ADC10SREF0          (0x0010)    /* ADC10 Select Reference Bit 0 */
#define ADC10SREF1          (0x0020)    /* ADC10 Select Reference Bit 1 */
#define ADC10SREF2          (0x0040)    /* ADC10 Select Reference Bit 2 */

/* ADC10MCTL0 Control Bits */
#define ADC10INCH0_L        (0x0001)    /* ADC10 Input Channel Select Bit 0 */
#define ADC10INCH1_L        (0x0002)    /* ADC10 Input Channel Select Bit 1 */
#define ADC10INCH2_L        (0x0004)    /* ADC10 Input Channel Select Bit 2 */
#define ADC10INCH3_L        (0x0008)    /* ADC10 Input Channel Select Bit 3 */
#define ADC10SREF0_L        (0x0010)    /* ADC10 Select Reference Bit 0 */
#define ADC10SREF1_L        (0x0020)    /* ADC10 Select Reference Bit 1 */
#define ADC10SREF2_L        (0x0040)    /* ADC10 Select Reference Bit 2 */

/* ADC10MCTL0 Control Bits */

#define ADC10INCH_0         (0)         /* ADC10 Input Channel 0 */
#define ADC10INCH_1         (1)         /* ADC10 Input Channel 1 */
#define ADC10INCH_2         (2)         /* ADC10 Input Channel 2 */
#define ADC10INCH_3         (3)         /* ADC10 Input Channel 3 */
#define ADC10INCH_4         (4)         /* ADC10 Input Channel 4 */
#define ADC10INCH_5         (5)         /* ADC10 Input Channel 5 */
#define ADC10INCH_6         (6)         /* ADC10 Input Channel 6 */
#define ADC10INCH_7         (7)         /* ADC10 Input Channel 7 */
#define ADC10INCH_8         (8)         /* ADC10 Input Channel 8 */
#define ADC10INCH_9         (9)         /* ADC10 Input Channel 9 */
#define ADC10INCH_10        (10)        /* ADC10 Input Channel 10 */
#define ADC10INCH_11        (11)        /* ADC10 Input Channel 11 */
#define ADC10INCH_12        (12)        /* ADC10 Input Channel 12 */
#define ADC10INCH_13        (13)        /* ADC10 Input Channel 13 */
#define ADC10INCH_14        (14)        /* ADC10 Input Channel 14 */
#define ADC10INCH_15        (15)        /* ADC10 Input Channel 15 */

#define ADC10SREF_0         (0x0000)    /* ADC10 Select Reference 0 */
#define ADC10SREF_1         (0x0010)    /* ADC10 Select Reference 1 */
#define ADC10SREF_2         (0x0020)    /* ADC10 Select Reference 2 */
#define ADC10SREF_3         (0x0030)    /* ADC10 Select Reference 3 */
#define ADC10SREF_4         (0x0040)    /* ADC10 Select Reference 4 */
#define ADC10SREF_5         (0x0050)    /* ADC10 Select Reference 5 */
#define ADC10SREF_6         (0x0060)    /* ADC10 Select Reference 6 */
#define ADC10SREF_7         (0x0070)    /* ADC10 Select Reference 7 */

/* ADC10IE Interrupt Enable Bits */
#define ADC10IE0            (0x0001)    /* ADC10_A Interrupt enable */
#define ADC10INIE           (0x0002)    /* ADC10_A Interrupt enable for the inside of window of the Window comparator */
#define ADC10LOIE           (0x0004)    /* ADC10_A Interrupt enable for lower threshold of the Window comparator */
#define ADC10HIIE           (0x0008)    /* ADC10_A Interrupt enable for upper threshold of the Window comparator */
#define ADC10OVIE           (0x0010)    /* ADC10_A ADC10MEM overflow Interrupt enable */
#define ADC10TOVIE          (0x0020)    /* ADC10_A conversion-time-overflow Interrupt enable */

/* ADC10IE Interrupt Enable Bits */
#define ADC10IE0_L          (0x0001)    /* ADC10_A Interrupt enable */
#define ADC10INIE_L         (0x0002)    /* ADC10_A Interrupt enable for the inside of window of the Window comparator */
#define ADC10LOIE_L         (0x0004)    /* ADC10_A Interrupt enable for lower threshold of the Window comparator */
#define ADC10HIIE_L         (0x0008)    /* ADC10_A Interrupt enable for upper threshold of the Window comparator */
#define ADC10OVIE_L         (0x0010)    /* ADC10_A ADC10MEM overflow Interrupt enable */
#define ADC10TOVIE_L        (0x0020)    /* ADC10_A conversion-time-overflow Interrupt enable */

/* ADC10IE Interrupt Enable Bits */

/* ADC10IFG Interrupt Flag Bits */
#define ADC10IFG0           (0x0001)    /* ADC10_A Interrupt Flag */
#define ADC10INIFG          (0x0002)    /* ADC10_A Interrupt Flag for the inside of window of the Window comparator */
#define ADC10LOIFG          (0x0004)    /* ADC10_A Interrupt Flag for lower threshold of the Window comparator */
#define ADC10HIIFG          (0x0008)    /* ADC10_A Interrupt Flag for upper threshold of the Window comparator */
#define ADC10OVIFG          (0x0010)    /* ADC10_A ADC10MEM overflow Interrupt Flag */
#define ADC10TOVIFG         (0x0020)    /* ADC10_A conversion-time-overflow Interrupt Flag */

/* ADC10IFG Interrupt Flag Bits */
#define ADC10IFG0_L         (0x0001)    /* ADC10_A Interrupt Flag */
#define ADC10INIFG_L        (0x0002)    /* ADC10_A Interrupt Flag for the inside of window of the Window comparator */
#define ADC10LOIFG_L        (0x0004)    /* ADC10_A Interrupt Flag for lower threshold of the Window comparator */
#define ADC10HIIFG_L        (0x0008)    /* ADC10_A Interrupt Flag for upper threshold of the Window comparator */
#define ADC10OVIFG_L        (0x0010)    /* ADC10_A ADC10MEM overflow Interrupt Flag */
#define ADC10TOVIFG_L       (0x0020)    /* ADC10_A conversion-time-overflow Interrupt Flag */

/* ADC10IFG Interrupt Flag Bits */

/* ADC10IV Definitions */
#define ADC10IV_NONE        (0x0000)    /* No Interrupt pending */
#define ADC10IV_ADC10OVIFG  (0x0002)    /* ADC10OVIFG */
#define ADC10IV_ADC10TOVIFG (0x0004)    /* ADC10TOVIFG */
#define ADC10IV_ADC10HIIFG  (0x0006)    /* ADC10HIIFG */
#define ADC10IV_ADC10LOIFG  (0x0008)    /* ADC10LOIFG */
#define ADC10IV_ADC10INIFG  (0x000A)    /* ADC10INIFG */
#define ADC10IV_ADC10IFG    (0x000C)    /* ADC10IFG */
