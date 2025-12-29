/**
 * Dosya: player.c
 * Amaç: Oyuncu (kedi) karakterinin hareketlerini, animasyon durumunu 
 * ve ekrana çizilmesini yönetir.
 */

#include "player.h"

// Oyuncuyu başlangıç değerleriyle hazırla
void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH) {
    p->x = 200; p->y = 280; // Başlangıç koordinatları
    p->speed = 350.0f;      // Hareket hızı (piksel/saniye)

    p->state = ANIM_IDLE;   // Başlangıçta duruyor
    p->frameIndex = 0;      // Animasyonun ilk karesi
    p->frameTimer = 0;
    
    p->flip = SDL_FLIP_NONE; // Başlangıçta normal (sola bakıyor varsayalım)

    p->sheet = sheet;       // Sprite görseli
    p->frameW = frameW;     // Bir karenin genişliği
    p->frameH = frameH;     // Bir karenin yüksekliği
}

// Klavye girişlerini işle (Input Handling)
void player_handle_input(Player* p, const Uint8* keys, float dt) {
    float vx = 0, vy = 0; // Hız vektörleri

    // Tuşlara göre yön belirle
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  vx -= 1; // Sola
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) vx += 1; // Sağa
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    vy -= 1; // Yukarı
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  vy += 1; // Aşağı

    // Hareket var mı?
    int moving = (vx != 0 || vy != 0);
    p->state = moving ? ANIM_WALK : ANIM_IDLE; // Hareket varsa yürüme animasyonu, yoksa durma

    // --- YÖN DÜZELTMESİ (FLIP) ---
    // vx < 0 ise (Sola gidiyor): Resmi olduğu gibi çiz.
    if (vx < 0) p->flip = SDL_FLIP_NONE;
    
    // vx > 0 ise (Sağa gidiyor): Resmi yatayda çevir (Mirror).
    if (vx > 0) p->flip = SDL_FLIP_HORIZONTAL;

    // Koordinatları güncelle
    if (moving) {
        // Çapraz giderken hızın artmaması için vektörü normalize et (Pisagor teoremi)
        float len = (vx*vx + vy*vy);
        if (len > 1) { vx *= 0.7071f; vy *= 0.7071f; } // 1/kök(2) ile çarp
        
        // Yeni pozisyon = Eski Pozisyon + Hız * Zaman
        p->x += vx * p->speed * dt;
        p->y += vy * p->speed * dt;
    }
}

// Oyuncunun durumunu güncelle (Update)
void player_update(Player* p, float dt) {
    if (p->state == ANIM_IDLE) {
        p->frameIndex = 0; // Dururken 0. kareyi göster
    } 
    else {
        // Yürürken belirli bir kareyi göster (Basit animasyon)
        p->frameIndex = 2; 
    }

    // --- SINIR KONTROLÜ (Boundary Checking) ---
    // Kedinin ekrandan dışarı çıkmasını engelle
    int catSize = 128; // Ekranda çizilen yaklaşık boyut (scale edilmiş hali)
    if (p->x < 0) p->x = 0;
    if (p->y < 0) p->y = 0;
    if (p->x > 960 - catSize) p->x = 960 - catSize;
    if (p->y > 450 - catSize) p->y = 450 - catSize; 
}

// Oyuncuyu ekrana çiz (Render)
void player_render(Player* p, SDL_Renderer* r) {
    // Sprite sheet'te hangi satırı kullanacağız? (0: Durma, 1: Yürüme gibi)
    int row = (p->state == ANIM_IDLE) ? 0 : 1;

    // Kaynak dikdörtgen (Resim dosyasındaki hangi parça alınacak?)
    SDL_Rect src = { p->frameIndex * p->frameW, row * p->frameH, p->frameW, p->frameH };
    
    // Hedef dikdörtgen (Ekranda nereye ve ne kadar büyük çizilecek?)
    // frameW * 4 diyerek resmi 4 kat büyüttük (Pixel art tarzı için)
    SDL_Rect dst = { (int)p->x, (int)p->y, p->frameW * 4, p->frameH * 4 }; 

    // Resmi çevirme (flip) özelliğiyle birlikte çiz
    SDL_RenderCopyEx(r, p->sheet, &src, &dst, 0.0, NULL, p->flip);
}