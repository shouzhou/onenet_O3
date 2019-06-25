/**
 * Copyright (c) 2017 China Mobile IOT.
 * All rights reserved.
**/

// �ó��� ��onenet������ �������޸� ���������Լ���RCT6���������У�ģ���ϴ���ʪ�� �Լ�����onenet����led ����



#include "platform.h"


nbiot_value_t temp;   /* �¶� */
nbiot_value_t humi;   /* ʪ�� */
nbiot_value_t illumi; //����

nbiot_value_t LED;

//uint16_t TempDHT,HumDHT;
DHT11_T  g_DHT11;
uint16_t sysruntime=0;
void write_callback( uint16_t       objid,
                     uint16_t       instid,
                     uint16_t       resid,
                     nbiot_value_t *data )
{
    printf( "write /%d/%d/%d��%d\r\n",
                  objid,
                  instid,
                  resid,data->value.as_bool );
    if(objid==3311&&instid==0&&resid==5850)
			 Led1_Set(data->value.as_bool);

}

void read_callback( uint16_t       objid,
                     uint16_t       instid,
                     uint16_t       resid,
                     nbiot_value_t *data )
{
       
		   
        if(objid==3301&&instid==0&&resid==5700){		    
                //  BH1750_test();				 
				  illumi.value.as_float=300.0;
		}else if(objid==3303&&instid==0&&resid==5700){
                //  DISABLE_INT();
                  DHT11_ReadData(&g_DHT11);
            	 // DHT22_Read_Data(&TempDHT,&HumDHT);
                 // ENABLE_INT();
              //   printf("temp= %d  humi =  %d \r\n",g_DHT11.Temp, g_DHT11.Hum);
			    //  SHT20_INFO sht20={0};					 
                //  sht20=SHT20_GetValue();
				  temp.value.as_float=(float)g_DHT11.Temp/10.0;
             //temp.value.as_float=(float)TempDHT/10.0;
		
		}else if(objid==3304&&instid==0&&resid==5700){
			    //  SHT20_INFO sht20={0};				 
                //  sht20=SHT20_GetValue();
                  DHT11_ReadData(&g_DHT11);
				  humi.value.as_float=(float)g_DHT11.Hum/10.0;
            //  humi.value.as_float=(float)HumDHT/10.0;
			
    }
	
}

void execute_callback( uint16_t       objid,
                       uint16_t       instid,
                       uint16_t       resid,
                       nbiot_value_t *data,
                       const void    *buff,
                       size_t         size )
{
    printf( "execute /%d/%d/%d\r\n",
                  objid,
                  instid,
                  resid );
}

#ifdef NOTIFY_ACK
void notify_ack_callback( uint16_t       objid,
                          uint16_t       instid,
                          uint16_t       resid,
                          nbiot_value_t *data,
                          bool           ack )
{
    printf( "notify ack /%x/%x/%x(%s)\r\n",
                  objid,
                  instid,
                  resid,
                  ack ? "true":"false" );
}
#endif

char uri[] = "coap://183.230.40.40:5683";
nbiot_device_t *dev = NULL;


time_t last_time=0;
time_t cur_time=0;


void res_update(time_t interval)
{
      if((cur_time%10 ==0)&&(cur_time<100))
      {
           tim_print_result(); 
      }
	    
       if(cur_time>=last_time+interval){  //���ʱ�䵽
            cur_time=0;
            last_time=0;				 
            sysruntime++;
		    temp.flag |= NBIOT_UPDATED;		  //������Ҫ����
            humi.flag |= NBIOT_UPDATED;					 
            DHT11_ReadData(&g_DHT11);  //��ȡ��ʪ��

            temp.value.as_float=(float)g_DHT11.Temp/10.0;  //ת��Ϊ������
			humi.value.as_float=(float)g_DHT11.Hum/10.0;
           
            bsp_Diwen_Updatedata(0x0003,g_DHT11.Temp);
            bsp_Diwen_Updatedata(0x0004,g_DHT11.Hum);
            bsp_Diwen_Updatedata(0x0005,300);
            bsp_Diwen_Updatedata(0x0006,sysruntime/6);
       
		    illumi.flag |= NBIOT_UPDATED;
     	    illumi.value.as_float=300.0;
		
			}else if(cur_time==0&&last_time==0){
			
			    cur_time=nbiot_time();
			    last_time=cur_time;
			
			}else{
			
			   cur_time=nbiot_time();
            //   printf("cur_time = %d \r\n",(uint16_t)cur_time);
			   
			} 	 

}	
int main( int argc, char *argv[])
{
       int life_time = 1000;
	   int ret;
       nbiot_init_environment( argc, argv );  

 #ifdef NOTIFY_ACK
        ret = nbiot_device_create( &dev,
                                   life_time,
                                   write_callback,
                                   read_callback,
                                   execute_callback,
                                   notify_ack_callback );
#else
        ret = nbiot_device_create( &dev,
                                   life_time,
                                   write_callback,
                                   read_callback,
                                   execute_callback );
#endif
       
        if ( ret )
        {
            nbiot_device_destroy( dev );
            printf( "device add resource(/3200/0/5750) failed, code = %d.\r\n", ret );
        }
		LED.type = NBIOT_BOOLEAN;
        LED.flag = NBIOT_READABLE|NBIOT_WRITABLE;
        ret = nbiot_resource_add( dev,
                                  3311,
                                  0,
                                  5850,
                                  &LED );//�ƿ���
        if ( ret )
        {
            nbiot_device_destroy( dev );
            printf( "device add resource(LED) failed, code = %d.\r\n", ret );
        }
				
        temp.type = NBIOT_FLOAT;
        temp.flag = NBIOT_READABLE;
        ret = nbiot_resource_add( dev,
                                  3303,
                                  0,
                                  5700,
                                  &temp );//�¶�
        if ( ret )
        {
            nbiot_device_destroy( dev );
            printf( "device add resource(temp) failed, code = %d.\r\n", ret );
        }
				
				
        humi.type = NBIOT_FLOAT;
        humi.flag = NBIOT_READABLE;
        ret = nbiot_resource_add( dev,
                                  3304,
                                  0,
                                  5700,
                                  &humi );//ʪ��
        if ( ret )
        {
            nbiot_device_destroy( dev );
            printf( "device add resource(humi) failed, code = %d.\r\n", ret );
        }

        illumi.type = NBIOT_FLOAT;
        illumi.flag = NBIOT_READABLE;
        ret = nbiot_resource_add( dev,
                                  3301,
                                  0,
                                  5700,
                                  &illumi );//����
        if ( ret )
        {
            nbiot_device_destroy( dev );
            printf( "device add resource(illumi) failed, code = %d.\r\n", ret );
        }

	    nbiot_object_add(dev);
        ret = nbiot_device_connect(dev,100);

        if ( ret )
        {
            printf( "connect OneNET failed.\r\n" );
			      nbiot_reset();
        }else{
	        printf( "connect OneNET success.\r\n" );
			  }
         
    while(1)
    {
             ret = nbiot_device_step( dev, 1);
             if ( ret )
             {
               printf( "device step error, code = %d.\r\n", ret );
				      // Led4_Set(LED_OFF);
               printf( "connect server failed.\r\n" );
					 nbiot_reset();
             }else{ 
                    res_update(20);	
             }	
             HC_Analyze();
            

			      
    } 
    nbiot_clear_environment();
 
    printf( "press enter key to exit..." );

    return 0;
}
