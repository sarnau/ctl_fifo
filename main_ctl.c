#include <ctl_api.h>
#include <stdint.h>
#include <string.h>
#if DEBUG
#include <__cross_studio_io.h>
#endif
#include "ctl_fifo.h"


static CTL_EVENT_SET_t gEvent;
static uint8_t buffer[16];
static CTL_FIFO_t gFifo;

void task_sender(void *p) {
  char cc = '0';
  while (1) {
    debug_printf("S %6d [%c]\n", ctl_get_current_time(), cc);
    ctl_fifo_add(&gFifo, &cc);
    cc++;
    if(cc > '9') cc = '0';
    ctl_timeout_wait(ctl_get_current_time() + 100);
   }
}

void task_receiver(void *p) {
  while (1) {
    unsigned ret = ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS, &gEvent, 1, CTL_TIMEOUT_DELAY, 10);
    if(!ret) {
      debug_printf("R %6d Timeout\n", ctl_get_current_time());
    } else {
      const uint8_t *ptr = (const uint8_t *)ctl_fifo_peek(&gFifo);
      debug_printf("R %6d %d [%c]\n", ctl_get_current_time(), ret, ptr[0]);
      ctl_fifo_remove(&gFifo);
    }
  }
}

void ctl_handle_error(CTL_ERROR_CODE_t e) {
  while (1)
    ;
}

int main(void) {
  ctl_board_init();
  static CTL_TASK_t main_task;
  ctl_task_init(&main_task, 255, "main");       // create subsequent tasks whilst running at the highest priority.
  ctl_start_timer(ctl_increment_tick_from_isr); // start the timer
                                                //  memset(new_task_stack, 0xcd, sizeof(new_task_stack));  // write known values into the stack
                                                //  new_task_stack[0]=new_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack

  ctl_fifo_init(&gFifo, buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), &gEvent, 1);

#define STACKSIZE 64
  static unsigned stack_sender[1 + STACKSIZE + 1];
  static CTL_TASK_t sender;
  ctl_task_run(&sender, 1, task_sender, 0, "task_sender", STACKSIZE, stack_sender + 1, 0);

  static unsigned stack_receiver[1 + STACKSIZE + 1];
  static CTL_TASK_t receiver;
  ctl_task_run(&receiver, 1, task_receiver, 0, "task_receiver", STACKSIZE, stack_receiver + 1, 0);
  ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
  unsigned int v = 0;
  while (1) {
    // power down can go here if supported
    v++;
  }
  return 0;
}