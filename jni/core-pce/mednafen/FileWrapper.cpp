/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <sys/stat.h>
#include "mednafen.h"
#include "FileWrapper.h"

#include <trio/trio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

// Some really bad preprocessor abuse follows to handle platforms that don't have fseeko and ftello...and of course
// for largefile support on Windows:

#define fseeko fseek
#define ftello ftell

#if SIZEOF_OFF_T == 4

 #ifdef HAVE_FOPEN64
  #define fopen fopen64
 #endif

 #ifdef HAVE_FTELLO64
  #undef ftello
  #define ftello ftello64
 #endif

 #ifdef HAVE_FSEEKO64
  #undef fseeko
  #define fseeko fseeko64
 #endif

 #ifdef HAVE_FSTAT64
  #define fstat fstat64
  #define stat stat64
 #endif

#endif

// For special uses, IE in classes that take a path or a FileWrapper & in the constructor, and the FileWrapper non-pointer member
// is in the initialization list for the path constructor but not the constructor with FileWrapper&

FileWrapper::FileWrapper(const char *path, const int mode, const char *purpose) : OpenedMode(mode)
{
 if(mode == MODE_WRITE)
  fp = fopen(path, "wb");
 else
  fp = fopen(path, "rb");

 if(!fp)
 {
  ErrnoHolder ene(errno);

  throw(MDFN_Error(ene.Errno(), _("Error opening file %s"), ene.StrError()));
 }
}

FileWrapper::~FileWrapper()
{
   close();
}

void FileWrapper::close(void)
{
 if(fp)
 {
  FILE *tmp = fp;

  fp = NULL;

  if(fclose(tmp) == EOF)
  {
   ErrnoHolder ene(errno);

   throw(MDFN_Error(ene.Errno(), _("Error closing opened file %s"), ene.StrError()));
  }
 }
}

uint64 FileWrapper::read(void *data, uint64 count, bool error_on_eof)
{
 uint64 read_count = fread(data, 1, count, fp);

 return(read_count);
}

void FileWrapper::flush(void)
{
 fflush(fp);
}

void FileWrapper::write(const void *data, uint64 count)
{
 fwrite(data, 1, count, fp);
}

int FileWrapper::scanf(const char *format, ...)
{
 va_list ap;
 int ret;

 va_start(ap, format);

 ret = trio_vfscanf(fp, format, ap);

 va_end(ap);

 return ret;
}

void FileWrapper::put_char(int c)
{
 fputc(c, fp);
}

void FileWrapper::put_string(const char *str)
{
 write(str, strlen(str));
}

// We need to decide whether to prohibit NULL characters in output and input strings via std::string.
// Yes for correctness, no for potential security issues(though unlikely in context all things considered).
void FileWrapper::put_string(const std::string &str)
{
 write(str.data(), str.size());
}

char *FileWrapper::get_line(char *buf_s, int buf_size)
{
 return ::fgets(buf_s, buf_size, fp);
}


void FileWrapper::seek(int64 offset, int whence)
{
 fseeko(fp, offset, whence);
}

int64 FileWrapper::size(void)
{
 struct stat buf;

 fstat(fileno(fp), &buf);

 return(buf.st_size);
}

int64 FileWrapper::tell(void)
{
 return ftello(fp);
}
