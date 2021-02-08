/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "st7735/st7735.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_buf_t disp_buf_3;
static lv_color_t buf3_1[LV_HOR_RES_MAX * 20];            /*A screen sized buffer*/
static lv_color_t buf3_2[LV_HOR_RES_MAX * 20];            /*An other screen sized buffer*/

static lv_disp_drv_t * p_disp_drv;
extern uint8_t completed1;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
//	 lv_disp_buf_init(&disp_buf_3, buf3_1, NULL, LV_HOR_RES_MAX * 10);
    lv_disp_buf_init(&disp_buf_3, buf3_1, buf3_2, sizeof(buf3_1)/sizeof(lv_color_t));   

    lv_disp_drv_t disp_drv;                     
    lv_disp_drv_init(&disp_drv);                  

    disp_drv.flush_cb = disp_flush;
    disp_drv.buffer = &disp_buf_3;
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    int32_t x;
    int32_t y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            /* Put a pixel to the display. For example: */
            /* put_px(x, y, *color_p)*/
						ST7735_DrawPixel(x,y,color_p->full);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}
//static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
//{
//    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
//    p_disp_drv = disp_drv;
//    int16_t w = (area->x2 - area->x1 + 1);
//    int16_t h = (area->y2 - area->y1 + 1);
//    uint32_t size = w * h * 2;
//    ST7735_SetAddressWindow(area->x1, area->y1, area->x2, area->y2);
//    LCD_CS_CLR();
//    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
//		completed1=0;
//    HAL_SPI_Transmit_DMA(&hspi1,(void *)color_p,size);


//}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi==&hspi1)
	{
    LCD_CS_SET();
    lv_disp_flush_ready(p_disp_drv);
	}
}
#endif
