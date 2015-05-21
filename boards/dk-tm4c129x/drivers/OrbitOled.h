/************************************************************************/
/*																		*/
/*	OrbitOled.h	--	Interface Declarations for OLED Display Driver 		*/
/*																		*/
/************************************************************************/
/*	Author:		Gene Apperson											*/
/*	Copyright 2013, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/29/2011(GeneA): created for PmodOLED								*/
/*	04/04/2013(JordanR):  Ported for Stellaris LaunchPad + Orbit BP		*/
/*	06/06/2013(JordanR):  Prepared for release							*/
/*																		*/
/************************************************************************/

#if !defined(ORBITOLED_INC)
#define	ORBITOLED_INC

#include "driverlib/pin_map.h"

/* ------------------------------------------------------------ */
/*					Miscellaneous Declarations					*/
/* ------------------------------------------------------------ */

/*
 * OLED Control
 */
#define SCK_OLEDPort	GPIO_PORTE_BASE
#define SDI_OLEDPort	GPIO_PORTK_BASE
#define VBAT_OLEDPort	GPIO_PORTK_BASE
#define VDD_OLEDPort	GPIO_PORTK_BASE
#define nCS_OLEDPort	GPIO_PORTE_BASE
#define nRES_OLEDPort	GPIO_PORTF_BASE
#define nDC_OLEDPort	GPIO_PORTD_BASE
#define SCK_OLED_PIN	GPIO_PIN_3
#define SDI_OLED_PIN	GPIO_PIN_1
#define SCK_OLED		GPIO_PE3_SSI3CLK	// GPIO_PD0_SSI3CLK <- hard coded, should be in pin_map.h
#define SDI_OLED		GPIO_PK1_SSI3TX	// GPIO_PD3_SSI3TX  <- hard coded, should be in pin_map.h
#define VBAT_OLED		GPIO_PIN_2
#define VDD_OLED		GPIO_PIN_3
#define nCS_OLED		GPIO_PIN_6
#define nRES_OLED		GPIO_PIN_3
#define nDC_OLED		GPIO_PIN_2

#define LOW				0
#define HIGH			1

#define	cbOledDispMax	512		//max number of bytes in display buffer

#define	ccolOledMax		128		//number of display columns
#define	crowOledMax		32		//number of display rows
#define	cpagOledMax		4		//number of display memory pages

#define	cbOledChar		8		//font glyph definitions is 8 bytes long
#define	chOledUserMax	0x20	//number of character defs in user font table
#define	cbOledFontUser	(chOledUserMax*cbOledChar)

/* Graphics drawing modes.
*/
#define	modOledSet		0
#define	modOledOr		1
#define	modOledAnd		2
#define	modOledXor		3

/* ------------------------------------------------------------ */
/*					General Type Declarations					*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*					Object Class Declarations					*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					Variable Declarations						*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

void	OrbitOledInit();
void	OrbitOledClear();
void	OrbitOledClearBuffer();
void	OrbitOledUpdate();

/* ------------------------------------------------------------ */

#endif

/************************************************************************/
