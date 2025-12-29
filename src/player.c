#include "player.h"

void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH) {
    p->x = 200; p->y = 280;
    p->speed = 350.0f;

    p->state = ANIM_IDLE;
    p->frameIndex = 0;
    p->frameTimer = 0;
    
    // Başlangıçta sağa baksın (NONE)
    p->flip = SDL_FLIP_NONE; 

    p->sheet = sheet;
    p->frameW = frameW;
    p->frameH = frameH;
}

void player_handle_input(Player* p, const Uint8* keys, float dt) {
    float vx = 0, vy = 0;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  vx -= 1;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) vx += 1;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    vy -= 1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  vy += 1;

    int moving = (vx != 0 || vy != 0);
    p->state = moving ? ANIM_WALK : ANIM_IDLE;

    // --- YÖN ÇEVİRME MANTIĞI ---
    // Sola gidiyorsa resmi AYNALA (Flip Horizontal)
    if (vx < 0) p->flip = SDL_FLIP_HORIZONTAL;
    
    // Sağa gidiyorsa resmi NORMAL yap
    if (vx > 0) p->flip = SDL_FLIP_NONE;

    // Hareket işlemleri
    if (moving) {
        float len = (vx*vx + vy*vy);
        if (len > 1) { vx *= 0.7071f; vy *= 0.7071f; }
        p->x += vx * p->speed * dt;
        p->y += vy * p->speed * dt;
    }
}

void player_update(Player* p, float dt) {
    // --- ANIMASYON DÜZELTMESİ ---
    
    if (p->state == ANIM_IDLE) {
        // Dururken 1. kareyi göster (Genelde oturma/bekleme karesi)
        p->frameIndex = 0; 
    } 
    else {
        // YÜRÜRKEN:
        // Eğer resim dosyan 'yürüyüş animasyonu' değil de 'yönler' içeriyorsa
        // sürekli kare değiştirmek kediyi döndürür.
        // O yüzden kareyi SABİTLİYORUZ.
        
        // DENEME-YANILMA: Eğer kedi garip duruyorsa buradaki '1' sayısını 0, 2 veya 3 yap.
        // Genelde yan profilden görünen kare 1. veya 2. karedir.
        p->frameIndex = 1; 
        
        // Not: Eğer gerçek bir adım atma animasyonun varsa aşağıdaki kodu açabilirsin:
        /*
        float frameDuration = 0.12f;
        p->frameTimer += dt;
        if (p->frameTimer >= frameDuration) {
            p->frameTimer = 0;
            p->frameIndex = (p->frameIndex + 1) % 4;
        }
        */
    }

    // Sınır Kontrolü (Ekran dışına çıkmasın)
    int catSize = 128; // 32 * 4
    if (p->x < 0) p->x = 0;
    if (p->y < 0) p->y = 0;
    if (p->x > 960 - catSize) p->x = 960 - catSize;
    if (p->y > 450 - catSize) p->y = 450 - catSize; 
}

void player_render(Player* p, SDL_Renderer* r) {
    // IDLE ise 0. satır, Yürüyorsa 1. satır
    int row = (p->state == ANIM_IDLE) ? 0 : 1;

    SDL_Rect src = { p->frameIndex * p->frameW, row * p->frameH, p->frameW, p->frameH };
    SDL_Rect dst = { (int)p->x, (int)p->y, p->frameW * 4, p->frameH * 4 }; 

    // SDL_RenderCopyEx ile çiziyoruz ki 'flip' özelliğini kullanabilelim
    SDL_RenderCopyEx(r, p->sheet, &src, &dst, 0.0, NULL, p->flip);
}