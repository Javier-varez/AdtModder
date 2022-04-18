#ifndef FILEIO_H_
#define FILEIO_H_

#include <cstdint>
#include <vector>

#include "ditto/result.h"
#include "ditto/span.h"

class File {
public:
  enum class Error {
    Unknown,
    IoError,
    FileAlreadyExists,
    FileDoesNotExist,
    InvalidPermissions,
  };

  static Ditto::Result<File, Error> Create(const char *name);
  static Ditto::Result<File, Error> Open(const char *name);

  Ditto::Result<std::vector<uint8_t>, Error> ReadAll();
  Ditto::Result<void, Error> Write(Ditto::span<uint8_t> buffer);

  Ditto::Result<size_t, Error> SetOffset(size_t offset);

  Ditto::Result<size_t, Error> Size() const;

  File(const File &) = delete;
  File &operator=(const File &) = delete;

  File(File &&);
  File &operator=(File &&);

  ~File();

private:
  int m_fd = -1;

  File(int fd) : m_fd(fd) {}

  static File::Error ErrorFromErrno(int error_var);
};

#endif // FILEIO_H_
