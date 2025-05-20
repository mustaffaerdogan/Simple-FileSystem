#ifndef FS_H
#define FS_H

#define MAX_FILES 128             // Maksimum dosya sayısı
#define MAX_FILENAME_LENGTH 32    // Maksimum dosya adı uzunluğu
#define BLOCK_SIZE 512            // Veri bloğu boyutu (byte)
#define DISK_SIZE (1024 * 1024)   // Sanal disk boyutu (1 MB)
#define METADATA_SIZE (4 * 1024)  // Metadata alanı boyutu (4 KB)

// Dosya bilgilerini tutan yapı
typedef struct {
    char dosya_adi[MAX_FILENAME_LENGTH];
    int baslangic_adresi;
    int boyut;
    int aktif_mi; // 1: aktif, 0: silinmiş
} DosyaGirdisi;

// Dosya sistemi metadata yapısı
typedef struct {
    int dosya_sayisi;
    DosyaGirdisi dosyalar[MAX_FILES];
} Metadata;

#endif
