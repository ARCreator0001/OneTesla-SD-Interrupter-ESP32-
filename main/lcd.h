#ifndef __LCD_H
#define __LCD_H

void lcd_init();
void lcd_clear();
void lcd_setcursor(char col, char row);

// string/char/number print
void lcd_print(const char* s);
void lcd_print(char c);
void lcd_print(unsigned char c);
void lcd_print(int n);
void lcd_print(long n);
void lcd_print(unsigned int n);
void lcd_print(unsigned long n);

// helpers
void lcd_printhome(const char* s);
void lcd_printat(char col, char row, const char* s);
void lcd_printat(char col, char row, char c);
void lcd_printat(char col, char row, unsigned long n);
void lcd_printat(char col, char row, unsigned int n);
void lcd_printat(char col, char row, char clr, const char* s);
void lcd_printat(char col, char row, char clr, char c);
void lcd_printat(char col, char row, char clr, unsigned long n);
void lcd_printat(char col, char row, char clr, unsigned int n);
void lcd_clrat(char col, char row, char clr);

#endif
