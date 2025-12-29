# 🐱 SushiSim – Sanal Kedi Simülasyonu

**SushiSim**, C programlama dili ve SDL2 kütüphanesi kullanılarak geliştirilmiş, pixel art tarzında interaktif bir **sanal evcil hayvan (Virtual Pet)** oyunudur.

Oyuncu; **Sushi** isimli kedinin açlık, mutluluk ve enerji ihtiyaçlarını giderirken, aynı zamanda çevresel temizliği sağlayarak en yüksek skora ulaşmaya çalışır.

---

## 🎮 Oyun Özellikleri

- 🐾 **Pixel Art Karakter:** Animasyonlu ve yön değiştirebilen kedi karakteri.
- 🖱️ **Etkileşimli Arayüz:** Mouse ile kontrol edilebilen butonlar ve durum çubukları (Health Bars).
- 🧠 **Detaylı İstatistik Sistemi:**
  - **Açlık (Hunger):** Zamanla artar, mama ile azalır.
  - **Mutluluk (Happiness):** İlgi gösterilmezse veya ortam kirliyse düşer.
  - **Enerji (Energy):** Hareket ettikçe azalır, uyuyunca dolar.
- 💩 **Kirlilik Mekaniği:** Kedi rastgele zamanlarda odayı kirletebilir (Kaka yapma ihtimali). Temizlenmezse mutluluk hızla düşer.
- ✨ **Partikül Efektleri:** Sevgi gösterildiğinde (oynama/besleme) ekranda beliren kalp efektleri.
- 💀 **Game Over & Restart:** İstatistikler tükendiğinde oyun biter, tek tuşla yeniden başlar.

---

## 🕹️ Kontroller

Oyun hem **Mouse** hem de **Klavye** ile tam kontrol imkanı sunar.

### 🖱️ Mouse Kontrolleri
- **BASLA** → Oyunu başlatır.
- **MAMA** → Açlığı giderir (+Mutluluk).
- **OYNA** → Mutluluğu artırır (-Enerji, -Tokluk).
- **UYU** → Enerjiyi doldurur (-Tokluk).
- **TEMİZLE** → Yerdeki kirliliği temizler (+Mutluluk).
- **TEKRAR** → Game Over ekranında oyunu sıfırlar.

### ⌨️ Klavye Kısayolları
**Hareket:**
- `W`, `A`, `S`, `D` veya `Yön Tuşları` → Kediyi oda içinde gezdirir.

**Eylemler:**
- `F` → Mama Ver (Feed)
- `P` → Oyna (Play)
- `R` → Uyu/Dinlen (Rest)
- `ESC` → Çıkış
- `Enter` → Yeniden Başlat (Restart)

---

## 🧩 Kullanılan Teknolojiler

Bu proje, oyun programlama temellerini kavramak amacıyla aşağıdaki teknolojilerle geliştirilmiştir:

- **Dil:** C (Standard C99/C11)
- **Grafik Motoru:** SDL2 (Simple DirectMedia Layer)
- **Eklentiler:** - `SDL2_image` (PNG yönetimi için)
  - `SDL2_ttf` (Yazı tipi renderlama için)
- **Ortam:** MSYS2 (MinGW-w64 UCRT64)
- **Derleyici:** GCC

---

## 🗂️ Proje Yapısı

Modüler bir kod yapısı kullanılmıştır:

SushiSim/
├── src/
│   ├── main.c           # Giriş noktası ve oyun döngüsü
│   ├── game.c / .h      # Oyun mantığı, olaylar ve state yönetimi
│   ├── player.c / .h    # Karakter hareketi ve animasyon işlemleri
│   ├── stats.c / .h     # Matematiksel hesaplamalar (Puan, istatistikler)
│   └── render.c / .h    # Çizim fonksiyonları (Text, Bar, Texture)
├── assets/
│   ├── sushi_sheet.png  # Sprite atlas dosyası
│   └── font.ttf         # Oyun fontu
├── README.md            # Proje dokümantasyonu
└── .gitignore

---

## ⚙️ Derleme ve Çalıştırma (MSYS2)

Projenin Windows üzerinde MSYS2 ortamında derlenmesi için adımlar:

### 1. Gerekli Paketlerin Kurulumu
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-SDL2 \
          mingw-w64-ucrt-x86_64-SDL2_image \
          mingw-w64-ucrt-x86_64-SDL2_ttf

2. Derleme (Compile)
Proje dizininde terminali açın ve şu komutu girin:

Bash

gcc src/*.c -o SushiSim.exe `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

3. Çalıştırma
Bash

./SushiSim.exe
Not: assets klasörü ve oluşturulan .exe dosyası aynı dizinde bulunmalıdır. Ayrıca gerekli .dll dosyalarının yolda (path) olduğundan emin olun.

🎨 Kaynakça ve Lisanslar
Grafik Varlıkları:

Oyunda kullanılan kedi sprite'ları: "Kittens Cats Character – 32x32 Pixel Animation Pack"

Kaynak: itch.io - last-tick

Bu varlıklar, eğitim amaçlı projede lisans koşullarına uygun şekilde kullanılmıştır.

🎯 Proje Amacı
Bu proje;

Oyun Döngüsü (Game Loop) mantığını kurmak,

Event Handling (Klavye/Mouse girdileri) yönetimini öğrenmek,

State Machine (Başlangıç -> Oyun -> Bitiş) yapısını kavramak,

Modüler C Programlama yeteneklerini geliştirmek amacıyla yapılmıştır.

👩‍💻 Geliştirici
Nezaket Demirci 
Bilgisayar Programcılığı Öğrencisi