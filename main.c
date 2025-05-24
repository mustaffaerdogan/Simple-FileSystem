#include <stdio.h>
#include <string.h>
#include "fs.h"

int main() {
    int secim;
    char ad[32], veri[1024], buffer[1024];

    while (1) {
        printf("\n===== Basit Dosya Sistemi =====\n");
        printf("1. Disk Formatla\n");
        printf("3. Dosya Oluştur\n");
        printf("4. Dosyaları Listele\n");
        printf("6. Dosyaya Veri Yaz\n");
        printf("7. Dosyadan Veri Oku\n");
        printf("0. Çıkış\n");
        printf("Seçiminiz: ");
        scanf("%d", &secim);
        getchar(); // \n temizle

        if (secim == 1) {
            fs_format();
        } else if (secim == 3) {
            printf("Dosya adı: ");
            scanf("%s", ad);
            getchar();
            fs_create(ad);
        } else if (secim == 4) {
            fs_ls();
        } else if (secim == 6) {
            printf("Dosya adı: ");
            scanf("%s", ad);
            getchar();
            printf("Yazılacak veri: ");
            fgets(veri, sizeof(veri), stdin);
            veri[strcspn(veri, "\n")] = '\0'; // \n sil
            fs_write(ad, veri, strlen(veri));
        } else if (secim == 7) {
            int offset, boyut;
            printf("Dosya adı: ");
            scanf("%s", ad);
            printf("Offset: ");
            scanf("%d", &offset);
            printf("Byte sayısı: ");
            scanf("%d", &boyut);
            fs_read(ad, offset, boyut, buffer);
        } else if (secim == 0) {
            printf("Çıkılıyor...\n");
            break;
        } else {
            printf("Geçersiz seçim!\n");
        }
    }

    return 0;
}
