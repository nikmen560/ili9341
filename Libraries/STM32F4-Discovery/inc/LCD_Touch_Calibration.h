/*
*******************************************************************************
  Copyright (C), 2010-2011, timll Tech. Co., Ltd.
  FileName		: LCD_Touch_Calibration.h
  Author		: lichy       
  Version 		: 1.0.0     
  Date			: 2011/09/19
  Description	: 
  Function List : 
        
  History		:
      			<author>  	£ºlichy       
				<time>   	£º11/09/19
				<version >  £º1.0.0 	
				<desc>		: build this moudle														
*******************************************************************************
*/
#ifndef _LCD_TOUCH_CALIBRATION_H_
#define _LCD_TOUCH_CALIBRATION_H_

typedef struct{
    uint16_t x;
    uint16_t y;
}Point_Struct;

typedef struct{
    /*scale factor for x direction*/
    float    xScale;
    /*scale factor for x direction*/
    float    yScale;
    /*Offset in x direction*/
    int16_t  xOffset;
    /*Offset in y direction*/
    int16_t  yOffset;
}Adjust_Struct;

#ifdef LCD_TOUCH_CALIBRATION_MODULE
    #define LCD_TOUCH_CALIBRATION_EXT
#else
    #define LCD_TOUCH_CALIBRATION_EXT extern
#endif

/* Private define ------------------------------------------------------------*/

    
/* External variables prototypes ---------------------------------------------*/
LCD_TOUCH_CALIBRATION_EXT Adjust_Struct adjust_Para;

/* Private functions ---------------------------------------------------------*/
void    Lcd_Touch_Calibration             (void);
void 	Calibration_Test_Dispose		  (void);
#endif
