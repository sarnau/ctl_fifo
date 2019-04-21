/***
 *  A thread-safe fifo for n elements of a fixed size. Notification about an added element is done via an event
 *
 *  For the CTL Tasking Library from Rowley Associates Limited.
 *
 *  Written in 2019 by Markus Fritze, https://github.com/sarnau
 *
 *  This is free and unencumbered software released into the public domain.
 ***/

#include "ctl_fifo.h"

void ctl_fifo_init(CTL_FIFO_t *fifo, void *buffer, size_t elementSize, size_t elementCount, CTL_EVENT_SET_t *eventPtr, CTL_EVENT_SET_t event) {
  fifo->buffer = buffer;
  fifo->elementSize = elementSize;
  fifo->elementCount = elementCount;
  fifo->eventPtr = eventPtr;
  fifo->event = event;

  fifo->writePos = 0;
  fifo->readPos = 0;
  ctl_events_init(fifo->eventPtr, 0);
}

size_t ctl_fifo_num_used(CTL_FIFO_t *fifo) {
  if (fifo->readPos > fifo->writePos) { // wrap around?
    return fifo->elementCount - (fifo->readPos - fifo->writePos);
  } else {
    return fifo->writePos - fifo->readPos;
  }
}

size_t ctl_fifo_capacity(CTL_FIFO_t *fifo) {
  return fifo->elementCount;
}

size_t ctl_fifo_element_size(CTL_FIFO_t *fifo) {
  return fifo->elementSize;
}

bool ctl_fifo_add(CTL_FIFO_t *fifo, const void *element) {
  int enabled = ctl_global_interrupts_disable();

  size_t wp = fifo->writePos + 1;
  if (wp >= fifo->elementCount)
    wp = 0;                  // wrap around
  if (wp == fifo->readPos) { // fifo full?
    if (enabled)
      ctl_global_interrupts_enable();
    return false; // do not accept this element
  } else {
    memcpy(fifo->writePos * fifo->elementSize + (uint8_t *)fifo->buffer, element, fifo->elementSize); // copy the element into the buffer
    fifo->writePos = wp;
    if (enabled)
      ctl_global_interrupts_enable();
    ctl_events_set_clear(fifo->eventPtr, fifo->event, 0); // notify tasks about it
    return true;
  }
}

bool ctl_fifo_remove(CTL_FIFO_t *fifo) {
  int enabled = ctl_global_interrupts_disable();
  if (fifo->readPos == fifo->writePos) { // fifo empty?
    if (enabled)
      ctl_global_interrupts_enable();
    return false; // nothing to remove
  }
  size_t rp = fifo->readPos + 1;
  if (rp >= fifo->elementCount)
    rp = 0; // wrap around
  fifo->readPos = rp;
  ctl_events_set_clear(fifo->eventPtr, 0, fifo->event); // notify tasks about it
  if (enabled)
    ctl_global_interrupts_enable();
  return true;
}

const void *ctl_fifo_peek(CTL_FIFO_t *fifo) {
  size_t index = fifo->readPos;
  if (index == fifo->writePos) // fifo empty?
    return NULL;
  return (uint8_t *)fifo->buffer + index * fifo->elementSize;
}