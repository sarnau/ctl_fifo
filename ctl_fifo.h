/***
 *  A thread-safe fifo for n elements of a fixed size. Notification about an added element is done via an event
 *
 *  For the CTL Tasking Library from Rowley Associates Limited.
 *
 *  Written in 2019 by Markus Fritze, https://github.com/sarnau
 *
 *  This is free and unencumbered software released into the public domain.
 ***/

#ifndef __CTL_FIFO__
#define __CTL_FIFO__

#ifdef __cplusplus
extern "C" {
#endif

// Require CTL_TASKING to be defined to use the CTL API.
#if !defined(CTL_TASKING)
  #error The CTL API requires CTL_TASKING to be defined!
#endif

#include <ctl_api.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct CTL_FIFO_s {
  void *buffer;
  size_t writePos;
  size_t readPos;
  size_t elementSize;
  size_t elementCount;
  CTL_EVENT_SET_t *eventPtr;
  CTL_EVENT_SET_t event;
} CTL_FIFO_t;

void ctl_fifo_init(CTL_FIFO_t *fifo, void *buffer, size_t elementSize, size_t elementCount, CTL_EVENT_SET_t *eventPtr, CTL_EVENT_SET_t event);

bool ctl_fifo_add(CTL_FIFO_t *fifo, const void *element); // add a message at the beginning of the FIFO
const void *ctl_fifo_peek(CTL_FIFO_t *fifo);             // look at the message at the end of the FIFO
bool ctl_fifo_remove(CTL_FIFO_t *fifo);                   // remove the message at the end of the FIFO

size_t ctl_fifo_num_used(CTL_FIFO_t *fifo);     // number of messages in the FIFO
size_t ctl_fifo_capacity(CTL_FIFO_t *fifo);     // capacity of the FIFO in number of elements
size_t ctl_fifo_element_size(CTL_FIFO_t *fifo); // size of a single element in bytes

#ifdef __cplusplus
}
#endif

#endif