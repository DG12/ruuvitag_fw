//DGG-init.c  2/12/20 start   (this is included before "wait for sensors,, app_sched_event_put(NULL, 0, main_sensor_task);
//  Regarding notifying the user:
//  blinking and NFC requies physcial proximaty to the tag                                                   (doesn't hurt)
//  log requires 1) nRF Dev hardware AND 2) LOG_ENABLE at compile time! (like in sdk_application_config.h)   (doesn't hurt)
//  transmiting configuration packet requires a receiver                                                     REQUIRED anyway
//  Configuration packet containing dateCompiled hh mm(implicit version),  (init)status and UICR configuration data
//      can be transmitted periodically 

// Blink version number (in case anyone is watching, also provides delay for log messages to catch up )
   char x;  //bit bucket for sscanf      v  . n  . p    in bluetooth_application_config.h its like "2.5.9"    02d ?   (_)
   int version[3]; sscanf(INIT_FWREV, "%1d%1c%1d%1c%02d", &version[0],&x,&version[1],&x, &version[2]);
   for( int i=0; i<3 ;i++ ){
                for ( int j=0; j< version[i]; j++ ) { GREEN_LED_ON; nrf_delay_ms(250);GREEN_LED_OFF; nrf_delay_ms(250); }
                nrf_delay_ms(500);
                        }                   // leading   \r causes    MAIN:INFO:  to be over writted ;-)
   NRF_LOG_INFO("\rFirmWareREVision:"  INIT_FWREV  ); // in text . Available from NFC  (in 2.5.7  causes reset)
   NRF_LOG_INFO("\r\ndateCompiled: %02X/%02X %02X:%02X \r\n", 
                                    compiledDate[3],compiledDate[2],compiledDate[1],compiledDate[0]); // %M%H%d%m 

// use UICR to establish tunables only if UICR_code matches first byte of UICR
#define UICR_code 3
    if (                     (UICR_code<<24 )        != (NRF_UICR->CUSTOMER[0] & 0xFF000000))// as seen with  mem32 10001080,1
       {NRF_LOG_WARNING( " -- UICR_code(%02Xxxxxxx)  !=  NRF_UICR(%08X). Tunables not applied \r\n\n",
                              UICR_code        ,         NRF_UICR->CUSTOMER[0]               );  }   // show the whole thing
    else {
      advertising_rates[RAWv2_FAST] = 100 * NRF_UICR->CUSTOMER[3] & 0x0000FFFF;// tenths of seconds  min .1sec; max 65.335sec=1min+ 
      main_loop_interval            = 100 * NRF_UICR->CUSTOMER[3] >> 16;                
      BME280t_adj                   =       NRF_UICR->CUSTOMER[4] >> 24;         //  -12.7C   ..  +12.7C      xxxxxxtt   int8
      BME280t_adj                   =  ( BME280t_adj > 0x7F ) ?    -.1 * (BME280t_adj &0x7F)  : .1 * BME280t_adj; 
      BME280h_adj                   =      (NRF_UICR->CUSTOMER[4] >> 16) & 0xFF; //   -12.7%  ..  +12.7%      xxxxhhxx   int8
      BME280h_adj                   =  ( BME280h_adj > 0x7F ) ?    -.1 * (BME280h_adj & 0x7F) : .1 * BME280h_adj; 

        }

// show values used:
    NRF_LOG_INFO("\r advertising_rates[RAWv2_FAST] = %d \r\n", advertising_rates[RAWv2_FAST] ); //                      600tenths  02 58
    NRF_LOG_INFO("\r main_loop_interval            = %d \r\n", main_loop_interval );// default= 1.280 x'05 00' ; 5sec=  500tenths  01 F4
    NRF_LOG_INFO("\r BME280 temp  adjust = %d (tenths)\r\n", BME280t_adj );
    NRF_LOG_INFO("\r BME280 humid adjust = %d (tenths)\r\n", BME280h_adj );



// Set up  config  packet format C0: init_statu, dateCompiled(_), tunableValues USED   L(data_buffer)=24
   data_buffer[0]  = 0xC0 ;  // configuration packet (5 other bits (E0 error, F0 )
   data_buffer[1]  = compiledDate[1];                       data_buffer[2]  = compiledDate[0]; // HH:MM 
   data_buffer[3]  = init_status  >>8;                      data_buffer[4]  = init_status  & 0xFF;   
   data_buffer[5]  = advertising_rates[RAWv2_FAST]>>8;      data_buffer[6]  = advertising_rates[RAWv2_FAST] & 0xFF;
   data_buffer[7]  = main_loop_interval >>8;                data_buffer[8]  = main_loop_interval  & 0xFF; 
   data_buffer[9]  = BME280t_adj;                           data_buffer[10] = BME280h_adj;
   data_buffer[11] = 0xD6;                                  data_buffer[12] = 0xD6;
   data_buffer[13] = 0xD6;                                  data_buffer[14] = 0xD6;
   data_buffer[15] = 0xD6;                                  data_buffer[16] = 0xD6;
   data_buffer[17] = 0xD6;

// put the MAC in the same place as a fmt5 packet for apple guys
   data_buffer[18] =((NRF_FICR->DEVICEADDR[1] >> 8)& 0xFF) | 0xC0;// force leading 0b11             ;
   data_buffer[19] =  NRF_FICR->DEVICEADDR[1]      & 0xFF;
   data_buffer[20] =  NRF_FICR->DEVICEADDR[0] >>24;
   data_buffer[21] = (NRF_FICR->DEVICEADDR[0] >>16)  & 0xFF;
   data_buffer[22] = (NRF_FICR->DEVICEADDR[0] >> 8)  & 0xFF; 
   data_buffer[23] =  NRF_FICR->DEVICEADDR[0]        & 0xFF;

   NRF_LOG_INFO("\rnRFC_FICR->DEVICEADDR: i.e. MAC=%02X:%02X:%02X:%02X:%02X:%02X\r\n", 
                                data_buffer[18],data_buffer[19],data_buffer[20],data_buffer[21],data_buffer[22],data_buffer[23]); 

//                   00 00 aa aa mm mm
   NRF_LOG_INFO("\rinit  adv   main  BME280\r\nstat  rate  loopi ta ha    (from C0 packet[3..] )                          ");
   NRF_LOG_HEXDUMP_INFO(&data_buffer[3],24-3); 


  bluetooth_set_manufacturer_data(data_buffer, sizeof(data_buffer));   
  bluetooth_advertising_start();                                    NRF_LOG_INFO("Advertising config \r\n");           // can this fail ?

        // To view packet with command line utilities:
        //  export bdaddr='zz yy xx ww vv uu' #  MAC address least significant byte FIRST  (high order bit forced on example  84 -> C4
        //  hcidump --raw |g  --line-buffered --after-context=2 $bdaddr
        //          04 3E 2B 02 01 00 01 C2 24 68 3C 10 C7 1F 02 01 06 03 03 AA
        //                                  zz yy xx ww vv uu
        //          FE 17 16 AA FE 10 F6 C0 vn mm ii_ss -- -- -- -- -- -- UICR -- -- -- --- -- --  
        // packet format C0 i.e. config  ^^       ^^^^^init status      

  nrf_delay_ms(400u); // keep sending init_stat for a while   like 11 packets

//DGG end
