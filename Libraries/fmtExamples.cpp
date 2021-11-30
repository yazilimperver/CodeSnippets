// İlgili yazı için aşağıdaki sayfaya göz atabilirsinz:
// https://www.yazilimperver.com/index.php/2021/11/29/haftalik-c-43-fmt-kutuphanesi/

// Temel kullanım ornekleri
// Çıktı: "Simdi buraya basacagimiz sayi: 42."
fmt::print("Simdi buraya basacagimiz sayi: {}.", 42);

// Mevcut kullanımların, fmt'ye donusunce nasıl ortak olarak ifade edilebilecegi
printf("%d", my_int);
printf("%lld", my_long_long);
printf("%" PRIu64, my_int64);

// Yukarıdakiler yerine, aşağıdakilerini kullanabilirsiniz. Her biri için özel bir formatlama girdisine ihtiyacınız olmaz
fmt::format("{}", my_int);
fmt::format("{}", my_long_long);
fmt::format("{}", my_int64);

// Konumsal arguman ornekleri
// Çıktı: "Smdi bana ilk degil de ikinci argumani ver 12 sonra ilkini ver ilkArg. Sonra bir daha ikiniciyi 12 ver."
fmt::print("İmdi bana ilk degil de ikinci argumani ver {1} sonra ilkini ver {0}. Sonra bir daha ikiniciyi {1} ver.", "ilkArg", 12);

// İsimlendirilmiş Argümanlar
// Çıktı: [42] Doe, John
fmt::display("[{id}] {lastName}, {firstName}",
             fmt::arg("id", 42), fmt::arg("firstName", "John"),
             fmt::arg("lastName", "Doe"));
			 
// Ortaya karışık formatlama ornekleri
fmt::print("0X{:X}", 42); // "0X2A" basar, 16'lik gosterim
fmt::print("{:#X}", 42); // "0X2A" basar, 16'lik gosterim
fmt::print("{0:5}", 42); // "   42" basar, sabit genişlik
fmt::print("{2:{3}}", "gereksizArguman", "bu da oyle", 42, 7); // "     42" basar, dinamik (baska bır arguman kullanarak) genişlik
fmt::print("{0:.3}", 1.234); // "1.23" basar, çözünrülük
fmt::print("{2:.{3}}", "gereksizArguman", "bu da oyle", 1.234, 3); // "1.23" basar, dinamik (baska bir arguman kullanarak) çözünrülük
fmt::print("{:<10}", "left"); // "left      " basar
fmt::print("{:>10}", "right"); // "     right" basar
fmt::print("{:^10}", "centered"); // " centered " basar
fmt::print("{:-^14}", "centered"); // "---centered---" basar

// Mevcut printf formatlamalarının uyarlanması:
printf("%05.2f", 1.234); 	   // 01.23 basar
fmt::print("{:05.2f}", 1.234);  // 01.23 basar

// Bellege cikti ornegi
fmt::memory_buffer buf;
fmt::format_to(buf, "[{0}] {2}, {1}", 42, "John", "Doe"); // buf içerisine ilgili çıktıyı yerleştiriyor
std::cout << buf.data() << '\n';

// Kullanıcı tiplerinin formatlanması
struct Material
{
	int         Id;
    double      Weight;
    std::string Name;     
};

// Tip için hazırlanması gereken formatlayıcı
template<>
struct fmt::formatter<Material>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(Material const& material, FormatContext& ctx)
    {
    	return fmt::format_to(ctx.out(), "Name: {1} (Id:{0}). Weight: {2:^6.3f} gr", material.Id, material.Name, material.Weight);
    }
};

// Ornek kullanım
// Çıktısı: "Name: Gold (Id:102). Weight: 13.457 gr"
Material gold { 102, 13.456789, "Gold" };
fmt::print("{}\n", gold);

// Zaman Formatlama
std::time_t t = std::time(nullptr);
fmt::print("Today is {:%Y-%m-%d}\n", *std::localtime(&t)); // Today is 2021-11-23
std::time_t t = std::time(nullptr);
fmt::print("The date is {:%Y-%m-%d}.", fmt::localtime(t)); //"The date is 2021-11-23." (with the current date):

// Renklendirme
#include <fmt/color.h>

int main() {
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, Hello, {}!\n", "world");
  fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) | fmt::emphasis::underline, "Hello, {}!\n", "world");
  fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic, "Hello, {}!\n", "world");
}