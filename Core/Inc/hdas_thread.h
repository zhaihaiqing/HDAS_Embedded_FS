
#ifndef __HDAS_THREAD_H
#define __HDAS_THREAD_H



int hdas_thread_init(void);

void das_thread_entry(void *par);
void storage_thread_entry(void *par);
void order_thread_entry(void *par);
void key1_thread_entry(void *par);
void key2_thread_entry(void *par);
void wdi_thread_entry(void *par);
void led1_thread_entry(void *par);
void led2_thread_entry(void *par);

int hdas_test_thread_init(void);
void test_thread_entry(void *par);


extern struct rt_semaphore storage_sem;
extern uint8_t hdas_sample_flag;

//extern unsigned int start_key_count;


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
