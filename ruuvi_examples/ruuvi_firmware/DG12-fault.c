// DG12-fault  this gets included after main.  app_error_fault_handler was defined as weak so this can be used instead
// LOG_ERROR because maybe the SDK in connected and maybe LOG is enabled
// Maybe user can see blink. A delay minimizes fast looping
// Try to send out F0 packet
// This is triggered  with j-link after halt then go due to WatchDog Timer runout 
//          Fatal. ID:00000001, PC:00011314, INFO:00000000, FaultCounter: 1 +++++++++++
// does not come here on IPSR = 003 (HardFaultMemManage)

static uint8_t faultCounter =0;  // Hope  we didn't reset and reinit this. Maybe keep it in UICR?

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{ 
    watchdog_feed(); // dont want to fault 
    if ( faultCounter++ == 0) faultCounter=1; // in case wraps never send 00
    NRF_LOG_ERROR("\r\n\r\n    Fatal. ID:%08x, PC:%08x, INFO:%08x, FaultCounter: %d +++++++++++\r\n\r\n",id,pc,info,faultCounter);
    NRF_LOG_FINAL_FLUSH();
  
    for ( int16_t i=0; i<13; i++) { RED_LED_ON; nrf_delay_ms(500u); RED_LED_OFF; nrf_delay_ms(500u); }
  
    watchdog_feed();

// seems that advertising has stopped 
//                                                          CANNOT seem to start it again. maybe stack is crapped
// set up  Fault  packet format F0: id
    data_buffer[0]  =   0xF0 ;  
    data_buffer[1]  =   faultCounter;  
    data_buffer[2]  =   id >>24;
    data_buffer[3]  =   id >>16 & 0xFF;
    data_buffer[4]  =   id >>8  & 0xFF;
    data_buffer[5]  =   id      & 0xFF ;

//stall for a while maybe packets will go out
    nrf_delay_ms(500u);

// Maybe the battery or temperature will be better now

         NVIC_SystemReset(); // well if nothing else the packetCounter will suddendly be reset rather than wrapping at 65535
// cannot just return
}
