#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

void fs_format() {
    FILE* disk = fopen("disk.sim", "w+b");
    if (disk == NULL) {
        perror("disk.sim oluşturulamadı");
        return;
    }

    // Initialize metadata
    Metadata metadata = {0};
    fseek(disk, 0, SEEK_SET);
    fwrite(&metadata, sizeof(Metadata), 1, disk);
    
    // Properly allocate the data region by writing zeros
    // This ensures the entire file is actually allocated on disk
    char zero_block[BLOCK_SIZE];
    memset(zero_block, 0, BLOCK_SIZE);
    
    // Write zeros for the entire data region (1000 blocks)
    for (int i = 0; i < 1000; i++) {
        fwrite(zero_block, 1, BLOCK_SIZE, disk);
    }
    
    fflush(disk);
    fclose(disk);
    printf("Disk formatlandı. Tüm dosyalar silindi.\n");
    
    // Loglama işlemi
    fs_log("FORMAT", "disk.sim");
}

void fs_create(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi &&
            strcmp(metadata.dosyalar[i].dosya_adi, dosya_adi) == 0) {
            printf("Hata: '%s' zaten var.\n", dosya_adi);
            fclose(disk);
            return;
        }
    }

    if (metadata.dosya_sayisi >= MAX_FILES) {
        printf("Hata: Maksimum dosya sayısına ulaşıldı.\n");
        fclose(disk);
        return;
    }

    DosyaGirdisi yeni = {0};
    strncpy(yeni.dosya_adi, dosya_adi, MAX_FILENAME_LENGTH);
    yeni.aktif_mi = 1;
    yeni.boyut = 0;

    int son_adres = METADATA_SIZE;
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            int bitis = metadata.dosyalar[i].baslangic_adresi +
                        (metadata.dosyalar[i].boyut > 0 ? metadata.dosyalar[i].boyut : BLOCK_SIZE);
            if (bitis > son_adres)
                son_adres = bitis;
        }
    }

    yeni.baslangic_adresi = son_adres;
    metadata.dosyalar[metadata.dosya_sayisi++] = yeni;

    fseek(disk, 0, SEEK_SET);
    fwrite(&metadata, sizeof(Metadata), 1, disk);
    fflush(disk);
    fclose(disk);
    printf("'%s' oluşturuldu.\n", dosya_adi);
    
    // Loglama işlemi
    fs_log("CREATE", dosya_adi);
}

void fs_write(const char* dosya_adi, const char* veri, int boyut) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            fseek(disk, d->baslangic_adresi, SEEK_SET);
            fwrite(veri, sizeof(char), boyut, disk);
            fflush(disk);

            d->boyut = boyut;

            fseek(disk, 0, SEEK_SET);
            fwrite(&metadata, sizeof(Metadata), 1, disk);
            fflush(disk);

            fclose(disk);
            printf("'%s' dosyasına yazıldı.\n", dosya_adi);
            
            // Loglama işlemi
            fs_log("WRITE", dosya_adi);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", dosya_adi);
}

void fs_read(const char* dosya_adi, int offset, int boyut, char* buffer) {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            if (offset > d->boyut) {
                printf("Offset dosya boyutunu aşıyor.\n");
                fclose(disk);
                return;
            }

            if (offset + boyut > d->boyut)
                boyut = d->boyut - offset;

            fseek(disk, d->baslangic_adresi + offset, SEEK_SET);
            fread(buffer, sizeof(char), boyut, disk);
            buffer[boyut] = '\0';

            printf("Okunan veri: %s\n", buffer);
            fclose(disk);
            
            // Loglama işlemi
            char log_mesaji[64];
            snprintf(log_mesaji, sizeof(log_mesaji), "%s(offset:%d,size:%d)", dosya_adi, offset, boyut);
            fs_log("READ", log_mesaji);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", dosya_adi);
}

void fs_ls() {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);
    fclose(disk);

    printf("Mevcut dosyalar:\n");
    int aktif_sayisi = 0;
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            printf("- %s (%d byte)\n", metadata.dosyalar[i].dosya_adi, metadata.dosyalar[i].boyut);
            aktif_sayisi++;
        }
    }
    
    // Loglama işlemi
    char log_mesaji[32];
    snprintf(log_mesaji, sizeof(log_mesaji), "(%d files)", aktif_sayisi);
    fs_log("LIST", log_mesaji);
}

void fs_delete(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            // Dosyayı pasif hale getir
            d->aktif_mi = 0;
            
            // Metadata'yı güncelle
            fseek(disk, 0, SEEK_SET);
            fwrite(&metadata, sizeof(Metadata), 1, disk);
            fflush(disk);
            
            fclose(disk);
            printf("'%s' dosyası silindi.\n", dosya_adi);
            
            // Loglama işlemi
            fs_log("DELETE", dosya_adi);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", dosya_adi);
}

int fs_exists(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) return 0;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && 
            strcmp(metadata.dosyalar[i].dosya_adi, dosya_adi) == 0) {
            fclose(disk);
            
            // Loglama işlemi
            fs_log("EXISTS", dosya_adi);
            return 1; // Dosya mevcut ve aktif
        }
    }

    fclose(disk);
    
    // Loglama işlemi (dosya bulunamadı)
    char log_mesaji[64];
    snprintf(log_mesaji, sizeof(log_mesaji), "%s(NOT_FOUND)", dosya_adi);
    fs_log("EXISTS", log_mesaji);
    return 0; // Dosya bulunamadı veya aktif değil
}

int fs_size(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) return -1;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && 
            strcmp(metadata.dosyalar[i].dosya_adi, dosya_adi) == 0) {
            int boyut = metadata.dosyalar[i].boyut;
            fclose(disk);
            
            // Loglama işlemi
            char log_mesaji[64];
            snprintf(log_mesaji, sizeof(log_mesaji), "%s(%d bytes)", dosya_adi, boyut);
            fs_log("SIZE", log_mesaji);
            return boyut; // Dosya boyutunu döndür
        }
    }

    fclose(disk);
    
    // Loglama işlemi (dosya bulunamadı)
    char log_mesaji[64];
    snprintf(log_mesaji, sizeof(log_mesaji), "%s(NOT_FOUND)", dosya_adi);
    fs_log("SIZE", log_mesaji);
    return -1; // Dosya bulunamadı
}

void fs_rename(const char* eski_ad, const char* yeni_ad) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Yeni adın zaten kullanılıp kullanılmadığını kontrol et
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && 
            strcmp(metadata.dosyalar[i].dosya_adi, yeni_ad) == 0) {
            printf("Hata: '%s' adında bir dosya zaten var.\n", yeni_ad);
            fclose(disk);
            return;
        }
    }

    // Eski dosyayı bul ve adını değiştir
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, eski_ad) == 0) {
            // Dosya adını güncelle
            strncpy(d->dosya_adi, yeni_ad, MAX_FILENAME_LENGTH);
            d->dosya_adi[MAX_FILENAME_LENGTH - 1] = '\0'; // Null terminator güvenliği
            
            // Metadata'yı güncelle
            fseek(disk, 0, SEEK_SET);
            fwrite(&metadata, sizeof(Metadata), 1, disk);
            fflush(disk);
            
            fclose(disk);
            printf("'%s' dosyası '%s' olarak yeniden adlandırıldı.\n", eski_ad, yeni_ad);
            
            // Loglama işlemi
            char log_mesaji[128];
            snprintf(log_mesaji, sizeof(log_mesaji), "%s->%s", eski_ad, yeni_ad);
            fs_log("RENAME", log_mesaji);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", eski_ad);
}

void fs_append(const char* dosya_adi, const char* veri, int boyut) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            // Dosyanın sonuna git (başlangıç + mevcut boyut)
            fseek(disk, d->baslangic_adresi + d->boyut, SEEK_SET);
            
            // Yeni veriyi dosyanın sonuna ekle
            fwrite(veri, sizeof(char), boyut, disk);
            fflush(disk);

            // Dosya boyutunu güncelle
            d->boyut += boyut;

            // Metadata'yı güncelle
            fseek(disk, 0, SEEK_SET);
            fwrite(&metadata, sizeof(Metadata), 1, disk);
            fflush(disk);

            fclose(disk);
            printf("'%s' dosyasına %d byte veri eklendi.\n", dosya_adi, boyut);
            
            // Loglama işlemi
            fs_log("APPEND", dosya_adi);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", dosya_adi);
}

void fs_truncate(const char* dosya_adi, int yeni_boyut) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) return;

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            int eski_boyut = d->boyut;
            
            if (yeni_boyut > eski_boyut) {
                // Dosyayı büyütmek için sıfırlar ekle
                fseek(disk, d->baslangic_adresi + eski_boyut, SEEK_SET);
                
                char sifir = 0;
                for (int j = 0; j < (yeni_boyut - eski_boyut); j++) {
                    fwrite(&sifir, sizeof(char), 1, disk);
                }
                fflush(disk);
                
                printf("'%s' dosyası %d byte'tan %d byte'a genişletildi.\n", 
                       dosya_adi, eski_boyut, yeni_boyut);
            } else if (yeni_boyut < eski_boyut) {
                printf("'%s' dosyası %d byte'tan %d byte'a kısaltıldı.\n", 
                       dosya_adi, eski_boyut, yeni_boyut);
            } else {
                printf("'%s' dosyası zaten %d byte boyutunda.\n", dosya_adi, yeni_boyut);
            }

            // Dosya boyutunu güncelle
            d->boyut = yeni_boyut;

            // Metadata'yı güncelle
            fseek(disk, 0, SEEK_SET);
            fwrite(&metadata, sizeof(Metadata), 1, disk);
            fflush(disk);

            fclose(disk);
            
            // Loglama işlemi
            char log_mesaji[64];
            snprintf(log_mesaji, sizeof(log_mesaji), "%s(%d)", dosya_adi, yeni_boyut);
            fs_log("TRUNCATE", log_mesaji);
            return;
        }
    }

    fclose(disk);
    printf("Hata: '%s' bulunamadı.\n", dosya_adi);
}

void fs_copy(const char* kaynak_ad, const char* hedef_ad) {
    // Önce hedef dosyanın zaten var olup olmadığını kontrol et
    if (fs_exists(hedef_ad)) {
        printf("Hata: '%s' adında bir dosya zaten var.\n", hedef_ad);
        return;
    }

    // Kaynak dosyanın var olup olmadığını ve boyutunu kontrol et
    if (!fs_exists(kaynak_ad)) {
        printf("Hata: Kaynak dosya '%s' bulunamadı.\n", kaynak_ad);
        return;
    }

    int kaynak_boyut = fs_size(kaynak_ad);
    if (kaynak_boyut == -1 || kaynak_boyut == 0) {
        printf("Hata: Kaynak dosya '%s' boş veya okunamıyor.\n", kaynak_ad);
        return;
    }

    // Kaynak dosyayı oku
    char* buffer = malloc(kaynak_boyut);
    if (!buffer) {
        printf("Hata: Bellek tahsis edilemedi.\n");
        return;
    }

    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) {
        free(buffer);
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Kaynak dosyayı bul ve oku
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && 
            strcmp(metadata.dosyalar[i].dosya_adi, kaynak_ad) == 0) {
            
            fseek(disk, metadata.dosyalar[i].baslangic_adresi, SEEK_SET);
            fread(buffer, sizeof(char), kaynak_boyut, disk);
            break;
        }
    }
    fclose(disk);

    // Hedef dosyayı oluştur
    fs_create(hedef_ad);

    // Hedef dosyaya veriyi yaz
    fs_write(hedef_ad, buffer, kaynak_boyut);

    free(buffer);
    printf("'%s' dosyası '%s' olarak kopyalandı (%d byte).\n", 
           kaynak_ad, hedef_ad, kaynak_boyut);
    
    // Loglama işlemi
    char log_mesaji[128];
    snprintf(log_mesaji, sizeof(log_mesaji), "%s->%s", kaynak_ad, hedef_ad);
    fs_log("COPY", log_mesaji);
}

void fs_backup(const char* yedek_dosya_adi) {
    // Kaynak dosyayı okuma modunda aç
    FILE* kaynak = fopen("disk.sim", "rb");
    if (!kaynak) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        return;
    }

    // Hedef yedek dosyayı yazma modunda aç
    FILE* hedef = fopen(yedek_dosya_adi, "wb");
    if (!hedef) {
        printf("Hata: Yedek dosyası oluşturulamadı.\n");
        fclose(kaynak);
        return;
    }

    // Dosyayı blok blok kopyala
    char buffer[BLOCK_SIZE];
    size_t okunan;
    int toplam_byte = 0;

    while ((okunan = fread(buffer, 1, BLOCK_SIZE, kaynak)) > 0) {
        fwrite(buffer, 1, okunan, hedef);
        toplam_byte += okunan;
    }

    fflush(hedef);
    fclose(kaynak);
    fclose(hedef);

    printf("Disk yedeği başarıyla alındı: '%s' (%d byte)\n", yedek_dosya_adi, toplam_byte);
    
    // Loglama işlemi
    fs_log("BACKUP", yedek_dosya_adi);
}

void fs_restore(const char* yedek_dosya_adi) {
    // Yedek dosyayı okuma modunda aç
    FILE* kaynak = fopen(yedek_dosya_adi, "rb");
    if (!kaynak) {
        printf("Hata: Yedek dosyası '%s' bulunamadı.\n", yedek_dosya_adi);
        return;
    }

    // Mevcut disk.sim dosyasını yazma modunda aç (üzerine yaz)
    FILE* hedef = fopen("disk.sim", "wb");
    if (!hedef) {
        printf("Hata: disk.sim dosyası oluşturulamadı.\n");
        fclose(kaynak);
        return;
    }

    // Yedek dosyayı disk.sim'e blok blok kopyala
    char buffer[BLOCK_SIZE];
    size_t okunan;
    int toplam_byte = 0;

    while ((okunan = fread(buffer, 1, BLOCK_SIZE, kaynak)) > 0) {
        fwrite(buffer, 1, okunan, hedef);
        toplam_byte += okunan;
    }

    fflush(hedef);
    fclose(kaynak);
    fclose(hedef);

    printf("Disk başarıyla geri yüklendi: '%s'den %d byte geri yüklendi\n", yedek_dosya_adi, toplam_byte);
    
    // Loglama işlemi
    fs_log("RESTORE", yedek_dosya_adi);
}

void fs_check_integrity() {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);
    fclose(disk);

    printf("\n=== Disk Tutarlılık Kontrolü ===\n");
    printf("Toplam dosya sayısı: %d\n", metadata.dosya_sayisi);

    int aktif_dosya_sayisi = 0;
    int hata_var = 0;

    // Her aktif dosyayı kontrol et
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        
        if (d->aktif_mi) {
            aktif_dosya_sayisi++;
            
            // Dosya sınırlarının 1MB'ı aşıp aşmadığını kontrol et
            int dosya_sonu = d->baslangic_adresi + d->boyut;
            if (dosya_sonu > 1048576) {
                printf("HATA: '%s' dosyası disk sınırlarını aşıyor (başlangıç: %d, boyut: %d, son: %d)\n", 
                       d->dosya_adi, d->baslangic_adresi, d->boyut, dosya_sonu);
                hata_var = 1;
            }

            // Diğer aktif dosyalarla çakışma kontrolü
            for (int j = i + 1; j < metadata.dosya_sayisi; j++) {
                DosyaGirdisi* diger = &metadata.dosyalar[j];
                
                if (diger->aktif_mi) {
                    int d_baslangic = d->baslangic_adresi;
                    int d_bitis = d->baslangic_adresi + d->boyut;
                    int diger_baslangic = diger->baslangic_adresi;
                    int diger_bitis = diger->baslangic_adresi + diger->boyut;

                    // Çakışma kontrolü: iki dosya aralığının kesişip kesişmediğini kontrol et
                    if ((d_baslangic < diger_bitis) && (diger_baslangic < d_bitis)) {
                        printf("HATA: Dosya çakışması bulundu!\n");
                        printf("  '%s': %d-%d arası\n", d->dosya_adi, d_baslangic, d_bitis-1);
                        printf("  '%s': %d-%d arası\n", diger->dosya_adi, diger_baslangic, diger_bitis-1);
                        hata_var = 1;
                    }
                }
            }
        }
    }

    printf("Aktif dosya sayısı: %d\n", aktif_dosya_sayisi);
    
    if (hata_var) {
        printf("❌ Disk tutarlılık hataları bulundu!\n");
    } else {
        printf("✅ Tüm dosyalar tutarlı\n");
    }
    printf("=============================\n");
    
    // Loglama işlemi
    char log_mesaji[64];
    snprintf(log_mesaji, sizeof(log_mesaji), "(%d files, %s)", aktif_dosya_sayisi, hata_var ? "ERRORS" : "OK");
    fs_log("CHECK_INTEGRITY", log_mesaji);
}

void fs_cat(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Dosyayı metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {
            // Dosya bulundu, içeriğini oku ve göster
            if (d->boyut == 0) {
                printf("Dosya boş.\n");
                fclose(disk);
                
                // Loglama işlemi
                char log_mesaji[64];
                snprintf(log_mesaji, sizeof(log_mesaji), "%s(empty)", dosya_adi);
                fs_log("CAT", log_mesaji);
                return;
            }

            // Dosya içeriği için bellek ayır
            char* buffer = malloc(d->boyut + 1);
            if (!buffer) {
                printf("Hata: Bellek tahsis edilemedi.\n");
                fclose(disk);
                return;
            }

            // Dosya başlangıç adresine git ve içeriği oku
            fseek(disk, d->baslangic_adresi, SEEK_SET);
            fread(buffer, sizeof(char), d->boyut, disk);
            buffer[d->boyut] = '\0'; // Null terminator ekle

            printf("\n=== '%s' dosyasının içeriği (%d byte) ===\n", dosya_adi, d->boyut);
            printf("%s\n", buffer);
            printf("=== Dosya sonu ===\n");

            free(buffer);
            fclose(disk);
            
            // Loglama işlemi
            char log_mesaji[64];
            snprintf(log_mesaji, sizeof(log_mesaji), "%s(%d bytes)", dosya_adi, d->boyut);
            fs_log("CAT", log_mesaji);
            return;
        }
    }

    // Dosya bulunamadı
    fclose(disk);
    printf("Hata: '%s' dosyası bulunamadı.\n", dosya_adi);
    
    // Loglama işlemi (dosya bulunamadı)
    char log_mesaji[64];
    snprintf(log_mesaji, sizeof(log_mesaji), "%s(NOT_FOUND)", dosya_adi);
    fs_log("CAT", log_mesaji);
}

void fs_diff(const char* dosya1_adi, const char* dosya2_adi) {
    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    DosyaGirdisi* dosya1 = NULL;
    DosyaGirdisi* dosya2 = NULL;

    // Her iki dosyayı da metadata içinde ara
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            if (strcmp(metadata.dosyalar[i].dosya_adi, dosya1_adi) == 0) {
                dosya1 = &metadata.dosyalar[i];
            }
            if (strcmp(metadata.dosyalar[i].dosya_adi, dosya2_adi) == 0) {
                dosya2 = &metadata.dosyalar[i];
            }
        }
    }

    // Dosyaların varlığını kontrol et
    if (!dosya1) {
        printf("Hata: '%s' dosyası bulunamadı.\n", dosya1_adi);
        fclose(disk);
        
        // Loglama işlemi
        char log_mesaji[128];
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(FILE1_NOT_FOUND)", dosya1_adi, dosya2_adi);
        fs_log("DIFF", log_mesaji);
        return;
    }
    if (!dosya2) {
        printf("Hata: '%s' dosyası bulunamadı.\n", dosya2_adi);
        fclose(disk);
        
        // Loglama işlemi
        char log_mesaji[128];
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(FILE2_NOT_FOUND)", dosya1_adi, dosya2_adi);
        fs_log("DIFF", log_mesaji);
        return;
    }

    printf("Karşılaştırılıyor: '%s' (%d byte) vs '%s' (%d byte)\n", 
           dosya1_adi, dosya1->boyut, dosya2_adi, dosya2->boyut);

    // Önce boyutları karşılaştır
    if (dosya1->boyut != dosya2->boyut) {
        printf("Dosyalar farklı (boyut farkı: %d vs %d byte)\n", 
               dosya1->boyut, dosya2->boyut);
        fclose(disk);
        
        // Loglama işlemi
        char log_mesaji[128];
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(SIZE_DIFF)", dosya1_adi, dosya2_adi);
        fs_log("DIFF", log_mesaji);
        return;
    }

    // Boyutlar aynıysa içeriği byte-by-byte karşılaştır
    if (dosya1->boyut == 0) {
        printf("Dosyalar aynı (her ikisi de boş)\n");
        fclose(disk);
        
        // Loglama işlemi
        char log_mesaji[128];
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(SAME_EMPTY)", dosya1_adi, dosya2_adi);
        fs_log("DIFF", log_mesaji);
        return;
    }

    // Her iki dosyanın içeriğini oku
    char* buffer1 = malloc(dosya1->boyut);
    char* buffer2 = malloc(dosya2->boyut);
    
    if (!buffer1 || !buffer2) {
        printf("Hata: Bellek tahsis edilemedi.\n");
        if (buffer1) free(buffer1);
        if (buffer2) free(buffer2);
        fclose(disk);
        return;
    }

    // İlk dosyayı oku
    fseek(disk, dosya1->baslangic_adresi, SEEK_SET);
    fread(buffer1, sizeof(char), dosya1->boyut, disk);

    // İkinci dosyayı oku
    fseek(disk, dosya2->baslangic_adresi, SEEK_SET);
    fread(buffer2, sizeof(char), dosya2->boyut, disk);

    // Byte-by-byte karşılaştırma
    int fark_var = 0;
    for (int i = 0; i < dosya1->boyut; i++) {
        if (buffer1[i] != buffer2[i]) {
            fark_var = 1;
            printf("Dosyalar farklı (ilk fark %d. byte'ta: 0x%02x vs 0x%02x)\n", 
                   i + 1, (unsigned char)buffer1[i], (unsigned char)buffer2[i]);
            break;
        }
    }

    char log_mesaji[128];
    if (!fark_var) {
        printf("Dosyalar aynı\n");
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(SAME)", dosya1_adi, dosya2_adi);
    } else {
        snprintf(log_mesaji, sizeof(log_mesaji), "%s vs %s(DIFFERENT)", dosya1_adi, dosya2_adi);
    }

    free(buffer1);
    free(buffer2);
    fclose(disk);
    
    // Loglama işlemi
    fs_log("DIFF", log_mesaji);
}

void fs_defragment() {
    FILE* disk = fopen("disk.sim", "r+b");
    if (!disk) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    printf("Disk birleştirme işlemi başlıyor...\n");

    // Aktif dosyaları topla ve içeriklerini oku
    typedef struct {
        DosyaGirdisi dosya_info;
        char* icerik;
    } DosyaVerisi;

    DosyaVerisi aktif_dosyalar[MAX_FILES];
    int aktif_sayisi = 0;

    // Tüm aktif dosyaları bul ve içeriklerini oku
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            aktif_dosyalar[aktif_sayisi].dosya_info = metadata.dosyalar[i];
            
            // Dosya içeriğini oku
            if (metadata.dosyalar[i].boyut > 0) {
                aktif_dosyalar[aktif_sayisi].icerik = malloc(metadata.dosyalar[i].boyut);
                if (!aktif_dosyalar[aktif_sayisi].icerik) {
                    printf("Hata: Bellek tahsis edilemedi.\n");
                    fclose(disk);
                    return;
                }
                
                fseek(disk, metadata.dosyalar[i].baslangic_adresi, SEEK_SET);
                fread(aktif_dosyalar[aktif_sayisi].icerik, 1, metadata.dosyalar[i].boyut, disk);
            } else {
                aktif_dosyalar[aktif_sayisi].icerik = NULL;
            }
            
            aktif_sayisi++;
        }
    }

    printf("Toplam %d aktif dosya bulundu.\n", aktif_sayisi);

    // Dosyaları yeniden yazma - METADATA_SIZE'den başlayarak ardışık şekilde
    int yeni_adres = METADATA_SIZE;
    
    for (int i = 0; i < aktif_sayisi; i++) {
        // Yeni başlangıç adresini güncelle
        aktif_dosyalar[i].dosya_info.baslangic_adresi = yeni_adres;
        
        // Dosya içeriğini yeni konuma yaz
        if (aktif_dosyalar[i].dosya_info.boyut > 0) {
            fseek(disk, yeni_adres, SEEK_SET);
            fwrite(aktif_dosyalar[i].icerik, 1, aktif_dosyalar[i].dosya_info.boyut, disk);
            
            printf("'%s' dosyası %d adresine taşındı (%d byte)\n", 
                   aktif_dosyalar[i].dosya_info.dosya_adi, 
                   yeni_adres, 
                   aktif_dosyalar[i].dosya_info.boyut);
        }
        
        // Bir sonraki dosya için adresi hesapla
        yeni_adres += aktif_dosyalar[i].dosya_info.boyut;
        
        // Belleği temizle
        if (aktif_dosyalar[i].icerik) {
            free(aktif_dosyalar[i].icerik);
        }
    }

    // Metadata'yı güncelle - sadece aktif dosyaları koru
    Metadata yeni_metadata = {0};
    yeni_metadata.dosya_sayisi = aktif_sayisi;
    
    for (int i = 0; i < aktif_sayisi; i++) {
        yeni_metadata.dosyalar[i] = aktif_dosyalar[i].dosya_info;
    }

    // Yeni metadata'yı diske yaz
    fseek(disk, 0, SEEK_SET);
    fwrite(&yeni_metadata, sizeof(Metadata), 1, disk);
    fflush(disk);

    fclose(disk);
    
    printf("Disk birleştirme tamamlandı! %d dosya ardışık olarak yerleştirildi.\n", aktif_sayisi);
    printf("Toplam kullanılan alan: %d byte (metadata hariç)\n", yeni_adres - METADATA_SIZE);
    
    // Loglama işlemi
    fs_log("DEFRAGMENT", "disk.sim");
}

void fs_log(const char* islem_adi, const char* hedef) {
    // Log dosyasını ekleme modunda aç
    FILE* log_dosyasi = fopen("fs_log.txt", "a");
    if (!log_dosyasi) {
        printf("Uyarı: Log dosyası oluşturulamadı.\n");
        return;
    }

    // Basit zaman damgası için sistem saati kullan (alternatif: fake timestamp)
    // Burada basit bir sayaç veya sabit format kullanıyoruz
    static int log_sayaci = 1;
    
    // Log satırını yaz
    fprintf(log_dosyasi, "[LOG #%d] %s %s\n", log_sayaci++, islem_adi, hedef);
    fflush(log_dosyasi);
    
    fclose(log_dosyasi);
}

void fs_move(const char* kaynak, const char* hedef) {
    // Kaynak dosyanın var olup olmadığını kontrol et
    if (!fs_exists(kaynak)) {
        printf("Hata: Kaynak dosya '%s' bulunamadı.\n", kaynak);
        return;
    }

    // Hedef dosyanın var olup olmadığını kontrol et
    if (!fs_exists(hedef)) {
        printf("Hata: Hedef dosya '%s' bulunamadı.\n", hedef);
        return;
    }

    // Kaynak dosyanın boyutunu öğren
    int kaynak_boyut = fs_size(kaynak);
    if (kaynak_boyut <= 0) {
        printf("Hata: Kaynak dosya '%s' boş veya okunamıyor.\n", kaynak);
        return;
    }

    // Kaynak dosyanın içeriğini oku
    char* buffer = malloc(kaynak_boyut);
    if (!buffer) {
        printf("Hata: Bellek tahsis edilemedi.\n");
        return;
    }

    FILE* disk = fopen("disk.sim", "rb");
    if (!disk) {
        printf("Hata: disk.sim dosyası açılamadı.\n");
        free(buffer);
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    // Kaynak dosyayı bul ve içeriğini oku
    int kaynak_bulundu = 0;
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && 
            strcmp(metadata.dosyalar[i].dosya_adi, kaynak) == 0) {
            
            // Dosya içeriğini oku
            fseek(disk, metadata.dosyalar[i].baslangic_adresi, SEEK_SET);
            fread(buffer, sizeof(char), kaynak_boyut, disk);
            kaynak_bulundu = 1;
            break;
        }
    }
    fclose(disk);

    if (!kaynak_bulundu) {
        printf("Hata: Kaynak dosya '%s' okunamadı.\n", kaynak);
        free(buffer);
        return;
    }

    // Hedef dosyaya içeriği ekle
    fs_append(hedef, buffer, kaynak_boyut);

    // Kaynak dosyayı sil
    fs_delete(kaynak);

    // Belleği temizle
    free(buffer);

    printf("'%s' dosyası '%s' dosyasının sonuna taşındı (%d byte).\n", 
           kaynak, hedef, kaynak_boyut);

    // Loglama işlemi
    char log_mesaji[128];
    snprintf(log_mesaji, sizeof(log_mesaji), "%s->%s(%d bytes)", kaynak, hedef, kaynak_boyut);
    fs_log("MOVE", log_mesaji);
}
