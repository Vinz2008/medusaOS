#include "kernel/rtc.h"
#include <types.h>
#include <stdio.h>
#include <kernel/io.h>
#include <kernel/nmi.h>
#include <kernel/kmalloc.h>


#define CURRENT_YEAR 2023

uint16_t century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;

int get_update_in_progress_flag() {
      outb(CMOS_ADRESS, 0x0A);
      return (inb(CMOS_DATA) & 0x80);
}
 
uint8_t read_RTC_register(uint16_t reg) {
      outb(CMOS_ADRESS, reg);
      return inb(CMOS_DATA);
}
 

uint8_t read_seconds_RTC(){
      return read_RTC_register(0x00);
}

uint8_t read_minutes_RTC(){
      return read_RTC_register(0x02);
}

uint8_t read_hours_RTC(){
      return read_RTC_register(0x04);
}

uint8_t read_days_RTC(){
      return read_RTC_register(0x07);
}

uint8_t read_months_RTC(){
      return read_RTC_register(0x08);
}

uint8_t read_years_RTC(){
      return read_RTC_register(0x09);
}

char* read_rtc_date(){
      uint8_t century;
      uint8_t last_second;
      uint8_t last_minute;
      uint8_t last_hour;
      uint8_t last_day;
      uint8_t last_month;
      uint8_t last_year;
      uint8_t last_century;
      uint8_t registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = read_seconds_RTC();
      minute = read_minutes_RTC();
      hour = read_hours_RTC();
      day = read_days_RTC();
      month = read_months_RTC();
      year = read_years_RTC();
      if(century_register != 0) {
            century = read_RTC_register(century_register);
      }
 
      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;
 
            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = read_seconds_RTC();
            minute = read_minutes_RTC();
            hour = read_hours_RTC();
            day = read_days_RTC();
            month = read_months_RTC();
            year = read_years_RTC();
            if(century_register != 0) {
                  century = read_RTC_register(century_register);
            }
      } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );
 
      registerB = read_RTC_register(0x0B);
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }
 
      // Convert 12 hour clock to 24 hour clock if necessary
 
      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
      char* str = kmalloc(150 * sizeof(char));
      sprintf(str, "%d:%d:%d %d/%d/%d",hour, minute, second, day, month, year);
      return str;
      //printf("%d:%d:%d %d/%d/%d\n",hour, minute, second, day, month, year);
}

