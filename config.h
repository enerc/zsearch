#pragma once

// number of indexed documents per chunk
#define CHUNK_SIZE 65536
#define CHUNK_SIZE_LOG2 16
// That's only 4.4 trillions of document at most per index. Increase it if using large dataset.
#define MAX_NUMBER_OF_CHUNKS (64L*1024L*1024L)

// average number of words in a string
#define AVERAGE_BYTES_PER_STRING (1024)

// Since Linux 2.6.0, alignment to the logical block
// size of the underlying storage (typically 512 bytes) suffices.  The
// logical block size can be determined using the ioctl(2) BLKSSZGET
// operation or from the shell using the command:
// blockdev --getss
#define USE_O_DIRECT	1
#define O_DIRECT_ALIGN	(512-1)

// size of the buffers to import files - 2 buffers to allocate
#define IMPORT_BUFFER_SIZE (1024L*1024L*1024L)

#define VERSION "0.1"