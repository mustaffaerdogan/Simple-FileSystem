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

    // Debug: Print actual structure sizes
    printf("DEBUG: sizeof(Metadata) = %zu bytes\n", sizeof(Metadata));
    printf("DEBUG: METADATA_SIZE = %d bytes\n", METADATA_SIZE);
    printf("DEBUG: sizeof(DosyaGirdisi) = %zu bytes\n", sizeof(DosyaGirdisi));

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
            printf("DEBUG: Yazma konumu: %d\n", d->baslangic_adresi);
            printf("DEBUG: Yazılacak veri: '%s' (%d byte)\n", veri, boyut);
            
            fseek(disk, d->baslangic_adresi, SEEK_SET);
            long pos = ftell(disk);
            printf("DEBUG: Güncel dosya konumu: %ld\n", pos);
            
            size_t yazilan = fwrite(veri, sizeof(char), boyut, disk);
            printf("DEBUG: Yazılan byte sayısı: %zu / %d\n", yazilan, boyut);
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
            printf("DEBUG: Dosya başlangıç adresi: %d\n", d->baslangic_adresi);
            printf("DEBUG: Dosya boyutu: %d\n", d->boyut);
            printf("DEBUG: Okuma offset: %d, boyut: %d\n", offset, boyut);
            
            if (offset > d->boyut) {
                printf("Offset dosya boyutunu aşıyor.\n");
                fclose(disk);
                return;
            }

            if (offset + boyut > d->boyut)
                boyut = d->boyut - offset;

            printf("DEBUG: Düzeltilmiş okuma boyutu: %d\n", boyut);
            
            fseek(disk, d->baslangic_adresi + offset, SEEK_SET);
            long pos = ftell(disk);
            printf("DEBUG: Okuma konumu: %ld\n", pos);
            
            size_t okunan = fread(buffer, sizeof(char), boyut, disk);
            printf("DEBUG: Okunan byte sayısı: %zu\n", okunan);
            buffer[boyut] = '\0';

            printf("DEBUG: Ham buffer içeriği (hex): ");
            for (int j = 0; j < boyut; j++) {
                printf("%02x ", (unsigned char)buffer[j]);
            }
            printf("\n");

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
