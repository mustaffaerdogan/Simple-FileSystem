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
        printf("8. Dosya Sil\n");
        printf("9. Dosya Varlık Kontrolü (fs_exists)\n");
        printf("10. Dosya Boyutunu Öğren (fs_size)\n");
        printf("11. Dosya Adını Değiştir (fs_rename)\n");

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
        } else if (secim == 8) {
            char ad[32];
            printf("Silinecek dosya adı: ");
            scanf("%s", ad);
            fs_delete(ad);
        }
        else if (secim == 9) {
            char ad[32];
            printf("Kontrol edilecek dosya adı: ");
            scanf("%s", ad);
            int sonuc = fs_exists(ad);
            printf("Dosya %s\n", sonuc ? "VAR" : "YOK");
        }
        else if (secim == 10) {
            char ad[32];
            printf("Boyutu öğrenilecek dosya adı: ");
            scanf("%s", ad);
            int b = fs_size(ad);
            if (b >= 0) printf("Dosya boyutu: %d byte\n", b);
            else printf("Dosya bulunamadı.\n");
        }
        else if (secim == 11) {
            char eski[32], yeni[32];
            printf("Eski dosya adı: ");
            scanf("%s", eski);
            printf("Yeni dosya adı: ");
            scanf("%s", yeni);
            fs_rename(eski, yeni);
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
