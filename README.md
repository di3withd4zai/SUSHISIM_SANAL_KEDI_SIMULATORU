# 🐱 SushiSim – Sanal Kedi Simülasyonu (C + SDL2)

<div align="center">

**C programlama dili ve SDL2 ile geliştirilen, pixel art tarzında interaktif sanal evcil hayvan oyunu.**  
Kedinin ihtiyaçlarını yönet, doğru zamanda aksiyon al, Sushi’yi mutlu tut 🖤

</div>

---

## 📌 Proje Özeti

**SushiSim**, oyuncunun **Sushi** adlı kedinin temel ihtiyaçlarını yönettiği gerçek zamanlı bir mini simülasyon oyunudur.  
Oyuncu; **Açlık, Mutluluk ve Enerji** değerlerini dengede tutarak oyunu sürdürmeye çalışır.

**Amaç:** Statları yöneterek oyunda mümkün olduğunca uzun süre hayatta kalmak.

---

## ✨ Güncel Oyun Özellikleri

- 🐾 **Pixel Art Kedi:** Idle / Uyuma / Oynama animasyonları
- 📊 **HUD Stat Barları:** Açlık – Mutluluk – Enerji (ekranda bar olarak)
- 🖱️ **Mouse ile Buton Kontrolü:**  
  - **MAMA** → mama kabına gider, yeme efekti uygulanır  
  - **OYNA** → oyuncağa gider, **Box3 (kutuda oynama)** animasyonuna geçer  
  - **UYU** → yatağa gider, uyuma animasyonu oynar  
- ⏸️ **Duraklat / Devam / Yeniden Başla:** Pause menüsü ile kontrol
- 💀 **Game Over:** Tüm statlar sıfırlanınca oyun biter, yeniden başlanabilir
- 💥 **Eylem Efektleri:** Mama sırasında kedi titreme/bounce efekti ile “yeme” hissi verir

> Not: Son sürümde **kaka/temizlik mekaniği yoktur**. Kedi hareketi de **manuel değil**, aksiyonlara göre otomatik gerçekleşir.

---

## 🕹️ Kontroller

### 🖱️ Mouse
- **BASLA** → Oyunu başlatır
- **MAMA** → Besle (kedi mama kabına gider)
- **OYNA** → Oyna (kedi oyuncağa gider, Box3 animasyonu başlar)
- **UYU** → Uyu (kedi yatağa gider, uyku animasyonu başlar)
- **DURAKLAT** → Oyunu duraklatır
- **DEVAM** → Oyuna geri döner
- **YENİDEN BAŞLA** → Oyunu sıfırlar

### ⌨️ Klavye (Opsiyonel)
- `ESC` → Çıkış
- `Enter` → (Game Over / Pause ekranında) yeniden başlatma/menü geçişleri (varsa)

---

## 🧠 Oyun Mantığı (Kısa)

- Statlar zamanla azalır.
- Oyuncu doğru zamanda aksiyon seçer:
  - **MAMA** açlığı dengeler
  - **UYU** enerjiyi toparlar
  - **OYNA** mutluluğu yükseltir
- Tüm statlar **0** olursa: **Game Over**

---

## 🧩 Kullanılan Teknolojiler

- **Dil:** C (C99/C11)
- **Kütüphane:** SDL2
- **Ek Paketler:** SDL2_image (PNG), SDL2_ttf (yazı/font)
- **Derleme Ortamı:** MSYS2 (UCRT64) + GCC (MinGW-w64)

---

## ⚙️ Kurulum ve Derleme (MSYS2 – UCRT64)

### 1) Gerekli Paketler
bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-SDL2 \
          mingw-w64-ucrt-x86_64-SDL2_image \
          mingw-w64-ucrt-x86_64-SDL2_ttf

---

### 2) Derleme
Proje klasöründe:

bash

gcc src/*.c -o SushiSim.exe `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

---

### 3) Çalıştırma

bash

./SushiSim.exe

---

⚠️ Önemli: assets/ klasörü, .exe ile aynı dizinde bulunmalıdır.
Aksi halde texture/font yüklenemez.

---

### 🗂️ Proje Yapısı

```text
SushiSim/
├── src/
│   ├── main.c        # Entry point ve ana döngü
│   ├── game.c / .h   # State yönetimi (Start / Play / Pause / GameOver)
│   ├── player.c / .h # Kedi animasyonları ve hedefe yürüyüş mantığı
│   ├── stats.c / .h  # Stat güncellemeleri ve aksiyon etkileri
│   └── render.c / .h # HUD, butonlar, sprite çizimleri
├── assets/
│   ├── *.png         # Oyun görselleri (kedi, UI, eşyalar)
│   └── font.ttf      # Yazı fontu
└── README.md

```
---

### 🎨 Kaynakça / Asset ve Araç Kullanımı

Bu proje geliştirilirken aşağıdaki kaynaklardan ve araçlardan yararlanılmıştır:

🎮 Grafik & UI Varlıkları

ToffeeCraft – Cat User Interface
https://toffeecraft.itch.io/cat-user-interface

ToffeeCraft – Cat Pack
https://toffeecraft.itch.io/cat-pack

ToffeeCraft – UI Pack
https://toffeecraft.itch.io/ui

ToffeeCraft – Cozy Coffee UI Pack
https://toffeecraft.itch.io/ui-user-interface-pack-cozy-coffee

ToffeeCraft – Cat Mega Bundle
https://toffeecraft.itch.io/cat-mega-bundle

Kullanılan tüm grafikler, ilgili lisans koşullarına uygun şekilde eğitim amaçlı projede kullanılmıştır.

---

### 🤖 Yapay Zeka & Geliştirme Destek Araçları

ChatGPT (OpenAI) –
Oyun mimarisi, C/SDL2 kod yapısı, hata ayıklama ve dokümantasyon desteği.

OpenAI Codex –
Kod düzenleme, refactor ve SDL2 tabanlı animasyon/logic geliştirme sürecinde yardımcı araç.

Google Gemini –
Genel yazılım fikirleri, problem çözme ve alternatif yaklaşımlar için destek aracı.

Yapay zeka araçları, geliştirici tarafından yönlendirilmiş ve nihai kararlar manuel olarak verilmiştir.

---

### 📚 Teknik Kaynaklar

SDL2 Documentation
https://wiki.libsdl.org/

SDL2_image & SDL2_ttf Documentation
https://wiki.libsdl.org/SDL2_image

https://wiki.libsdl.org/SDL2_ttf

---

### 👩‍💻 Geliştirici

# Nezaket Demirci
# Bilgisayar Programcılığı Öğrencisi

### Bu proje, C programlama dili, oyun döngüsü (game loop), durum makineleri (state machine) ve temel 2D oyun mantığını öğrenmek amacıyla geliştirilmiştir.
