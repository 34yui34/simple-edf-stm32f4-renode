#ifndef UART1_LOGGER_H_
#define UART1_LOGGER_H_

#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void uart1_logger_init(void);

#endif /* UART1_LOGGER_H_ */