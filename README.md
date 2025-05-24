# Simple-FileSystem

A UNIX-style simple file system simulator built with C, using only system calls (open, read, write, etc.) and a single disk file (disk.sim) to emulate file operations like create, read, write, delete, and more.

# 📘 Proje Açıklaması (Türkçe)

## 🎯 Proje Amacı ve Temel Mantık

Simple-FileSystem, C programlama dili kullanılarak geliştirilmiş bir dosya sistemi simülatörüdür. Bu proje, gerçek bir dosya sisteminin nasıl çalıştığını anlamak ve sistem çağrıları (system calls) kullanarak dosya operasyonlarını simüle etmek amacıyla tasarlanmıştır. 

Proje, 1MB boyutunda sanal bir disk (disk.sim) kullanarak UNIX-tarzı dosya operasyonlarını gerçekleştirir. Tüm dosya işlemleri yalnızca sistem çağrıları (open, read, write, lseek, close) kullanılarak yapılır ve herhangi bir üst düzey dosya işleme kütüphanesi kullanılmaz.

## 📁 Dosya Yapısı

```
Simple-FileSystem/
├── Main.c          # Ana program dosyası ve kullanıcı arayüzü
├── fs.c            # Dosya sistemi fonksiyonlarının implementasyonu
├── fs.h            # Header dosyası ve fonksiyon tanımları
├── Makefile        # Derleme dosyası
├── disk.sim        # 1MB sanal disk dosyası (çalışma zamanında oluşturulur)
└── README.md       # Proje dokümantasyonu
```

## 🔨 Derleme Talimatları

Projeyi derlemek için terminal/command prompt'ta aşağıdaki komutu çalıştırın:

```bash
make
```

Bu komut `SimpleFS` adında çalıştırılabilir bir dosya oluşturacaktır.

## 🚀 Simülatörü Çalıştırma

Derleme işleminden sonra, programı çalıştırmak için:

```bash
./SimpleFS
```

## 📋 Örnek Menü ve Kullanım

Program çalıştığında aşağıdaki gibi bir menü görüntülenecektir:

```
=== Simple File System Simulator ===
1.  Format disk
2.  Create file
3.  Delete file
4.  Write to file
5.  Read file
6.  List files
7.  Rename file
8.  Check if file exists
9.  Get file size
10. Append to file
11. Truncate file
12. Copy file
13. Backup filesystem
14. Restore filesystem
15. Check integrity
16. Display file content
17. Compare files
18. Show log
19. Defragment disk
20. Move file
0.  Exit

Choice: 
```

## ⚙️ Teknik Özellikler

- **Sanal Disk**: 1MB boyutunda tek bir disk dosyası (disk.sim)
- **Sistem Çağrıları**: Yalnızca düşük seviye sistem çağrıları kullanılır
- **Dosya Sistemi**: UNIX-tarzı basit dosya sistemi mimarisi
- **Bellek Yönetimi**: Verimli bellek kullanımı ve hata kontrolü

## 🛠️ Implement Edilen Fonksiyonlar

- `fs_create` - Dosya oluşturma
- `fs_delete` - Dosya silme
- `fs_write` - Dosyaya yazma
- `fs_read` - Dosya okuma
- `fs_ls` - Dosya listeleme
- `fs_format` - Disk formatlama
- `fs_rename` - Dosya yeniden adlandırma
- `fs_exists` - Dosya varlık kontrolü
- `fs_size` - Dosya boyutu öğrenme
- `fs_append` - Dosya sonuna ekleme
- `fs_truncate` - Dosya kısaltma
- `fs_copy` - Dosya kopyalama
- `fs_backup` - Sistem yedekleme
- `fs_restore` - Sistem geri yükleme
- `fs_check_integrity` - Bütünlük kontrolü
- `fs_cat` - Dosya içeriği görüntüleme
- `fs_diff` - Dosya karşılaştırma
- `fs_log` - İşlem günlüğü
- `fs_defragment` - Disk birleştirme
- `fs_move` - Dosya taşıma

# 📘 Project Description (English)

## 🎯 Project Purpose and Basic Logic

Simple-FileSystem is a file system simulator developed using the C programming language. This project is designed to understand how a real file system works and to simulate file operations using system calls.

The project performs UNIX-style file operations using a 1MB virtual disk (disk.sim). All file operations are performed using only system calls (open, read, write, lseek, close) without using any high-level file handling libraries.

## 📁 File Structure

```
Simple-FileSystem/
├── Main.c          # Main program file and user interface
├── fs.c            # Implementation of file system functions
├── fs.h            # Header file and function definitions
├── Makefile        # Build configuration file
├── disk.sim        # 1MB virtual disk file (created at runtime)
└── README.md       # Project documentation
```

## 🔨 Compilation Instructions

To compile the project, run the following command in your terminal/command prompt:

```bash
make
```

This command will create an executable file named `SimpleFS`.

## 🚀 Running the Simulator

After compilation, to run the program:

```bash
./SimpleFS
```

## 📋 Sample Menu and Usage

When the program runs, a menu like the following will be displayed:

```
=== Simple File System Simulator ===
1.  Format disk
2.  Create file
3.  Delete file
4.  Write to file
5.  Read file
6.  List files
7.  Rename file
8.  Check if file exists
9.  Get file size
10. Append to file
11. Truncate file
12. Copy file
13. Backup filesystem
14. Restore filesystem
15. Check integrity
16. Display file content
17. Compare files
18. Show log
19. Defragment disk
20. Move file
0.  Exit

Choice: 
```

## ⚙️ Technical Features

- **Virtual Disk**: Single 1MB disk file (disk.sim)
- **System Calls**: Only low-level system calls are used
- **File System**: UNIX-style simple file system architecture
- **Memory Management**: Efficient memory usage and error handling

## 🛠️ Implemented Functions

- `fs_create` - File creation
- `fs_delete` - File deletion
- `fs_write` - Writing to file
- `fs_read` - File reading
- `fs_ls` - File listing
- `fs_format` - Disk formatting
- `fs_rename` - File renaming
- `fs_exists` - File existence check
- `fs_size` - Get file size
- `fs_append` - Append to file
- `fs_truncate` - File truncation
- `fs_copy` - File copying
- `fs_backup` - System backup
- `fs_restore` - System restoration
- `fs_check_integrity` - Integrity check
- `fs_cat` - Display file content
- `fs_diff` - File comparison
- `fs_log` - Operation logging
- `fs_defragment` - Disk defragmentation
- `fs_move` - File moving 