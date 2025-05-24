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

#endif
