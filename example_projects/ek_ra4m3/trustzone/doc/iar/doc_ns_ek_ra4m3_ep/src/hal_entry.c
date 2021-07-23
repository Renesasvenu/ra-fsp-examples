/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * @addtogroup DOC_NS_EP
 * @{
 **********************************************************************************************************************/
/** Macros **/
#define DOC_DATA_SUB                    0xA5A4      //To write the first data to doc register for subtraction event
#define DOC_DATA_UNDERFLOW              0x0002      //To write the second data to doc register for subtraction event
#define DOC_DATA_ADD                    0X5A5A      //To write the first data to doc register for addition event
#define DOC_DATA_OVERFLOW               0x0001      //To write the second data to doc register for addition event to overflow
#define DOC_DATA_COMPARISON_MATCH       0xA5A5      //To write the data to doc register for comparison match event
#define DOC_DATA_COMPARISON_MISMATCH    0xB5B5      //To write the data to doc register for comparison mismatch event

static void doc_deinit(void);

/** Global Variables **/
static bool b_doc_event_flag = false;           //Event counter updates for every callback triggers
extern bsp_leds_t g_bsp_leds;                  // LED Structure used to blink on board LED

/* The 2 following prototypes for custom guard function can be removed in future */
fsp_err_t led_set_guard (bsp_io_port_pin_t pin, bsp_io_level_t level);
doc_event_t doc_cfg_event_read_guard (void);

doc_callback_args_t g_doc_callback_args;
void doc_callback(doc_callback_args_t *p_args);

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
     fsp_pack_version_t version = {RESET_VALUE};
     fsp_err_t err              = FSP_SUCCESS;
     /* Version get API for FLEX pack information */
     R_FSP_VersionGet(&version);

     /* Example Project information printed on the Console */
     APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);

     APP_PRINT("\r\nThis project demonstrates the basic functionality of DOC driver."
             "\r\nDOC module performs one of the four operations(Comparison Mismatch, Comparison Match,"
             "\r\nAddition Overflow & Subtraction Underflow) on reference data based on the user selection in RA configurator."
             "\r\nStatus and Result of the operation is printed on RTTViewer. "
             "\r\nLED turns ON to indicate the success of operation and stays OFF if the operation fails.\r\n");

     /*
      Initialize the DOC module for Comparison/Addition/Subtraction Operations with Initial value written in DOC Data Setting Register
      Note:
		 Arguments are passed in as NULL since the guard functions will ignore them.
		 The guard functions are generated in the secure project doc_s_ek_ra6m4_ep/src/g_doc_guard.c
	  */
     err = g_doc_open_guard (NULL, NULL);
     /* Handle Error */
     if(FSP_SUCCESS != err)
     {
         APP_ERR_PRINT("\r\n ** DOC Open API Failed **");
         APP_ERR_TRAP(err);
     }

     err = g_doc_callback_set_guard(NULL, doc_callback, NULL, &g_doc_callback_args);
     /* Handle Error */
     if(FSP_SUCCESS != err)
     {
         APP_ERR_PRINT("\r\n ** g_doc_callback_set_guard API Failed **");
         APP_ERR_TRAP(err);
     }

     /* Write the predefined data to doc register, depends on the selected event operations will take place*/
     switch(doc_cfg_event_read_guard ())
     {
         case DOC_EVENT_COMPARISON_MISMATCH:
             APP_PRINT("\r\nSelected event is Comparison MisMastch");
             /* write mismatch to the DOC Data Input Register */
             err = g_doc_write_guard (NULL, DOC_DATA_COMPARISON_MISMATCH);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit ();
                 APP_ERR_TRAP(err);
             }
             break;

         case DOC_EVENT_COMPARISON_MATCH:
             APP_PRINT("\r\nSelected event is Comparison Match");
             /* write match data to the DOC Data Input Register */
             err = g_doc_write_guard (NULL, DOC_DATA_COMPARISON_MATCH);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit ();
                 APP_ERR_TRAP(err);
             }
             break;

         case DOC_EVENT_ADDITION:
             APP_PRINT("\r\nSelected event is Addition Overflow");
             /* write first data to the DOC Data Input Register */
             err = g_doc_write_guard (NULL, DOC_DATA_ADD);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit ();
                 APP_ERR_TRAP(err);
             }
             /* write second data to the DOC Data Input Register to occur overflow event */
             err = g_doc_write_guard(NULL, DOC_DATA_OVERFLOW);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit ();
                 APP_ERR_TRAP(err);
             }
             break;

         case DOC_EVENT_SUBTRACTION:
             APP_PRINT("\r\nSelected event is Subtraction Underflow");
             /* write first data to the DOC Data Input Register */
             err = g_doc_write_guard (NULL, DOC_DATA_SUB);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit();
                 APP_ERR_TRAP(err);
             }
             /* write second data to the DOC Data Input Register to occur underflow event */
             err = g_doc_write_guard (NULL, DOC_DATA_UNDERFLOW);
             /* Handle Error */
             if (FSP_SUCCESS != err)
             {
                 APP_ERR_PRINT("\r\n ** DOC Write API Failed **");
                 doc_deinit();
                 APP_ERR_TRAP(err);
             }
             break;
         default: break;
     }

      /* Prints the RTT message when interrupt occurs upon successful event operations.*/
     if (true == b_doc_event_flag)
     {
         b_doc_event_flag = false;   //RESET flag
         APP_PRINT("\r\nDOC operation is successful for the selected event");
         /* Turn ON LED to indicate callback triggered, along with output on RTT*/
         led_set_guard((bsp_io_port_pin_t)g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
     }
     else
     {
         APP_PRINT("\r\nDOC operation failed for the selected event");
     }
     APP_PRINT("\r\nRestart the application to rerun the code");
     while (true)
     {
         ;
     }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * @brief   Based on the selected event in the configurator, The doc call back will occur for the selected
 *          event when operations are performed with the reference data.
  * @param[in]   p_args
 * @return      None
 **********************************************************************************************************************/
void doc_callback(doc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set the event flag */
    b_doc_event_flag = true;
}

/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the DOC module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void doc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_doc_close_guard (NULL);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n **DOC Close API failed**");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
	if (BSP_WARM_START_RESET == event) {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
	}

	if (BSP_WARM_START_POST_C == event) {
		/* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

/*******************************************************************************************************************//**
 * @} (end defgroup DOC_NS_EP)
 **********************************************************************************************************************/
