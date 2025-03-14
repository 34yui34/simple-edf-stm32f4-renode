#ifndef TIMER2_TICK_INT_H
#define TIMER2_TICK_INT_H

void timer2_tick_init(void);
void timer2_set_tick_callback(void (*tick_cb)(void));

#endif /* TIMER2_TICK_INT_H */