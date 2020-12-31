// DG12-tunableItems.c is included at the end of the static variables.
// These are variables used instead of the figurative CONSTANTs.
// They are initalized to the defined figurative constatns and set to the UICR values in init  if UICR_code matches UICR
// Many commments here show where and what the origianal code was.

// #define DEFAULT_MODE RAWv2_FAST    aka mode 1
//
// REPLACE the line:
// static const uint16_t advertising_rates[] = {            // already not using the figurative CONSTANT
// with
// static       uint16_t advertising_rates[] = {   // not const as [1] may be updated from UICR in init
//
// from bluetooth_application_config.h:
// #define                         ADVERTISING_INTERVAL_RAW         1280u //!< Apple guidelines Specify at most and exactly 1285 ms x'05 00'
// #define                         ADVERTISING_INTERVAL_RAW_SLOW MAIN_LOOP_INTERVAL_RAW_SLOW
// advertising_rates[RAWv2_FAST] = ADVERTISING_INTERVAL_RAW;                                     // most requested now configurable        

// // just a note:
// #define APPLICATION_ADV_INTERVAL         ADVERTISING_INTERVAL_RAW ----unused----


// from bluetooth_application_config.h
// #define MAIN_LOOP_INTERVAL_RAW               1280u       //   x'05 00'
// #define MAIN_LOOP_INTERVAL_RAW_SLOW      ((5*1285)-5) // Apple maximum interval * 5
    static uint32_t main_loop_interval = MAIN_LOOP_INTERVAL_RAW ;                                                        // configurable 
// REPLACE in change_mode case  RAWv2_FAST: aka 1
//            app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_RAW, RUUVITAG_APP_TIMER_PRESCALER), NULL);
//            app_timer_start(main_timer_id, APP_TIMER_TICKS(main_loop_interval,     RUUVITAG_APP_TIMER_PRESCALER), NULL);
// REPLACE in main init                             change_mode is invoked via sched_event after getting it from flash or defaulted
//           if( init_timer(main_timer_id, APP_TIMER_MODE_REPEATED, MAIN_LOOP_INTERVAL_RAW, main_timer_handler) )
//           if( init_timer(main_timer_id, APP_TIMER_MODE_REPEATED, main_loop_interval,     main_timer_handler) )
//
//   4 differences (so far)
//
     static int32_t BME280t_adj = 0; // signed adjustment to temperature sensor (the difference may not be liner but better than nothing!)
     static int32_t BME280h_adj = 0; //                      humidity
