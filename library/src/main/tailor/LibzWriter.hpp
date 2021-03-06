/*
 * Copyright (C) 2020 ByteDance Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBZ_WRITER_H
#define LIBZ_WRITER_H 1

#include <unistd.h>
#include <string.h>
#include <zlib.h>
#include "stream.hpp"
//**************************************************************************************************
class LibzWriter : public Writer {
public:
    LibzWriter(const char *name);
    ~LibzWriter();
public:
     int proxy(int flags);
	void flush(char *buff, size_t bytes, bool isEof);
private:
	z_stream stream;
	char     output[LENGTH];
};
//**************************************************************************************************
LibzWriter::LibzWriter(const char *path) {
    name = path;
    wrap = -1;

    target = fopen(path, "w");
    offset = 0;

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    deflateInit(&stream, 6);
}

LibzWriter::~LibzWriter() {
    flush(buffer, offset, true);
    offset = 0;
    deflateEnd(&stream);

    fflush(target);
    fclose(target);
    target = nullptr;
}

int LibzWriter::proxy(int flags) {
    char proxy[256];
    sprintf(proxy, "%s.proxy", name);
    return wrap = open(proxy, flags);
}

void LibzWriter::flush(char *buff, size_t count, bool isEof) {
    stream.avail_in = count;
    stream.next_in = (Bytef *) buff;

    do {
        stream.avail_out = LENGTH;
        stream.next_out = (Bytef *) output;

        if (Z_STREAM_ERROR == deflate(&stream, isEof ? Z_FINISH : Z_NO_FLUSH)) {
            return;
        } else {
            fwrite(output, 1, LENGTH - stream.avail_out, target);
        }
    } while (this->stream.avail_out == 0);
}
#endif //LIBZ_WRITER_H