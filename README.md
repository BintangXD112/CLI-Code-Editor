# Basic CLI Code Editor (C)

Editor teks sederhana berbasis terminal/CLI, ditulis dalam bahasa C, terinspirasi dari editor minimalis seperti `kilo`.

## Fitur
- Buka file teks
- Navigasi dengan tombol panah
- Edit (insert karakter)
- Simpan file (Ctrl+S)
- Keluar (Ctrl+Q)
- Modular: kode dipisah per fitur agar mudah dikembangkan

## Struktur File
```
.
├── basic_editor.c   # Entry point utama
├── util.c/h         # Fungsi utilitas terminal & error
├── editor.c/h       # Struktur data & inisialisasi editor
├── input.c/h        # Input keyboard & navigasi
├── screen.c/h       # Tampilan layar
├── fileio.c/h       # File I/O (buka, simpan, append row)
└── README.md        # Dokumentasi
```

## Kompilasi
Pastikan sudah terinstall GCC dan berjalan di Linux/Unix.

```sh
gcc -o basic_editor basic_editor.c util.c editor.c input.c screen.c fileio.c
```

## Menjalankan
```sh
./basic_editor namafile.txt
```

- Navigasi: panah atas/bawah/kiri/kanan
- Simpan: Ctrl+S
- Keluar: Ctrl+Q

## Kontribusi / Extend Fitur
- Tambahkan fitur baru di file yang sesuai (misal: input.c untuk keyboard, screen.c untuk tampilan, dst)
- Untuk fitur besar, buat file baru dan include di basic_editor.c
- Pull request & diskusi sangat diterima!

## Catatan
- Hanya berjalan di Linux/Unix (menggunakan termios, ioctl, dsb)
- Belum ada fitur hapus karakter, baris baru, dsb (silakan extend sendiri)

---

Terinspirasi dari [kilo editor](https://github.com/antirez/kilo) oleh Salvatore Sanfilippo. 