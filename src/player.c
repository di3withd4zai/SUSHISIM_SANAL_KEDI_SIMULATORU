/**
 * Dosya: src/player.c
 * Açıklama: Kedinin hareket matematiği ve görsel efektleri.
 */

#include "player.h"
#include <math.h> // Matematik fonksiyonları (Karekök, Sinüs)

#define SPEED 250.0f  // Kedinin hızı
#define SCALE 4       // Kedi ekranda 4 kat büyük görünsün

void player_init(Player* p, SDL_Texture* tIdle, SDL_Texture* tBox) {
    p->x = 600; p->y = 350; // Başlangıç konumu
    p->state = STATE_IDLE;
    p->texIdle = tIdle;
    p->texBox = tBox;       // Box3.png
    p->w = 32; p->h = 32;   // Sprite boyutu
    p->frameIndex = 0; p->frameTimer = 0;
    p->flip = SDL_FLIP_NONE;
}

// Kediyi bir noktaya gönderir
void player_set_target(Player* p, float x, float y, PlayerState actionOnArrival) {
    p->targetX = x;
    p->targetY = y;
    p->nextState = actionOnArrival; // Varınca yapılacak işi kaydet
    p->state = STATE_WALKING;       // Yürümeye başla
}

void player_update(Player* p, float dt) {
    // Animasyon karesini ilerlet
    p->frameTimer += dt;
    if (p->frameTimer > 0.2f) {
        p->frameIndex = (p->frameIndex + 1) % 4; // 0-1-2-3 döngüsü
        p->frameTimer = 0;
    }

    switch (p->state) {
        case STATE_WALKING: {
            // Vektör Matematiği: Hedefe olan farkı bul
            float dx = p->targetX - p->x;
            float dy = p->targetY - p->y;
            // Pisagor teoremi ile mesafeyi bul: c = sqrt(a^2 + b^2)
            float dist = sqrt(dx*dx + dy*dy);

            // Hedefe 15 piksel kadar yaklaştıysak varmış sayalım
            if (dist < 15.0f) {
                p->x = p->targetX;
                p->y = p->targetY;
                p->state = p->nextState; // Planlanan eyleme geç
                p->actionTimer = 4.0f;   // Eylem 4 saniye sürsün
            } else {
                // Hedefe doğru adım at (Normalize vektör * Hız * Zaman)
                p->x += (dx / dist) * SPEED * dt;
                p->y += (dy / dist) * SPEED * dt;
                
                // Yüzünü gittiği yere dön
                if (dx > 0) p->flip = SDL_FLIP_HORIZONTAL;
                else p->flip = SDL_FLIP_NONE;
            }
            break;
        }
        case STATE_EATING:
        case STATE_SLEEPING:
        case STATE_PLAYING:
            // Eylem süresini geri say
            p->actionTimer -= dt;
            if (p->actionTimer <= 0) p->state = STATE_IDLE;
            break;
        default: break;
    }
}

// Kediyi Çizme (Gelişmiş Efektlerle)
void player_render(Player* p, SDL_Renderer* r) {
    SDL_Texture* currentTex = p->texIdle;

    // Eğer oynuyorsa KUTU resmini (Box3.png) kullan
    if (p->state == STATE_PLAYING) {
        currentTex = p->texBox;
    }

    SDL_Rect src = { p->frameIndex * p->w, 0, p->w, p->h };
    SDL_Rect dst = { (int)p->x, (int)p->y, p->w * SCALE, p->h * SCALE };

    // --- ÖZEL EFEKTLER (Animasyon eksikliğini kapatmak için) ---
    
    // Varsayılan: Beyaz ve tam görünür
    SDL_SetTextureColorMod(currentTex, 255, 255, 255);
    SDL_SetTextureAlphaMod(currentTex, 255);

    if (p->state == STATE_SLEEPING) {
        // Uyurken rengi MAVİ yap ve hafif şeffaflaştır (Rüya efekti)
        SDL_SetTextureColorMod(currentTex, 150, 150, 255);
        SDL_SetTextureAlphaMod(currentTex, 200);
    } 
    else if (p->state == STATE_EATING) {
        // Yemek yerken rengi SARI yap ve titret (Yeme efekti)
        SDL_SetTextureColorMod(currentTex, 255, 255, 200);
        // Sinüs dalgası ile sağa sola sallanma
        dst.x += (int)(sin(p->frameTimer * 30) * 4); 
    }

    SDL_RenderCopyEx(r, currentTex, &src, &dst, 0.0, NULL, p->flip);

    // Çizim bitince efektleri temizle (Diğer nesneleri etkilemesin)
    SDL_SetTextureColorMod(currentTex, 255, 255, 255);
    SDL_SetTextureAlphaMod(currentTex, 255);
}