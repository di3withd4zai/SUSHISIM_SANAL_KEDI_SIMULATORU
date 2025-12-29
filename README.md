# 🐱 SushiSim – Sanal Kedi Simülasyonu

<div align="center">

**Pixel art tarzında, C ve SDL2 ile geliştirilmiş interaktif sanal evcil hayvan oyunu.**

[ Özellikler ] • [ Kontroller ] • [ Kurulum ]

</div>

---

## 📖 Proje Hakkında

**SushiSim**, oyuncunun **Sushi** isimli bir kedinin ihtiyaçlarını yönettiği gerçek zamanlı bir simülasyondur. Oyuncu; açlık, mutluluk ve enerji dengesini korurken, aynı zamanda çevresel temizliği sağlayarak en yüksek skora ulaşmaya çalışır.

> **Amaç:** Kediyi hayatta tutmak ve en yüksek puanı toplamak!

---

## 🎮 Oyun Özellikleri

| Özellik | Açıklama |
| :--- | :--- |
| 🐾 **Pixel Art Karakter** | Animasyonlu, yürüyen ve yön değiştirebilen kedi karakteri. |
| 🧠 **İstatistik Sistemi** | **Açlık**, **Mutluluk** ve **Enerji** değerleri zamanla azalır. |
| 💩 **Kirlilik Mekaniği** | Kedi rastgele zamanlarda **kaka yapabilir**. Temizlenmezse mutluluk hızla düşer. |
| ✨ **Efektler** | Sevgi gösterildiğinde (oynama/besleme) ekranda **kalp partikülleri** belirir. |
| 🖱️ **Etkileşim** | Mouse ile buton kontrolü ve klavye kısayolları. |
| 💀 **Game Loop** | İstatistikler tükendiğinde **Game Over** olur, tek tuşla yeniden başlar. |

---

## 🕹️ Kontroller

Oyun hem **Mouse** hem de **Klavye** ile tam kontrol imkanı sunar.

### 🖱️ Mouse Kontrolleri
Ekrandaki butonlara tıklayarak aşağıdaki işlemleri yapabilirsiniz:

| Buton | İşlev | Etkisi |
| :--- | :--- | :--- |
| **BASLA** | Oyunu Başlat | Ana menüden oyuna geçer. |
| **MAMA** | Besle | `+Açlık` `+Mutluluk` |
| **OYNA** | Oyun Oyna | `++Mutluluk` `-Enerji` `-Açlık` |
| **UYU** | Dinlen | `++Enerji` `-Açlık` |
| **TEMİZLE** | Temizlik Yap | Yerdeki kirliliği temizler (`+Mutluluk`). |
| **TEKRAR** | Restart | Oyunu sıfırlar (Game Over ekranında). |

### ⌨️ Klavye Kısayolları

| Tuş | İşlem |
| :---: | :--- |
| **W / A / S / D** | Kediyi oda içinde hareket ettirir. |
| **F** | Mama Ver (Feed) |
| **P** | Oyna (Play) |
| **R** | Uyu/Dinlen (Rest) |
| **ESC** | Oyundan Çıkış |
| **ENTER** | Yeniden Başlat (Restart) |

---

## ⚙️ Kurulum ve Derleme (MSYS2)

Projenin Windows üzerinde **MSYS2** ortamında derlenmesi için aşağıdaki adımları izleyin.

### 1. Gerekli Paketlerin Kurulumu
Terminali açın ve gerekli kütüphaneleri indirin:

pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-SDL2 \
          mingw-w64-ucrt-x86_64-SDL2_image \
          mingw-w64-ucrt-x86_64-SDL2_ttf


### 2. Derleme (Compile)

Proje dizinine gidin ve derleme komutunu çalıştırın:


gcc src/*.c -o SushiSim.exe `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf



### 3. Çalıştırma

Oyunu başlatmak için:


./SushiSim.exe



> ⚠️ **ÖNEMLİ NOT:**
> `assets` klasörü (resimler ve fontlar) ile oluşturulan `SushiSim.exe` dosyası **aynı klasörde** bulunmalıdır. Aksi takdirde oyun görselleri yükleyemez ve kapanır.

---

## 🗂️ Proje Yapısı

Kodun okunabilirliğini artırmak için **modüler** bir yapı tercih edilmiştir:


SushiSim/
├── src/
│   ├── main.c           # Giriş noktası (Entry Point)
│   ├── game.c           # Oyun döngüsü ve olay yönetimi
│   ├── player.c         # Karakter animasyon ve hareketi
│   ├── stats.c          # Matematiksel hesaplamalar
│   └── render.c         # Çizim fonksiyonları
├── assets/
│   ├── sushi_sheet.png  # Karakter Sprite Atlası
│   └── font.ttf         # Oyun Fontu
└── README.md            # Proje Dokümantasyonu



---

## 🧩 Kullanılan Teknolojiler

* **Dil:** C (Standard C99/C11)
* **Motor:** SDL2 (Simple DirectMedia Layer)
* **Eklentiler:** SDL2_image, SDL2_ttf
* **Derleyici:** GCC (MinGW-w64)

---

## 🎨 Kaynakça ve Lisanslar

* **Geliştirici:** Nezaket Demirci (Bilgisayar Programcılığı Öğrencisi)
* **Grafikler:** [Kittens Cats Character Pack](https://last-tick.itch.io/kittens-cats-character-32x32-pixel-animation-pack) (itch.io - last-tick)
* **Lisans:** Bu proje eğitim amacıyla geliştirilmiştir.
