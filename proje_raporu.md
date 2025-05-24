# Basit Dosya Sistemi Simülatörü (SimpleFS) - Teknik Proje Raporu

---

## 📘 Türkçe Bölüm

### 1. Proje Adı ve Giriş

**Proje Adı:** Basit Dosya Sistemi Simülatörü (SimpleFS)

Bu proje, C programlama dili kullanılarak geliştirilmiş eğitsel bir dosya sistemi simülatörüdür. Projenin temel amacı, gerçek dosya sistemlerinin iç yapısını ve çalışma mantığını anlamak, sistem çağrıları ile düşük seviyeli dosya operasyonlarını öğrenmek ve dosya sistemi algoritmaları hakkında pratik deneyim kazanmaktır.

SimpleFS, UNIX-tarzı dosya operasyonlarını tek bir sanal disk dosyası (`disk.sim`) içerisinde simüle eder. Bu yaklaşım, öğrencilerin ve geliştiricilerin dosya sistemi kavramlarını güvenli bir ortamda deneyimlemelerini sağlar.

**Neden Bu Proje Geliştirildi:**
- İşletim sistemi derslerinde dosya sistemi konularının pratik uygulaması
- Sistem çağrıları ve düşük seviyeli programlama becerilerinin geliştirilmesi
- Dosya sistemi metadata yönetimi ve veri organizasyonu anlayışının artırılması
- C dilinde modüler programlama ve hata yönetimi pratikleri

### 2. Kullanılan Teknolojiler

- **Programlama Dili:** C (ANSI C standardı)
- **Sistem Çağrıları:** POSIX dosya operasyonları (`fopen`, `fwrite`, `fread`, `fseek`, `fclose`)
- **Derleme Aracı:** GCC derleyici
- **Build Sistemi:** GNU Make (Makefile)
- **Veri Yapıları:** Statik diziler, struct yapıları
- **Bellek Yönetimi:** Stack-based allocation (dinamik bellek yönetimi kullanılmamış)
- **Platform:** Cross-platform (Linux, macOS, Windows)

### 3. Proje Dosya Yapısı

```
Simple-FileSystem/
├── Main.c              # Ana program ve kullanıcı arayüzü (225 satır)
├── fs.c                # Dosya sistemi fonksiyonları (989 satır)
├── fs.h                # Header dosyası ve veri yapıları (53 satır)
├── Makefile            # Derleme yapılandırması (17 satır)
├── disk.sim            # 1MB sanal disk dosyası (runtime'da oluşturulur)
├── README.md           # Proje dokümantasyonu
└── proje_raporu.md     # Teknik proje raporu
```

**Dosya Detayları:**
- **Main.c:** Kullanıcı arayüzü, menü sistemi ve ana program döngüsü
- **fs.c:** Tüm dosya sistemi operasyonlarının implementasyonu
- **fs.h:** Veri yapıları, sabitler ve fonksiyon prototipleri
- **Makefile:** Otomatik derleme kuralları ve bağımlılık yönetimi

### 4. Uygulanan Fonksiyonlar

#### Temel Dosya Operasyonları
- **`fs_create(filename)`** - Yeni dosya oluşturma ve metadata kaydı
- **`fs_delete(filename)`** - Dosya silme ve alan serbest bırakma
- **`fs_write(filename, data)`** - Dosyaya veri yazma operasyonu
- **`fs_read(filename)`** - Dosya içeriği okuma ve görüntüleme
- **`fs_ls()`** - Mevcut dosyaların listelenmesi

#### Dosya Sistemi Yönetimi
- **`fs_format()`** - Sanal disk formatlama ve metadata sıfırlama
- **`fs_exists(filename)`** - Dosya varlık kontrolü
- **`fs_size(filename)`** - Dosya boyutu hesaplama
- **`fs_rename(old, new)`** - Dosya yeniden adlandırma

#### Gelişmiş Operasyonlar
- **`fs_append(filename, data)`** - Dosya sonuna veri ekleme
- **`fs_truncate(filename, size)`** - Dosya boyutu kısaltma
- **`fs_copy(source, dest)`** - Dosya kopyalama operasyonu
- **`fs_move(source, dest)`** - Dosya taşıma (kopyala + sil)

#### Sistem Operasyonları
- **`fs_backup()`** - Dosya sistemi yedekleme
- **`fs_restore()`** - Yedekten geri yükleme
- **`fs_diff(file1, file2)`** - İki dosya karşılaştırma
- **`fs_log()`** - İşlem geçmişi görüntüleme
- **`fs_cat(filename)`** - Dosya içeriğini detaylı görüntüleme

#### Bakım Operasyonları
- **`fs_defragment()`** - Disk birleştirme ve optimizasyon
- **`fs_check_integrity()`** - Dosya sistemi bütünlük kontrolü

### 5. Test Örnekleri

#### fs_create Testi
```bash
Choice: 2
Enter filename: test.txt
File 'test.txt' created successfully.

# Negatif Test - Duplicate File
Choice: 2
Enter filename: test.txt
Error: File 'test.txt' already exists.
```

#### fs_write ve fs_read Testi
```bash
Choice: 4
Enter filename: test.txt
Enter data: Merhaba Dünya! Bu bir test dosyasıdır.
Data written to 'test.txt' successfully.

Choice: 5
Enter filename: test.txt
Content of 'test.txt':
Merhaba Dünya! Bu bir test dosyasıdır.
```

#### fs_ls Testi
```bash
Choice: 6
Files in filesystem:
1. test.txt (45 bytes)
2. config.dat (128 bytes)
3. sample.log (256 bytes)
Total files: 3
```

#### fs_copy ve fs_move Testi
```bash
Choice: 12
Enter source filename: test.txt
Enter destination filename: backup.txt
File copied successfully.

Choice: 20
Enter source filename: backup.txt
Enter destination filename: archive.txt
File moved successfully.
```

#### Hata Durumları
```bash
# Varolmayan dosya okuma
Choice: 5
Enter filename: nonexistent.txt
Error: File 'nonexistent.txt' not found.

# Disk doluluk kontrolü
Choice: 4
Enter filename: large.dat
Enter data: [Very large data...]
Error: Not enough space on disk.
```

### 6. Karşılaşılan Problemler ve Çözüm Yöntemleri

#### Problem 1: Metadata Overflow
**Sorun:** İlk implementasyonda metadata alanı 4096 byte olarak planlanmıştı, ancak 100 dosya için 5636 byte gerekli oldu.
```c
// Yanlış hesaplama
#define METADATA_SIZE 4096  // Yetersiz alan
```
**Çözüm:** Metadata alanı 6144 byte'a genişletildi ve dinamik alan kontrolü eklendi.
```c
#define METADATA_SIZE 6144  // Genişletilmiş alan
#define MAX_FILES 100
```

#### Problem 2: Silent fwrite Failure
**Sorun:** `fwrite()` fonksiyonu bazı durumlarda hata döndürmüyordu ancak veri yazılmıyordu.
```c
// Hatalı yaklaşım
fwrite(data, 1, size, file);
// Return değeri kontrol edilmiyor
```
**Çözüm:** Return değeri kontrolü ve `fflush()` eklendi.
```c
size_t written = fwrite(data, 1, size, file);
if (written != size) {
    printf("Error: Write operation failed\n");
    return -1;
}
fflush(file);
```

#### Problem 3: fs_move Dizin Desteği
**Sorun:** `fs_move` fonksiyonu dizin yollarını desteklemiyordu.
**Çözüm:** Filename validation ve path parsing eklendi.
```c
int validate_filename(const char* filename) {
    if (strchr(filename, '/') || strchr(filename, '\\')) {
        printf("Error: Directory paths not supported\n");
        return 0;
    }
    return 1;
}
```

#### Problem 4: Memory Corruption
**Sorun:** Büyük dosyalar yazılırken buffer overflow oluşuyordu.
**Çözüm:** Buffer size kontrolü ve güvenli string işlemleri.
```c
if (strlen(data) > MAX_FILE_SIZE) {
    printf("Error: File too large (max %d bytes)\n", MAX_FILE_SIZE);
    return -1;
}
```

### 7. Projenin Çalışma Mantığı

#### Disk Yapısı
```
disk.sim (1MB = 1,048,576 bytes)
├── Metadata Bölgesi [0 - 6143 bytes]
│   ├── File Entry 1 (61 bytes)
│   ├── File Entry 2 (61 bytes)
│   └── ... (100 dosyaya kadar)
└── Data Bölgesi [6144 - 1,048,575 bytes]
    ├── Block 0 (512 bytes)
    ├── Block 1 (512 bytes)
    └── ... (2036 blok)
```

#### Metadata Güncelleme Süreci
1. Dosya oluşturulduğunda metadata'da yeni entry eklenir
2. File entry: filename (50 byte) + size (4 byte) + start_block (4 byte) + in_use (1 byte)
3. Boş blok aranır ve dosya verisi yazılır
4. Metadata'daki size ve start_block bilgileri güncellenir

#### Dosya Adresleme Mantığı
```c
// Dosya başlangıç adresi hesaplama
offset = METADATA_SIZE + (start_block * BLOCK_SIZE);
fseek(disk_file, offset, SEEK_SET);
```

#### Veri Bölgesi Yönetimi
- Her dosya 512 byte'lık bloklarda saklanır
- Fragmentation minimize edilmeye çalışılır
- `fs_defragment()` ile dosyalar yeniden düzenlenir

### 8. Sonuç ve Değerlendirme

Bu proje, dosya sistemi konseptlerinin pratik uygulaması açısından oldukça başarılı olmuştur. Temel dosya operasyonlarından gelişmiş sistem fonksiyonlarına kadar 20 farklı komutun implementasyonu tamamlanmıştır.

**Öğrenilen Konular:**
- Düşük seviyeli dosya operasyonları ve sistem çağrıları
- Metadata yönetimi ve veri organizasyonu
- Buffer yönetimi ve bellek güvenliği
- Hata yakalama ve exception handling
- Modüler programlama ve kod organizasyonu

**Proje Çıktıları:**
- Tam fonksiyonel dosya sistemi simülatörü
- 1MB sanal disk üzerinde çoklu dosya desteği
- İnteraktif CLI arayüzü
- Kapsamlı hata yönetimi sistemi
- Backup/restore ve bütünlük kontrol özellikleri

**Gelecek Geliştirmeler:**
- Dizin (directory) desteği eklenmesi
- Dosya izin sistemi implementasyonu
- Asenkron I/O operasyonları
- GUI arayüzü geliştirilmesi

### 9. Kaynakça

1. Tanenbaum, A. S., & Bos, H. (2014). *Modern Operating Systems*. Pearson.
2. Stevens, W. R., & Rago, S. A. (2013). *Advanced Programming in the UNIX Environment*. Addison-Wesley.
3. Kernighan, B. W., & Ritchie, D. M. (1988). *The C Programming Language*. Prentice Hall.
4. POSIX.1-2017 Documentation. IEEE Computer Society.
5. GNU Make Manual. Free Software Foundation.

---

## 📘 English Section

### 1. Project Name and Introduction

**Project Name:** Simple File System Simulator (SimpleFS)

This project is an educational file system simulator developed using the C programming language. The primary purpose of this project is to understand the internal structure and working principles of real file systems, learn low-level file operations using system calls, and gain practical experience with file system algorithms.

SimpleFS simulates UNIX-style file operations within a single virtual disk file (`disk.sim`). This approach allows students and developers to experience file system concepts in a safe environment.

**Why This Project Was Developed:**
- Practical application of file system topics in operating systems courses
- Development of system call and low-level programming skills
- Enhanced understanding of file system metadata management and data organization
- Modular programming and error management practices in C

### 2. Technologies Used

- **Programming Language:** C (ANSI C standard)
- **System Calls:** POSIX file operations (`fopen`, `fwrite`, `fread`, `fseek`, `fclose`)
- **Compiler:** GCC compiler
- **Build System:** GNU Make (Makefile)
- **Data Structures:** Static arrays, struct structures
- **Memory Management:** Stack-based allocation (no dynamic memory management)
- **Platform:** Cross-platform (Linux, macOS, Windows)

### 3. Project File Structure

```
Simple-FileSystem/
├── Main.c              # Main program and user interface (225 lines)
├── fs.c                # File system functions (989 lines)
├── fs.h                # Header file and data structures (53 lines)
├── Makefile            # Build configuration (17 lines)
├── disk.sim            # 1MB virtual disk file (created at runtime)
├── README.md           # Project documentation
└── proje_raporu.md     # Technical project report
```

**File Details:**
- **Main.c:** User interface, menu system, and main program loop
- **fs.c:** Implementation of all file system operations
- **fs.h:** Data structures, constants, and function prototypes
- **Makefile:** Automatic compilation rules and dependency management

### 4. Implemented Functions

#### Basic File Operations
- **`fs_create(filename)`** - Create new file and record metadata
- **`fs_delete(filename)`** - Delete file and free space
- **`fs_write(filename, data)`** - Write data to file operation
- **`fs_read(filename)`** - Read and display file content
- **`fs_ls()`** - List existing files

#### File System Management
- **`fs_format()`** - Format virtual disk and reset metadata
- **`fs_exists(filename)`** - Check file existence
- **`fs_size(filename)`** - Calculate file size
- **`fs_rename(old, new)`** - Rename file

#### Advanced Operations
- **`fs_append(filename, data)`** - Append data to file
- **`fs_truncate(filename, size)`** - Truncate file size
- **`fs_copy(source, dest)`** - File copy operation
- **`fs_move(source, dest)`** - Move file (copy + delete)

#### System Operations
- **`fs_backup()`** - Backup file system
- **`fs_restore()`** - Restore from backup
- **`fs_diff(file1, file2)`** - Compare two files
- **`fs_log()`** - Display operation history
- **`fs_cat(filename)`** - Display file content in detail

#### Maintenance Operations
- **`fs_defragment()`** - Disk defragmentation and optimization
- **`fs_check_integrity()`** - File system integrity check

### 5. Test Examples

#### fs_create Test
```bash
Choice: 2
Enter filename: test.txt
File 'test.txt' created successfully.

# Negative Test - Duplicate File
Choice: 2
Enter filename: test.txt
Error: File 'test.txt' already exists.
```

#### fs_write and fs_read Test
```bash
Choice: 4
Enter filename: test.txt
Enter data: Hello World! This is a test file.
Data written to 'test.txt' successfully.

Choice: 5
Enter filename: test.txt
Content of 'test.txt':
Hello World! This is a test file.
```

#### fs_ls Test
```bash
Choice: 6
Files in filesystem:
1. test.txt (45 bytes)
2. config.dat (128 bytes)
3. sample.log (256 bytes)
Total files: 3
```

#### fs_copy and fs_move Test
```bash
Choice: 12
Enter source filename: test.txt
Enter destination filename: backup.txt
File copied successfully.

Choice: 20
Enter source filename: backup.txt
Enter destination filename: archive.txt
File moved successfully.
```

#### Error Cases
```bash
# Reading non-existent file
Choice: 5
Enter filename: nonexistent.txt
Error: File 'nonexistent.txt' not found.

# Disk space check
Choice: 4
Enter filename: large.dat
Enter data: [Very large data...]
Error: Not enough space on disk.
```

### 6. Problems Encountered and Solution Methods

#### Problem 1: Metadata Overflow
**Issue:** In the initial implementation, metadata area was planned as 4096 bytes, but 5636 bytes were required for 100 files.
```c
// Incorrect calculation
#define METADATA_SIZE 4096  // Insufficient space
```
**Solution:** Metadata area was expanded to 6144 bytes and dynamic space control was added.
```c
#define METADATA_SIZE 6144  // Expanded space
#define MAX_FILES 100
```

#### Problem 2: Silent fwrite Failure
**Issue:** `fwrite()` function sometimes didn't return error but data wasn't written.
```c
// Incorrect approach
fwrite(data, 1, size, file);
// Return value not checked
```
**Solution:** Return value check and `fflush()` added.
```c
size_t written = fwrite(data, 1, size, file);
if (written != size) {
    printf("Error: Write operation failed\n");
    return -1;
}
fflush(file);
```

#### Problem 3: fs_move Directory Support
**Issue:** `fs_move` function didn't support directory paths.
**Solution:** Filename validation and path parsing added.
```c
int validate_filename(const char* filename) {
    if (strchr(filename, '/') || strchr(filename, '\\')) {
        printf("Error: Directory paths not supported\n");
        return 0;
    }
    return 1;
}
```

#### Problem 4: Memory Corruption
**Issue:** Buffer overflow occurred when writing large files.
**Solution:** Buffer size control and safe string operations.
```c
if (strlen(data) > MAX_FILE_SIZE) {
    printf("Error: File too large (max %d bytes)\n", MAX_FILE_SIZE);
    return -1;
}
```

### 7. Project Working Logic

#### Disk Structure
```
disk.sim (1MB = 1,048,576 bytes)
├── Metadata Region [0 - 6143 bytes]
│   ├── File Entry 1 (61 bytes)
│   ├── File Entry 2 (61 bytes)
│   └── ... (up to 100 files)
└── Data Region [6144 - 1,048,575 bytes]
    ├── Block 0 (512 bytes)
    ├── Block 1 (512 bytes)
    └── ... (2036 blocks)
```

#### Metadata Update Process
1. When a file is created, a new entry is added to metadata
2. File entry: filename (50 bytes) + size (4 bytes) + start_block (4 bytes) + in_use (1 byte)
3. Empty block is searched and file data is written
4. Metadata'daki size ve start_block bilgileri güncellenir

#### File Addressing Logic
```c
// Calculate file start address
offset = METADATA_SIZE + (start_block * BLOCK_SIZE);
fseek(disk_file, offset, SEEK_SET);
```

#### Data Region Management
- Each file is stored in 512-byte blocks
- Fragmentation is minimized
- Files are reorganized with `fs_defragment()`

### 8. Conclusion and Evaluation

This project has been quite successful in terms of practical application of file system concepts. Implementation of 20 different commands from basic file operations to advanced system functions has been completed.

**Topics Learned:**
- Low-level file operations and system calls
- Metadata management and data organization
- Buffer management and memory safety
- Error handling and exception handling
- Modular programming and code organization

**Project Outputs:**
- Fully functional file system simulator
- Multiple file support on 1MB virtual disk
- Interactive CLI interface
- Comprehensive error management system
- Backup/restore and integrity check features

**Future Developments:**
- Adding directory support
- Implementing file permission system
- Asynchronous I/O operations
- GUI interface development

### 9. References

1. Tanenbaum, A. S., & Bos, H. (2014). *Modern Operating Systems*. Pearson.
2. Stevens, W. R., & Rago, S. A. (2013). *Advanced Programming in the UNIX Environment*. Addison-Wesley.
3. Kernighan, B. W., & Ritchie, D. M. (1988). *The C Programming Language*. Prentice Hall.
4. POSIX.1-2017 Documentation. IEEE Computer Society.
5. GNU Make Manual. Free Software Foundation. 