/**
 * @file   ActiveObject.h
 * @author Yazılımperver (yazilimperver@gmail.com)
 * @brief  Ornek bir aktif nesne sınıfımız. Detaylar için https://www.state-machine.com/doc/Sutter2010a.pdf
 * @version 0.1
 * @date    2021-12-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "UnboundedSharedQueue.h"
#include <functional>
#include <iostream>
#include <memory>

// Temel aktif nesnemiz
class ActiveObject
{
public:
    typedef std::function<void()> Message;

    ActiveObject() 
        : done(false) {
        thd = std::unique_ptr<std::thread>(
            new std::thread([=] { this->Run(); }));
    }

    /*! Destructor */
    ~ActiveObject() {
        Send([&]{ done = true; });

        std::cout << "Aktif nesne yokedilecek. Once butun islerin tamamlanmasini bekliyoruz!\n";
        thd->join();
        std::cout << "Aktif nesne yokedildi!\n";
    }

    /*! Aktif nesnenin disariya sunulan API'si */
    void Send(Message m) {
        mq.Push(m);
    }
private:
    /*! Constructor. Bu kullanilmayacak. */
    ActiveObject(const ActiveObject &){

    }

    /*! Atama operatoru. Bu kullanilmayacak */
    void operator=(const ActiveObject &){

    }

    /*! Gelen asenkron istekleri tutacagimiz kuyruk */
    CUnboundedSharedQueue<Message> mq;

    /*! Aktif nesneye iliskin thread */
    std::unique_ptr<std::thread> thd;
    
    void Run() {
        std::cout << "Aktif nesne yaratildi ve baslatildi. Mesajlari bekliyoruz!\n";
        Message msg;

        while (!done) {
            mq.Pop(msg);
            
            // Ilgili islevi gerceklestirelim
            msg(); 
        }          
    }

    /*! Isimiz bitti mi */
    bool done;
};


class Backgrounder {
public:
    void Save(std::string filename) {
        using namespace std::chrono_literals;
        mActiveObject.Send([=]{ 
            std::cout <<"Dosya <" << filename <<"> kayit isi basladi!\n";
            std::this_thread::sleep_for(2000ms);
            std::cout <<"Dosya  <" << filename <<"> kayit isi bitti!\n";
            } );
    }
    void Print(std::string data) {
        using namespace std::chrono_literals;
        mActiveObject.Send([=, &data]{ 
            std::cout <<"Ekrana bastirilacak olan mesaj: " << data << "\n";            
            std::this_thread::sleep_for(500ms);
            } );
    }

private:
    // Burada ayrica bu aktif nesneye spesifik veriler tutulabilir
    // ...

    // Aktif nesnemiz
    ActiveObject mActiveObject;
};

int main() {
    Backgrounder backActiveObject;
    backActiveObject.Save("BuyukDosya1.bin");
    backActiveObject.Save("BuyukDosya2.bin");
    backActiveObject.Print("Merhaba dostum!");
    std::cout <<"Normal sartlarda 4.5 sn sonra bunu gormeniz lazimdi!\n";
    std::cout <<"Ama uygulama kapanmadan butun islerin tamamlanmasini bekleyecegiz!\n";

    return 0;
}