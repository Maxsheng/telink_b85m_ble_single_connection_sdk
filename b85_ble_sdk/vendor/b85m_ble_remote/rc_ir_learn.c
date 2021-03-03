/********************************************************************************************************
 * @file	 rc_ir_learn.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/


#include "tl_common.h"
#include "drivers.h"

#include "rc_ir.h"
#include "rc_ir_learn.h"

#if (REMOTE_IR_LEARN_ENABLE)


ir_learn_ctrl_t	ir_learn_ctrl;
ir_learn_ctrl_t *g_ir_learn_ctrl = &ir_learn_ctrl;
ir_send_dma_data_t ir_send_dma_data;
extern ir_send_ctrl_t ir_send_ctrl;

/************************************************************************
* 【Function】ir_learn_init(void)				  			 				*
* 【Description】 ir learn init algorithm ,set related GPIO function and	*
* 			    irq related. 											*
************************************************************************/
void ir_learn_init(void)
{

	memset((void*)g_ir_learn_ctrl, 0, sizeof(ir_learn_ctrl_t));  //ir_learn_ctrl_t is same as YF

	// To output ircontrol and irout low, then ir receive can work.
	gpio_set_func(GPIO_IR_OUT, AS_GPIO);
	gpio_set_input_en(GPIO_IR_OUT, 0);
	gpio_set_output_en(GPIO_IR_OUT, 1);
	gpio_set_data_strength(GPIO_IR_OUT, 1);
	gpio_write(GPIO_IR_OUT, 0);

	gpio_set_func(GPIO_IR_CONTROL, AS_GPIO);
	gpio_set_input_en(GPIO_IR_CONTROL, 0);
	gpio_set_output_en(GPIO_IR_CONTROL, 1);
	gpio_set_data_strength(GPIO_IR_CONTROL, 1);//驱动使能仅在GPIO配置为输出时才有效
	gpio_write(GPIO_IR_CONTROL, 0);//IRcontrol low ,enable IR learn func.

	gpio_set_func(GPIO_IR_LEARN_IN, AS_GPIO);
	gpio_set_input_en(GPIO_IR_LEARN_IN, 1);
	gpio_set_output_en(GPIO_IR_LEARN_IN, 0);
	gpio_setup_up_down_resistor(GPIO_IR_LEARN_IN, PM_PIN_PULLUP_10K);  //open pull up resistor
	gpio_set_interrupt_pol(GPIO_IR_LEARN_IN, pol_falling);    //falling edge

	reg_irq_src = FLD_IRQ_GPIO_EN; //clean irq status
	reg_irq_mask |= FLD_IRQ_GPIO_EN;
}

/***********************************************************************
* 【Function】ir_record(u32 tm) 				  			 			   *
* 【Description】IR learn algorithm ,need to put in ram code when 	   *
*               learning high frequency carrier wave.  				   *
* 【Parameter】tm:clock time now ,to count interval between 2 interrupt  *
***********************************************************************/
_attribute_ram_code_
static inline void ir_record(u32 tm)
{
	//1.add irq cnt
	++ g_ir_learn_ctrl->ir_enter_irq_cnt;		  //add irq counter
	g_ir_learn_ctrl->curr_trigger_tm_point = tm;  //record latest time
	g_ir_learn_ctrl->time_interval = (u32)(g_ir_learn_ctrl->curr_trigger_tm_point - g_ir_learn_ctrl->last_trigger_tm_point);  // count interval

	//2.first irq , do following things :
	// a>record irq time
	// b>change state to IR_LEARN_BEGIN
	if(g_ir_learn_ctrl->ir_enter_irq_cnt == 1 )
	{
		g_ir_learn_ctrl -> ir_learn_tick = tm;
		g_ir_learn_ctrl -> ir_learn_state = IR_LEARN_BEGIN;
	}

	//3.second irq , do following things if time interval is valid:
	//	a>set carrier or not symbol,stand for is recording carrier.
	//  b>record first interval
	//	c>record switch start point for later calculate
	else if(g_ir_learn_ctrl->ir_enter_irq_cnt == 2 )
	{
		//second come in
		//if first interval too long ,it fails.
		if(g_ir_learn_ctrl->time_interval > IR_LEARN_INTERVAL_THRESHOLD)
		{
			g_ir_learn_ctrl->ir_learn_state = IR_LEARN_FAIL_FIRST_INTERVAL_TOO_LONG;
		    return;
		}
		//if first interval is valid, IR learn process begin.
		else
		{
			g_ir_learn_ctrl->carr_or_not = 1;
			g_ir_learn_ctrl->carr_first_interval = g_ir_learn_ctrl->time_interval;
			g_ir_learn_ctrl->carr_switch_start_tm_point = g_ir_learn_ctrl->last_trigger_tm_point;
		}
	}

	//4.other irq
	else
	{
		// time interval less than IR_LEARN_INTERVAL_THRESHOLD, means it's normal carrier
		if(g_ir_learn_ctrl->time_interval < IR_LEARN_INTERVAL_THRESHOLD)
		{
			//if previews irq is carrier, symbol is 1. else here is 0 ,need to record in new buff.
			if(g_ir_learn_ctrl->carr_or_not == 0)
			{
				g_ir_learn_ctrl->wave_series_cnt ++;
				g_ir_learn_ctrl->carr_or_not = 1;
			}
			g_ir_learn_ctrl->wave_series_buf[g_ir_learn_ctrl->wave_series_cnt] = \
					(u32)(g_ir_learn_ctrl->curr_trigger_tm_point - g_ir_learn_ctrl->carr_switch_start_tm_point);
		}
		// this part means it's change from carrier to non_carrier
		else if( IR_LEARN_INTERVAL_THRESHOLD < g_ir_learn_ctrl->time_interval && \
				g_ir_learn_ctrl->time_interval < IR_LEARN_END_THRESHOLD)
		{
			DBG_CHN2_HIGH;
			if(g_ir_learn_ctrl->carr_or_not == 1)
			{
				g_ir_learn_ctrl->carr_or_not = 0;
				g_ir_learn_ctrl->carr_switch_start_tm_point = tm;
				g_ir_learn_ctrl->wave_series_buf[g_ir_learn_ctrl->wave_series_cnt] += g_ir_learn_ctrl->carr_first_interval;
				g_ir_learn_ctrl->wave_series_cnt ++;
				g_ir_learn_ctrl->wave_series_buf[g_ir_learn_ctrl->wave_series_cnt] = (u32)(g_ir_learn_ctrl->time_interval - g_ir_learn_ctrl->carr_first_interval/3);
			}
			else
			{
				g_ir_learn_ctrl->ir_learn_state = IR_LEARN_FAIL_TWO_LONG_NO_CARRIER;
				return;
			}
			DBG_CHN2_LOW;
		}
		//if next wave comes too late, we consider the process is over,the latest wave is useless.
		else if(g_ir_learn_ctrl->time_interval>IR_LEARN_END_THRESHOLD)
		{
			g_ir_learn_ctrl->ir_learn_state = IR_LEARN_SAMPLE_END;
		}

		//as long as the process begins,record first (IR_CARR_CHECK_CNT) interval and choose shortest as carrier.
		if(g_ir_learn_ctrl->carr_check_cnt < IR_CARR_CHECK_CNT)
		{
			if(g_ir_learn_ctrl->carr_cycle_interval < g_ir_learn_ctrl->time_interval)
			{
				g_ir_learn_ctrl->carr_cycle_interval = g_ir_learn_ctrl->time_interval;
			}
			++g_ir_learn_ctrl->carr_check_cnt;
		}

		if(g_ir_learn_ctrl->wave_series_cnt >= MAX_SECTION_NUMBER)
		{
			g_ir_learn_ctrl->ir_learn_state = IR_LEARN_FAIL_FLASH_FULL;
		}
	}

	//always record last trigger time
    g_ir_learn_ctrl->last_trigger_tm_point = g_ir_learn_ctrl->curr_trigger_tm_point;
}

/***********************************************************************
* 【Function】ir_learn_irq_handler(void)			  			 		   *
* 【Description】IR learn process in irq. 				  			   *
***********************************************************************/
_attribute_ram_code_
void ir_learn_irq_handler(void)
{
    reg_irq_src = IR_LEARN_INTERRUPT_MASK;
	if ((g_ir_learn_ctrl -> ir_learn_state != IR_LEARN_WAIT_KEY) && (g_ir_learn_ctrl -> ir_learn_state != IR_LEARN_BEGIN))
    {
    	return;
    }
	ir_record(clock_time());  // IR Learning
}

/***********************************************************************
* 【Function】ir_learn_send_init(void)				  			 	   *
* 【Description】 IR learn send init,set pwn and irq related.			   *
***********************************************************************/
void ir_learn_send_init(void)
{
	//only pwm0 support fifo mode
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ,16000000);

	pwm_n_revert(PWM0_ID);	//if use PWMx_N, user must set "pwm_n_revert" before gpio_set_func(pwmx_N).
	gpio_set_func(GPIO_IR_OUT, AS_PWM0_N);
	pwm_set_mode(PWM0_ID, PWM_IR_DMA_FIFO_MODE);
	pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning

	pwm_set_dma_address(&ir_send_dma_data);

	//add fifo stop irq, when all waveform send over, this irq will triggers
	//enable system irq PWM
	reg_irq_mask |= FLD_IRQ_SW_PWM_EN;

	//enable pwm0 fifo stop irq
	reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //clear irq status

	ir_send_ctrl.last_cmd = 0xff; //must
}

/***********************************************************************
* 【Function】ir_learn_start(void)					  			 	   *
* 【Description】 Begin IR learn process.			   					   *
***********************************************************************/
void ir_learn_start(void)
{
	memset(&ir_learn_ctrl,0,sizeof(ir_learn_ctrl));
	reg_irq_src = IR_LEARN_INTERRUPT_MASK;
	gpio_en_interrupt(GPIO_IR_LEARN_IN, 1);
	g_ir_learn_ctrl -> ir_learn_state = IR_LEARN_WAIT_KEY;
	g_ir_learn_ctrl -> ir_learn_tick = clock_time();
}

/***********************************************************************
* 【Function】ir_learn_start(void)					  			 	   *
* 【Description】 Stop IR learn process.			   					   *
***********************************************************************/
void ir_learn_stop(void)
{
    reg_irq_src = IR_LEARN_INTERRUPT_MASK;
	gpio_en_interrupt(GPIO_IR_LEARN_IN, 0);
}

/***********************************************************************
* 【Function】get_ir_learn_state(void)				  			 	   *
* 【Description】 Get ir learn state in UI layer.						   *
*  				0 	 : ir learn success								   *
* 				1 	 : ir learn is doing or disable					   *
* 				else : ir learn fail ,return fail reason,			   *
* 					   match enum ir_learn_states 	         		   *
***********************************************************************/
unsigned char get_ir_learn_state(void)
{
	if(g_ir_learn_ctrl -> ir_learn_state == IR_LEARN_SUCCESS)
		return 0;
	else if(g_ir_learn_ctrl -> ir_learn_state < IR_LEARN_SUCCESS)
		return 1;
	else
		return (g_ir_learn_ctrl -> ir_learn_state);
}

/***********************************************************************
* 【Function】ir_learn_copy_result(ir_learn_send_t* send_buffer)				  			 	   *
* 【Description】 Copy necessary parameter to send_buffer to 			   					   *
***********************************************************************/
void ir_learn_copy_result(ir_learn_send_t* send_buffer)
{
	ir_learn_send_t* g_ir_learn_send = send_buffer;
	g_ir_learn_send -> ir_learn_carrier_cycle = g_ir_learn_ctrl -> carr_cycle_interval;
	g_ir_learn_send -> ir_learn_wave_num = g_ir_learn_ctrl -> wave_series_cnt;
	for(u32 i=0;i<(g_ir_learn_ctrl -> wave_series_cnt)+1;i++)
	{
		g_ir_learn_send -> ir_lenrn_send_buf[i] = g_ir_learn_ctrl -> wave_series_buf[i];
	}
}

/***********************************************************************
* 【Function】ir_learn_detect(void)					  			 	   *
* 【Description】IR learn deal process,better to use it every loop	   *
***********************************************************************/
void ir_learn_detect(void)
{
	//ir learn overtime
	if( (g_ir_learn_ctrl -> ir_learn_state == IR_LEARN_WAIT_KEY) && clock_time_exceed(g_ir_learn_ctrl -> ir_learn_tick , IR_LEARN_OVERTIME_THRESHOLD) )	//10s overtime
	{
		g_ir_learn_ctrl -> ir_learn_state = IR_LEARN_FAIL_WAIT_OVER_TIME;
		ir_learn_stop();
	}
	//time beyond IR_LEARN_SAMPLE_END , check wave number to decide if success
	else if( ((g_ir_learn_ctrl -> ir_learn_state == IR_LEARN_BEGIN)&&(clock_time_exceed(g_ir_learn_ctrl -> curr_trigger_tm_point , IR_LEARN_END_THRESHOLD/16))) || \
		(g_ir_learn_ctrl -> ir_learn_state == IR_LEARN_SAMPLE_END) )
	{
		if(g_ir_learn_ctrl -> wave_series_cnt > CARR_AND_NO_CARR_MIN_NUMBER)
		{
			g_ir_learn_ctrl -> ir_learn_state = IR_LEARN_SUCCESS;
			g_ir_learn_ctrl -> wave_series_buf[g_ir_learn_ctrl -> wave_series_cnt-1] += g_ir_learn_ctrl -> carr_cycle_interval;	//add last carrier
			ir_learn_stop();
		}
		else
		{
			g_ir_learn_ctrl -> ir_learn_state = IR_LEARN_FAIL_WAVE_NUM_TOO_FEW;
			ir_learn_stop();
		}
	}
}

/***********************************************************************
* 【Function】ir_learn_send(void)					  			 	  	   *
* 【Description】Send IR code that be learned. 		   			       *
* *********************************************************************/
_attribute_ram_code_
void ir_learn_send(ir_learn_send_t* send_buffer)
{
    if (ir_sending_check())
    {
        return;
    }
    ir_learn_send_t* g_ir_learn_send = send_buffer;
    u32 carrier_cycle = g_ir_learn_send->ir_learn_carrier_cycle;
    u32 carrier_high = g_ir_learn_send->ir_learn_carrier_cycle / 3;

    ir_send_dma_data.data_num = 0;

    pwm_set_cycle_and_duty(PWM0_ID, carrier_cycle,  carrier_high ); 	//config carrier: 38k, 1/3 duty

    u8 is_carrier = 1;
    foreach (i,g_ir_learn_send->ir_learn_wave_num+1)
    {
    	ir_send_dma_data.data[ir_send_dma_data.data_num ++] = pwm_config_dma_fifo_waveform(is_carrier, PWM0_PULSE_NORMAL,g_ir_learn_send->ir_lenrn_send_buf[i]/carrier_cycle);
    	is_carrier = is_carrier == 1 ? 0:1;
    }

	ir_send_dma_data.dma_len = ir_send_dma_data.data_num * 2;

	ir_send_ctrl.repeat_enable = 0;  //need repeat signal

	reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
	reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask

	ir_send_ctrl.is_sending = IR_SENDING_DATA;

	ir_send_ctrl.sending_start_time = clock_time();

	pwm_start_dma_ir_sending();

}

#endif
