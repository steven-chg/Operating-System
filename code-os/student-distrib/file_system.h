#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "lib.h"
#include "types.h"

/* Define constants */
#define MAX_FILES 63                   // maximum of 63 files in the file system
#define MAX_FILE_NAME 32               // file name up to 32 characters 
#define RTC_FILE_TYPE 0                // file type 0 for a file giving user-level access to RTC
#define DIRECTORY_FILE_TYPE 1          // file type 1 for directory files
#define REGULAR_FILE_TYPE 2            // file type 2 for regular files 

#define FILE_SYSTEM_BLOCK_SIZE 4096                       // file system divided into 4kB blocks, or 4096 byte blocks
#define MAX_DATA_BLOCKS ((FILE_SYSTEM_BLOCK_SIZE - 4)/4)  // max number of data blocks in each index node (subtract 4 bytes for length in bytes of file)

/* Define directory_entry struct (64B each) to include the file name, file type, and index node number */
typedef struct {
    int8_t file_name[MAX_FILE_NAME];             //File name up to 32 characters
    uint32_t file_type;
    uint32_t inode_number;
    uint8_t reserved_bytes[24];                  //24B reserved 
} dentry_t;

/* Define boot_block struct (first block in the file system memory), to include file system statistics and directory entries */
typedef struct {
    uint32_t directory_entry_count;
    uint32_t inode_count;
    uint32_t data_block_count;
    uint8_t reserved_bytes[52];                //52B reserved in boot block 
    dentry_t dir_entries[MAX_FILES];           //file system holds up to 63 files (each file is 64B)
} boot_block; 

/* Define index_node struct, to include length in bytes of the file size and numbers of data block that make up the file */
typedef struct {
    uint32_t file_length; 
    uint32_t data_block_num[MAX_DATA_BLOCKS];      //maximum of 1023 data block numbers in each index node 
} index_node;

/* Define data_block struct, to hold up to 4096 bytes of data each */
typedef struct {
    uint8_t data_byte[FILE_SYSTEM_BLOCK_SIZE];  //maximum of 4096 bytes of data in each data block
} data_block;

/* Define global variables */  
/////////////////////////////// SET THIS IN KERNEL BEFORE CALLING file_system_init 
extern uint32_t* file_system_ptr;                  //pointer to the beginning of the entire file system (set in kernel.c before calling file_system_init)
boot_block* boot_block_ptr;                        //pointer to the boot block of the file system
index_node* index_nodes_ptr;                       //pointer to first index node in the file system
data_block* data_blocks_ptr;                       //pointer to first data block in the file system


/* 
 * file_system_init
 *  DESCRIPTION: Function to iniitialize the read-only file system
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: For the current file system, set the pointer to the boot block, pointer to the first index node, and pointer
 *               to the first data block 
 */
void file_system_init();

/* 
 * read_dentry_by_name
 *  DESCRIPTION: Function to read and populate a directory entry block by name 
 *  INPUTS: fname - name to look for in directory entries 
 *          dentry - dentry_t block containing file_name, file_type, and inode_number to be filled
 *  OUTPUTS: none
 *  RETURN VALUE: -1 on failure, 0 on success
 *  SIDE EFFECT: Fill in the dentry_t block passed in as the second argument with the file_name, 
 *               file_type, and inode_number for the file.
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/* 
 * read_dentry_by_index
 *  DESCRIPTION: Function to read and populate a directory entry block by directory entry index (0 indexed)
 *  INPUTS: index - index of the directory entry that we want to read (not inode number)
 *          dentry - dentry_t block containing file_name, file_type, and inode_number to be filled
 *  OUTPUTS: none
 *  RETURN VALUE: -1 on failure, 0 on success
 *  SIDE EFFECT: Fill in the dentry_t block passed in as the second argument with the file_name,
 *               file_type, and inode_number for the file.
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/* 
 * read_data
 *  DESCRIPTION: Function to read from file and store in buffer
 *  INPUTS: inode - index node number of the file to read from 
 *          offset - position offset in the file to begin reading from
 *          buf - buffer to store what is read from file
 *          length - number of bytes to read (length of file) and place in the buffer buf
 *  OUTPUTS: none
 *  RETURN VALUE: -1 on failure, otherwise, returns number of bytes read and placed in the buffer (0 if we are at the end of file)
 *  SIDE EFFECT: Read up to length bytes starting from position offset in the file with 
 *               inode number inode, returning number of bytes read and placed in the buffer buf.
 *               Read until end of file or end of buffer provided.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

///////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
/* 
 * file_open
 *  DESCRIPTION: Function to open a file in the file system 
 *  INPUTS: filename - name of file to be opened 
 *  OUTPUTS: none
 *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
 *  SIDE EFFECT: Opens the file if there is still space in the file array and initializes the file descriptor structure for the file
 */
int32_t file_open(const uint8_t* filename);

////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
/* 
 * file_close
 *  DESCRIPTION: Function to close a file in the file system
 *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open file 
 *  OUTPUTS: none
 *  RETURN VALUE: 0 on success, -1 on failure
 *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
 */
int32_t file_close(int32_t fd);

/* 
 * file_write
 *  DESCRIPTION: Function to write to the file 
 *  INPUTS: fd - file descriptor (index into file array to find file to write into)
 *          buf - contains data to write into file
 *          nbytes - number of bytes to write into file
 *  OUTPUTS: none
 *  RETURN VALUE: -1 always (because we have a read only file system; cannot write)
 *  SIDE EFFECT: None, because we have a read only file system
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

/* 
 * file_read
 *  DESCRIPTION: Function to read data from a file (calls read_data function within)
 *  INPUTS: fd - file descriptor (index into file array to find file to read from)
 *          buf - buffer that stores data read from the file
 *          nbytes - number of bytes to read from the file 
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes read from the file (0 if initial position is at or beyond end of file)
 *  SIDE EFFECT: Given a filename, a buffer, and a buffer length, will read data from the given
 *               file into the buffer until the end of file or end of buffer provided, whichever comes first
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
/* 
 * directory_open()
 *  DESCRIPTION: Function to open a directory file in the file system
 *  INPUTS: filename - name of directory to be opened
 *  OUTPUTS: none
 *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
 *  SIDE EFFECT: Opens the directory if there is still space in the file array and initializes the file descriptor structure for the directory
 */
int32_t directory_open(const uint8_t* filename);

////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
/* 
 * directory_close()
 *  DESCRIPTION: Function to close a directory in the file system
 *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open directory
 *  OUTPUTS: none
 *  RETURN VALUE: 0 on success, -1 on failure
 *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
 */
int32_t directory_close(int32_t fd);

/* 
 * file_write()
 *  DESCRIPTION: Function to write to a directory file in the file system
 *  INPUTS: fd - file descriptor (index into file array to find directory to write into)
 *          buf - contains data to write into file
 *          nbytes - number of bytes to write into file *  OUTPUTS: none
 *  RETURN VALUE: -1 always (because we have a read only file system; cannot write)
 *  SIDE EFFECT: None, because we have a read only file system
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);

////////////////////// CHECKPOINT 2 SHOULD READ ALL FILE NAMES ONE FILE NAME AT A TIME ////////////////////////////////////////
/*
 * directory_read()
 *  DESCRIPTION: Function to read all file names (one file at a time) from consecutive directory entries in the file system
 *  INPUTS: fd - file descriptor (index into file array to find file to begin reading name from)
 *          buf - buffer to store file name read from consecutive directory entries (one name at a time)
 *          nbytes - number of bytes to read 
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes read (0 if initial position is at or beyond end of directory entries)
 *  SIDE EFFECT: The filename should be read into the buffer (as much as possible, or all 32 bytes), and
 *               subsequent reads should read from successive directory entries until the last is reached, at which point read should
 *               repeatedly return 0.
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);














////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                     Checkpoint 2 Working Version                                           */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ////////////// RESET TO 0 FIRST IN TEST.C FOR CHECKPOINT 2, WILL IN EFFECT BE REPLACED BY "FILE_POS IN FD IN FUTURE" /////////////////////
// uint32_t directory_read_counter;                //counter to track directory entry read index 
// uint32_t byte_to_read_from;                     //counter to track where we are in a file

// int32_t file_read(int32_t fd, void* buf, int32_t nbytes,  const uint8_t* fname);


#endif
