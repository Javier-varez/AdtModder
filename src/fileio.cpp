#include "fileio.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fmt/core.h"

using Ditto::Result;

File::Error File::ErrorFromErrno(int error_var) {
  switch (error_var) {
  case EPERM:
    return Error::InvalidPermissions;
  case EACCES:
    return Error::InvalidPermissions;
  case EEXIST:
    return Error::FileAlreadyExists;
  case ENOENT:
    return Error::FileDoesNotExist;
  case EIO:
    return Error::IoError;
  default:
    fmt::print("Unknown error: ({}) {}\n", error_var, strerror(error_var));
    return Error::Unknown;
  }
}
Result<File, File::Error> File::Create(const char *name) {
  int fd = open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH);
  if (fd < 0) {
    return File::ErrorFromErrno(errno);
  }
  return File{fd};
}

Result<File, File::Error> File::Open(const char *name) {
  int fd = open(name, O_RDONLY);
  if (fd < 0) {
    return File::ErrorFromErrno(errno);
  }
  return File{fd};
}

Result<size_t, File::Error> File::Size() const {
  struct stat stats;
  int status = fstat(m_fd, &stats);
  if (status < 0) {
    const auto error = ErrorFromErrno(errno);
    return error;
  }

  return static_cast<size_t>(stats.st_size);
}

Result<size_t, File::Error> File::SetOffset(size_t offset) {
  const auto offset_result = lseek(m_fd, offset, SEEK_SET);
  if (offset_result == -1) {
    const auto error = ErrorFromErrno(errno);
    return error;
  }
  return offset_result;
}

Result<std::vector<uint8_t>, File::Error> File::ReadAll() {
  DITTO_PROPAGATE(SetOffset(0));
  const auto size = DITTO_PROPAGATE(Size());

  std::vector<uint8_t> data;
  data.resize(size);

  const auto read_size = read(m_fd, data.data(), size);
  if (read_size < 0) {
    const auto error = ErrorFromErrno(errno);
    return error;
  }

  return data;
}

Result<void, File::Error> File::Write(Ditto::span<uint8_t> buffer) {
  const auto write_size = write(m_fd, buffer.data(), buffer.size());
  if (write_size < 0) {
    return File::ErrorFromErrno(errno);
  }

  return Result<void, Error>::ok();
}

File::File(File &&other) : m_fd(other.m_fd) { other.m_fd = -1; }

File &File::operator=(File &&other) {
  if (this == &other)
    return *this;

  if (m_fd >= 0) {
    close(m_fd);
  }
  m_fd = other.m_fd;
  other.m_fd = -1;

  return *this;
}

File::~File() {
  if (m_fd < 0) {
    return;
  }

  close(m_fd);
}
