#include <stdio.h>
#include <string.h>

#include "fs.h"

// fs.c'deki fonksiyonların prototipleri
void fs_format();
void fs_create(const char* dosya_adi);
void fs_ls();
void fs_delete(const char* dosya_adi);

void fs_write(const char* dosya_adi, const char* veri, int boyut);
void fs_read(const char* dosya_adi, int offset, int boyut, char* buffer);



int main() {
    int secim;

    while (1) {
        printf("\n===== Basit Dosya Sistemi =====\n");
        printf("1. Disk Formatla\n");
        printf("3. Dosya Oluştur\n");
        printf("4. Dosyaları Listele\n");
        printf("5. Dosya Sil\n");
        printf("6. Dosyaya Veri Yaz\n");
        printf("7. Dosyadan Veri Oku\n");



        printf("0. Çıkış\n");
        printf("Seçiminiz: ");
        scanf("%d", &secim);

        if (secim == 1) {
            fs_format();
        } else if (secim == 3) {
            char ad[32];
            printf("Dosya adı: ");
            scanf("%s", ad);
            fs_create(ad);
        }else if (secim == 4) {
            fs_ls();
            
        } else if (secim == 5) {
            char ad[32];
            printf("Silinecek dosya adı: ");
            scanf("%s", ad);
            fs_delete(ad);
        }else if (secim == 6) {
    char ad[32], veri[1024];
    printf("Dosya adı: ");
    scanf("%s", ad);
    getchar();
    printf("Yazılacak veri: ");
    fgets(veri, sizeof(veri), stdin);
    veri[strcspn(veri, "\n")] = '\0';
    fs_write(ad, veri, strlen(veri));
}

else if (secim == 7) {
    char ad[32], buffer[1024];
    int offset, boyut;
    printf("Dosya adı: ");
    scanf("%s", ad);
    printf("Offset: ");
    scanf("%d", &offset);
    printf("Byte sayısı: ");
    scanf("%d", &boyut);
    fs_read(ad, offset, boyut, buffer);
}

        
        
        else if (secim == 0) {
            printf("Çıkılıyor...\n");
            break;
        } else {
            printf("Geçersiz seçim!\n");
        }
    }

    return 0;
}
