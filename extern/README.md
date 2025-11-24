# External Dependencies

This directory is for third-party dependencies that are not available via system package managers.

## SQLite3

### Windows

1. Download SQLite3 from https://www.sqlite.org/download.html
2. Extract the files:
   - Place `sqlite3.h` in `extern/sqlite/include/`
   - Place `sqlite3.lib` (or `sqlite3.dll`) in `extern/sqlite/lib/`

Or use vcpkg:
```bash
vcpkg install sqlite3
```

### Linux

SQLite3 is typically available via package manager:
```bash
sudo apt-get install libsqlite3-dev  # Debian/Ubuntu
sudo yum install sqlite-devel        # RHEL/CentOS
```

## zstd

### Windows

1. Download zstd from https://github.com/facebook/zstd/releases
2. Extract the files:
   - Place `zstd.h` in `extern/zstd/include/`
   - Place `zstd.lib` (or `zstd.dll`) in `extern/zstd/lib/`

Or use vcpkg:
```bash
vcpkg install zstd
```

### Linux

zstd is typically available via package manager:
```bash
sudo apt-get install libzstd-dev  # Debian/Ubuntu
sudo yum install libzstd-devel    # RHEL/CentOS
```

