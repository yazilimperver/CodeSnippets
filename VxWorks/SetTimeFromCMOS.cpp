/**
 * @file    SetTimeFromCMOS.cpp
 * @author  Yazılımperver
 * @brief   VxWorks icin CMOS'dan verileri okuyup tarihi ve saati ayarlamak icin kullanilabilecek kod
 *          İlgili yazi: https://www.yazilimperver.com/index.php/2021/11/12/haftalik-c-42-vxworks-icin-saat-guncelleme/
 * @version 0.1
 * @date    2021-11-10 
 */

#include "vxWorks.h"
#include "time.h"
#include "stdio.h"
#include "memLib.h"
#include "sysLib.h"

STATUS SetTimeFromCMOS (void)
{
    int i;
    time_t     time1;
    struct tm* time2;

    unsigned int month_array[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned int current_second_bcd;
    unsigned int current_minute_bcd;
    unsigned int current_hour_bcd;
    unsigned int current_day_of_week_bcd;
    unsigned int current_date_bcd;
    unsigned int current_month_bcd;
    unsigned int current_year_bcd;
    unsigned int current_century_bcd;
    unsigned int second;
    unsigned int minute;
    unsigned int hour;
    unsigned int day;
    unsigned int date;
    unsigned int month;
    unsigned int year;
    unsigned int yday;
    unsigned int century;
    struct timespec timespecToUpdate;

    // Ilklendirilmemis zaman bilgisini basalim = THU JAN 01 00:00:01 1970
    time1 = time (0);
    time2 = localtime (&time1);

    printf ("Current vxWorks Time = %s", asctime (time2));

    // CMOS RAM'den gerçek zamanlı saat bılgısını okuyalım
    // Bu veriler 0x70 ve 0x71 adreslerinden okunacak
    // Detaylar için https://wiki.osdev.org/CMOS, http://ics.p.lodz.pl/~mceg/telekom/RTC%20-%20Ports.pdf
    sysOutByte (0x70, 0x0);
    current_second_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x2);
    current_minute_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x4);
    current_hour_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x6);
    current_day_of_week_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x7);
    current_date_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x8);
    current_month_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x9);
    current_year_bcd = sysInByte (0x71);
    sysOutByte (0x70, 0x32);
    current_century_bcd = sysInByte (0x71);

    // Gerçek zamanlı BCD saat verisini basalım
    printf ("Second = %02X\nMinute = %02X\nHour = %02X\nDay = %02X \nDate = %02X\n", current_second_bcd, current_minute_bcd, current_hour_bcd, 
            current_day_of_week_bcd, current_date_bcd);
    printf ("Month = %02X \nYear = %02X \nCentury = %02X", current_month_bcd,
            current_year_bcd, current_century_bcd);
    
    // BCD saat verilerini VxWorks'e aktarmak için gerekli donusumleri yapalim
    // Detaylar için /WIND_BASE/target/h/time.h
    second = ((current_second_bcd & 0xF0) >> 4) * 10;
    second = second + (current_second_bcd & 0x0F);

    minute = ((current_minute_bcd & 0xF0) >> 4) * 10;
    minute = minute + (current_minute_bcd & 0x0F);

    hour = ((current_hour_bcd & 0xF0) >> 4) * 10;
    hour = hour + (current_hour_bcd & 0x0F);

    day = ((current_day_of_week_bcd & 0xF0) >> 4) * 10;
    day = day + (current_day_of_week_bcd & 0x0F);

    date = ((current_date_bcd & 0xF0) >> 4) * 10;
    date = date + (current_date_bcd & 0x0F);

    month = ((current_month_bcd & 0xF0) >> 4) * 10;
    month = month + (current_month_bcd & 0x0F);

    year = ((current_year_bcd & 0xF0) >> 4) * 10;
    year = year + (current_year_bcd & 0x0F);

    century = ((current_century_bcd & 0xF0) >> 4) * 10;
    century = century + (current_century_bcd & 0x0F);
    century = century * 100;

    year = century + year;
    year = year - 1900;

    for (i = 0; i < month; i++)
        yday = yday + month_array[i];
    yday = yday + date;

    // Dönüştürülen verileri basalım
    printf ("Converted: Second = %d minute = %d hour = %d ",
        second, minute, hour);
    printf ("Converted: Date = %d month = %d year = %d day = %d yday = %d", date, month, year, day, yday);
    
    // tm veri yapısını dönüştürülen veriler ile dolduralım
    time2->tm_sec = second;
    time2->tm_min = minute;
    time2->tm_hour = hour;
    time2->tm_mday = date;
    
    // Ay verileri 0-11 aralığı
    time2->tm_mon = month-1;  
    time2->tm_year = year;
    time2->tm_wday = day;
    time2->tm_yday = yday;
    time2->tm_isdst = 1;     

    // İlgili veri yapısını saniyeye çevirelim
    time1 = mktime (time2);

    // VxWorks zamanını yeni zaman ile güncelleyelim
    timespecToUpdate.tv_sec = time1;
    timespecToUpdate.tv_nsec = 0;

    // İlgili gerçek zamanlı saati güncelleyelim
    clock_settime(CLOCK_REALTIME, &timespecToUpdate);

    // Güncellenen zamanı tekrar okuyalım ve güncellendiğini kontrol edelim
    time1 = time(0);
    time2 = localtime(&time1);

    printf("New vxWorks Time = %s", asctime(time2));

    return OK;
}

int main()
{
    SetTimeFromCMOS();
    return 0;
}
