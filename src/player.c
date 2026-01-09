/**
 * Dosya: src/player.c
 * Açıklama: Kedinin hareket matematiği ve görsel efektleri.
 * GUNCELLEME: Uyuma animasyonu için özel sprite desteği eklendi.
 */

#include "player.h"
#include <math.h> 

#define SPEED 250.0f  
// Kedinin genel boyutu (Yürürken)
#define SCALE 4       

void player_init(Player* p, SDL_Texture* tIdle, SDL_Texture* tBox) {
    p->x = 600; p->y = 350; 
    p->state = STATE_IDLE;
    p->texIdle = tIdle;
    p->texBox = tBox;       
    p->w = 32; p->h = 32;   
    p->frameIndex = 0; p->frameTimer = 0;
    p->flip = SDL_FLIP_NONE;
}

void player_set_target(Player* p, float x, float y, PlayerState actionOnArrival) {
    p->targetX = x;
    p->targetY = y;
    p->nextState = actionOnArrival; 
    p->state = STATE_WALKING;       
}

void player_update(Player* p, float dt) {
    p->frameTimer += dt;
    if (p->frameTimer > 0.2f) {
        p->frameIndex = (p->frameIndex + 1) % 4; 
        p->frameTimer = 0;
    }

    switch (p->state) {
        case STATE_WALKING: {
            float dx = p->targetX - p->x;
            float dy = p->targetY - p->y;
            float dist = sqrt(dx*dx + dy*dy);

            if (dist < 15.0f) {
                p->x = p->targetX;
                p->y = p->targetY;
                p->state = p->nextState; 
                p->actionTimer = 4.0f;   
            } else {
                p->x += (dx / dist) * SPEED * dt;
                p->y += (dy / dist) * SPEED * dt;
                
                if (dx > 0) p->flip = SDL_FLIP_HORIZONTAL;
                else p->flip = SDL_FLIP_NONE;
            }
            break;
        }
        case STATE_EATING:
        case STATE_SLEEPING:
        case STATE_PLAYING:
            p->actionTimer -= dt;
            if (p->actionTimer <= 0) p->state = STATE_IDLE;
            break;
        default: break;
    }
}

// >>> RENDER FONKSİYONU GÜNCELLENDİ <<<
void player_render(Player* p, SDL_Renderer* r) {
    SDL_Texture* currentTex = p->texIdle;
    
    // Varsayılan Kesim (Yürüme/Bekleme) - 32x32 kareler
    SDL_Rect src = { p->frameIndex * p->w, 0, p->w, p->h }; 
    SDL_Rect dst = { (int)p->x, (int)p->y, p->w * SCALE, p->h * SCALE };

    // 1. OYUN OYNAMA DURUMU (Kutunun içine girme veya oyuncakla oynama)
    if (p->state == STATE_PLAYING) {
        currentTex = p->texBox; // Furnitures.png kullanılıyor
        
        // game.c'deki TOY değerlerini buraya manuel yazıyoruz:
        src.x = 370; src.y = 10;
        src.w = 80;  src.h = 120;
        
        dst.w = src.w * 2; 
        dst.h = src.h * 2;
        
        // Konum düzeltmesi (Tam olduğu yerde görünsün)
        dst.x = (int)p->x;
        dst.y = (int)p->y - 50; 
    }
    
    // 2. UYUMA DURUMU (ÖZEL SPRITE VE KONUM AYARI)
    else if (p->state == STATE_SLEEPING) {
        // --- AYARLAR BAŞLANGICI ---
        
        // A) Hangi dosyada? (Idle.png ise texIdle, Furnitures.png ise texBox)
        currentTex = p->texIdle; 
        
        // B) Resmin koordinatları (Sprite Sheet üzerindeki yeri)
        // Not: Buraya kendi uyuyan kedi resminin koordinatlarını girmelisin.
        src.x = 62;    // Örnek: İlk kare
        src.y = 86;    // Örnek: İlk satır
        src.w = 48;   // Genişlik
        src.h = 37;   // Yükseklik

        // C) Yatağın üzerinde hizalama (Offset)
        // Kedi zaten yatağa yürüdü (p->x, p->y orada), ama tam yastığa koymak için:
        int shiftX = 20;  // Sağa kaydır
        int shiftY = -15; // Yukarı kaldır (Yatağın üstünde gibi dursun)

        // D) Ekranda görünecek boyut
        dst.w = src.w * 2; 
        dst.h = src.h * 2;
    }

    // 3. YEMEK YEME DURUMU
    else if (p->state == STATE_EATING) {
        SDL_SetTextureColorMod(currentTex, 255, 255, 200);
        // Titreme efekti (Yemek yerken mutlu sallanma)
        dst.x += (int)(sin(p->frameTimer * 30) * 4); 
    }

    // Ekrana Çiz
    SDL_RenderCopyEx(r, currentTex, &src, &dst, 0.0, NULL, p->flip);

    // Çizim bitince renk modlarını sıfırla (Diğer çizimleri etkilemesin)
    SDL_SetTextureColorMod(currentTex, 255, 255, 255);
    SDL_SetTextureAlphaMod(currentTex, 255);
}