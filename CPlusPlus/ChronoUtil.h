// Kod'un kullanıldıgı yazı icin https://www.yazilimperver.com/index.php/2021/08/16/haftalik-c-39-stdchrono-2-clocks-time_point/ 
#include <chrono>
#include <ctime>
#include <string>
#include <iomanip>
#include <iostream>

// Bundan sonra kullanacağımız bazı faydalı fonksiyonları toplayacağımız sınıftır
class ChronoUtil {
public:
   // timepoint nesnelerimizi time_t tipine, onu da takvimsel saat gösterimine (ör. Mon May 23 13:44:00 2011), çevirmek için kullanacağımız fonksiyondurdur
    static std::string ToString (const std::chrono::system_clock::time_point& tp)    {
       // sistem zamanına dönüştürelim:
       std::time_t t = std::chrono::system_clock::to_time_t(tp);
       
       // takvimsel zamana dönüştürelim
       std::string ts = ctime(&t);   
       
       // bunun sonuna eklenen yeni satır karakterini silelim
       ts.resize(ts.size()-1);  
       return ts;
    }
    
    // Takvimsel zaman bilgilerinden, sistem saatinin time_point nesnesine çevirmek için kullanabileceğimi fonksiyondur
    static std::chrono::system_clock::time_point MakeTimePoint (int32_t year, int32_t mon, int32_t day,
                   int32_t hour, int32_t min, int32_t sec = 0) {
       struct std::tm t;
       t.tm_sec = sec;        // Saniye (0 .. 59)
       t.tm_min = min;        // Dakika (0 .. 59)
       t.tm_hour = hour;      // Saat (0 .. 23)
       t.tm_mday = day;       // Gün (0 .. 31)
       t.tm_mon = mon-1;      // Ay (0 .. 11)
       t.tm_year = year-1900; // 1900'den geçen sene
       t.tm_isdst = -1;       // Gün ışığını koruma var mı
       std::time_t tt = std::mktime(&t);
       
       if (tt == -1) {
           throw "Geçerli sistem zamanı yok!";
       }
       return std::chrono::system_clock::from_time_t(tt);
    }  
  
    // timepoint nesnelerimizi time_t tipine, onu da takvimsel saat gösterimine (ör. Mon May 23 13:44:00 2011), çevirmek için kullanacağımız fonksiyondurdur
    static std::string ToString (const std::chrono::system_clock::time_point& tp)   {
       // sistem zamanına dönüştürelim:
       std::time_t t = std::chrono::system_clock::to_time_t(tp);
       
       // takvimsel zamana dönüştürelim
       std::string ts = ctime(&t);   
       
       // bunun sonuna eklenen yeni satır karakterini silelim
       ts.resize(ts.size()-1);  
       return ts;
    }
  
  // Saati şablon parametresi olarak geçirelim
  template <typename T>
  static void PrintClockInfo(){ 
      std::cout << "  precision: " << T::num << "/" << T::den << " second " << "\n";
      typedef typename std::ratio_multiply<T,std::kilo>::type MillSec;
      typedef typename std::ratio_multiply<T,std::mega>::type MicroSec;
      std::cout << std::fixed;
      std::cout << "             " << static_cast<double>(MillSec::num)/MillSec::den << " milliseconds "   << "\n";
      std::cout << "             " << static_cast<double>(MicroSec::num)/MicroSec::den << " microseconds " << "\n";
  }
  
  static void PrintThreeClockTypeInfo(){
    std::cout << std::boolalpha << "\n";
    
    std::cout << "std::chrono::system_clock: " << "\n";
    std::cout << "  is steady: " << std::chrono::system_clock::is_steady << "\n";
    ChronoUtil::PrintClockInfo<std::chrono::system_clock::period>();
    
    std::cout << "\n";
    
    std::cout << "std::chrono::steady_clock: " << "\n";
    std::cout << "  is steady: " << std::chrono::steady_clock::is_steady << "\n";
    ChronoUtil::PrintClockInfo<std::chrono::steady_clock::period>();
    
    std::cout << "\n";
    
    std::cout << "std::chrono::high_resolution_clock: " << "\n";
    std::cout << "  is steady: " << std::chrono::high_resolution_clock::is_steady << "\n";
    ChronoUtil::PrintClockInfo<std::chrono::high_resolution_clock::period>();
  }
  
  // Sistem saatine iliskin bilgileri basalim
  static void PrintSystemClockInfo(){    
     // Sistem saatinin, başlangıç zamanına bakalım
     std::chrono::system_clock::time_point tp;
     std::cout << "epoch: " << ChronoUtil::ToString(tp) << "\n";

     // Sistem saatinin mevcut anına bakalım
     tp = std::chrono::system_clock::now();
     std::cout << "now:   " << ChronoUtil::ToString(tp) << "\n";

     // Sistem saati ile ifade edilebilecek minimum zaman anına bakalım
     tp = std::chrono::system_clock::time_point::min();
     std::cout << "min:   " << ChronoUtil::ToString(tp) << "\n";

     // Sistem saati ile ifade edilebilecek maksimum zaman anına bakalım
     tp = std::chrono::system_clock::time_point::max();
     std::cout << "max:   " << ChronoUtil::ToString(tp) << "\n";
  }
};
