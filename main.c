#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        printf("12. Dosyaya veri ekle (append)\n");
        printf("13. Dosya içeriğini kes (truncate)\n");
        printf("14. Dosyayı kopyala\n");
        printf("15. Disk Yedeğini Al (fs_backup)\n");
        printf("16. Yedekten Geri Yükle (fs_restore)\n");
        printf("17. Disk Tutarlılığını Kontrol Et (fs_check_integrity)\n");
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
        else if (secim == 12) {
            // Dosyaya veri ekleme (append) işlemi
            char ad[32], veri[1024];
            printf("Veri eklenecek dosya adı: ");
            scanf("%s", ad);
            getchar();
            printf("Eklenecek veri: ");
            fgets(veri, sizeof(veri), stdin);
            veri[strcspn(veri, "\n")] = '\0'; // \n karakterini temizle
            fs_append(ad, veri, strlen(veri));
        }
        else if (secim == 13) {
            // Dosya kesme (truncate) işlemi
            char ad[32];
            int yeni_boyut;
            printf("Kesilecek dosya adı: ");
            scanf("%s", ad);
            printf("Yeni dosya boyutu (byte): ");
            scanf("%d", &yeni_boyut);
            
            if (yeni_boyut < 0) {
                printf("Hata: Dosya boyutu negatif olamaz.\n");
            } else {
                fs_truncate(ad, yeni_boyut);
            }
        }
        else if (secim == 14) {
            // Dosya kopyalama işlemi
            char kaynak[32], hedef[32];
            printf("Kaynak dosya adı: ");
            scanf("%s", kaynak);
            printf("Hedef dosya adı: ");
            scanf("%s", hedef);
            fs_copy(kaynak, hedef);
        }
        else if (secim == 15) {
            // Disk yedekleme işlemi
            char yedek_dosya[64];
            printf("Yedek dosya adı (örn: backup.sim): ");
            scanf("%s", yedek_dosya);
            fs_backup(yedek_dosya);
        }
        else if (secim == 16) {
            // Diskten geri yükleme işlemi
            char yedek_dosya[64];
            printf("Geri yüklenecek yedek dosya adı: ");
            scanf("%s", yedek_dosya);
            printf("DİKKAT: Mevcut disk.sim dosyası tamamen silinecek! Devam etmek istiyor musunuz? (e/h): ");
            char onay;
            scanf(" %c", &onay);
            
            if (onay == 'e' || onay == 'E') {
                fs_restore(yedek_dosya);
            } else {
                printf("Geri yükleme işlemi iptal edildi.\n");
            }
        }
        else if (secim == 17) {
            // Disk tutarlılık kontrolü
            fs_check_integrity();
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
