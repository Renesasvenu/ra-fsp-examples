/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : hal_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.
 * This function is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
    /* TODO: add your own code here */

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
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
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is
 * not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif