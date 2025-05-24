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
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            printf("- %s (%d byte)\n", metadata.dosyalar[i].dosya_adi, metadata.dosyalar[i].boyut);
        }
    }
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
            return 1; // Dosya mevcut ve aktif
        }
    }

    fclose(disk);
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
            return boyut; // Dosya boyutunu döndür
        }
    }

    fclose(disk);
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
}
