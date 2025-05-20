#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

void fs_format() {
    FILE* disk = fopen("disk.sim", "r+b");
    if (disk == NULL) {
        // Dosya yoksa oluştur
        disk = fopen("disk.sim", "w+b");
        if (disk == NULL) {
            perror("disk.sim oluşturulamadı");
            return;
        }
    }

    // Metadata'yı sıfırla
    Metadata metadata;
    metadata.dosya_sayisi = 0;
    memset(metadata.dosyalar, 0, sizeof(metadata.dosyalar));

    // Diskin başına yaz (ilk 4 KB)
    fseek(disk, 0, SEEK_SET);
    fwrite(&metadata, sizeof(Metadata), 1, disk);

    printf("Disk formatlandı. Tüm dosyalar silindi.\n");

    fclose(disk);
}

void fs_create(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (disk == NULL) {
        printf("disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi && strcmp(metadata.dosyalar[i].dosya_adi, dosya_adi) == 0) {
            printf("Hata: '%s' adlı dosya zaten var.\n", dosya_adi);
            fclose(disk);
            return;
        }
    }

    if (metadata.dosya_sayisi >= MAX_FILES) {
        printf("Hata: Maksimum dosya sayısına ulaşıldı.\n");
        fclose(disk);
        return;
    }

    DosyaGirdisi yeni;
    strncpy(yeni.dosya_adi, dosya_adi, MAX_FILENAME_LENGTH);
    yeni.boyut = 0;
    yeni.aktif_mi = 1;

    // Kullanılacak ilk boş adresi hesapla
    int son_adres = METADATA_SIZE;
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi) {
            int bitis = metadata.dosyalar[i].baslangic_adresi + metadata.dosyalar[i].boyut;
            if (bitis > son_adres) {
                son_adres = bitis;
            }
        }
    }
    yeni.baslangic_adresi = son_adres;

    metadata.dosyalar[metadata.dosya_sayisi++] = yeni;

    fseek(disk, 0, SEEK_SET);
    fwrite(&metadata, sizeof(Metadata), 1, disk);
    fflush(disk);

    printf("'%s' adlı dosya oluşturuldu.\n", dosya_adi);
    fclose(disk);
}




void fs_ls() {
    FILE* disk = fopen("disk.sim", "rb");
    if (disk == NULL) {
        printf("disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fseek(disk, 0, SEEK_SET);
    fread(&metadata, sizeof(Metadata), 1, disk);
    fclose(disk);

    if (metadata.dosya_sayisi == 0) {
        printf("Hiç dosya bulunamadı.\n");
        return;
    }

    printf("Mevcut dosyalar:\n");
    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi == 1) {
            printf("- %s (%d byte)\n",
                   metadata.dosyalar[i].dosya_adi,
                   metadata.dosyalar[i].boyut);
        }
    }
}

void fs_delete(const char* dosya_adi) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (disk == NULL) {
        printf("disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fseek(disk, 0, SEEK_SET);
    fread(&metadata, sizeof(Metadata), 1, disk);

    int bulundu = 0;

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        if (metadata.dosyalar[i].aktif_mi == 1 &&
            strcmp(metadata.dosyalar[i].dosya_adi, dosya_adi) == 0) {

            metadata.dosyalar[i].aktif_mi = 0; // Silindi olarak işaretle
            bulundu = 1;
            break;
        }
    }

    if (bulundu) {
        fseek(disk, 0, SEEK_SET);
        fwrite(&metadata, sizeof(Metadata), 1, disk);
        printf("'%s' adlı dosya silindi.\n", dosya_adi);
    } else {
        printf("Hata: '%s' adlı dosya bulunamadı.\n", dosya_adi);
    }

    fclose(disk);
}

void fs_write(const char* dosya_adi, const char* veri, int boyut) {
    FILE* disk = fopen("disk.sim", "r+b");
    if (disk == NULL) {
        printf("disk.sim dosyası açılamadı.\n");
        return;
    }

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

            printf("'%s' adlı dosyaya %d byte veri yazıldı.\n", dosya_adi, boyut);
            fclose(disk);
            return;
        }
    }

    printf("Hata: '%s' adlı dosya bulunamadı.\n", dosya_adi);
    fclose(disk);
}



void fs_read(const char* dosya_adi, int offset, int boyut, char* buffer) {
    FILE* disk = fopen("disk.sim", "rb");
    if (disk == NULL) {
        printf("disk.sim dosyası açılamadı.\n");
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, disk);

    for (int i = 0; i < metadata.dosya_sayisi; i++) {
        DosyaGirdisi* d = &metadata.dosyalar[i];
        if (d->aktif_mi && strcmp(d->dosya_adi, dosya_adi) == 0) {

            if (offset > d->boyut) {
                printf("Hata: Offset dosya boyutunu aşıyor.\n");
                fclose(disk);
                return;
            }

            if (offset + boyut > d->boyut) {
                boyut = d->boyut - offset;  // Kalan kadar oku
            }

            fseek(disk, d->baslangic_adresi + offset, SEEK_SET);
            fread(buffer, sizeof(char), boyut, disk);
            buffer[boyut] = '\0';  // null karakter ile sonlandır

            // 🔍 Debug: Okunan veriyi hex formatında yazdır
            printf("Okunan veri (hex): ");
            for (int j = 0; j < boyut; j++) {
                printf("%02X ", (unsigned char)buffer[j]);
            }
            printf("\n");

            // 🟢 Okunan string çıktısı
            printf("Okunan veri (string): %s\n", buffer);

            fclose(disk);
            return;
        }
    }

    printf("Hata: '%s' adlı dosya bulunamadı.\n", dosya_adi);
    fclose(disk);
}



