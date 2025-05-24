#ifndef FS_H
#define FS_H

#define MAX_FILES 128
#define MAX_FILENAME_LENGTH 32
#define METADATA_SIZE 6144
#define BLOCK_SIZE 512

typedef struct {
    char dosya_adi[MAX_FILENAME_LENGTH];
    int boyut;
    int baslangic_adresi;
    int aktif_mi;
} DosyaGirdisi;

typedef struct {
    int dosya_sayisi;
    DosyaGirdisi dosyalar[MAX_FILES];
} Metadata;

void fs_format();
void fs_create(const char* dosya_adi);
void fs_write(const char* dosya_adi, const char* veri, int boyut);
void fs_read(const char* dosya_adi, int offset, int boyut, char* buffer);
void fs_ls();

// Yeni dosya yönetim fonksiyonları
void fs_delete(const char* dosya_adi);
int fs_exists(const char* dosya_adi);
int fs_size(const char* dosya_adi);
void fs_rename(const char* eski_ad, const char* yeni_ad);

// Veri manipülasyon fonksiyonları
void fs_append(const char* dosya_adi, const char* veri, int boyut);
void fs_truncate(const char* dosya_adi, int yeni_boyut);
void fs_copy(const char* kaynak_ad, const char* hedef_ad);

// Sistem seviyesi fonksiyonlar
void fs_backup(const char* yedek_dosya_adi);
void fs_restore(const char* yedek_dosya_adi);
void fs_check_integrity();

#endif
