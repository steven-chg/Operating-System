#include "systemcall.h"
#include "file_system.h"

/* 
 * file_system_init
 *  DESCRIPTION: Function to iniitialize the read-only file system
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: For the current file system, set the pointer to the boot block, pointer to the first index node, and pointer
 *               to the first data block 
 */
void file_system_init(){
    boot_block_ptr = (boot_block*) file_system_ptr;                                     // set the boot block pointer of current file system
    index_nodes_ptr = (index_node*) (boot_block_ptr + 1);                               // set ptr to first index node ( + 1 to offset the boot block)
    data_blocks_ptr = (data_block*) (index_nodes_ptr + (boot_block_ptr->inode_count));  // set ptr to first data block of 4096 bytes each
}

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
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int32_t current_dentry_index, input_file_length;

    // Get the file length of argument (since file name need not be NUL terminated, if strlen returns greater than 32, fail)
    input_file_length = strlen((int8_t*)fname);
    // Fail and return -1 since input length is too long 
    if(input_file_length > MAX_FILE_NAME) return -1;//input_file_length = MAX_FILE_NAME;

    // Loop through all directory entries (63 + 1 since we want to go through files 0 - 63)
    for(current_dentry_index = 0; current_dentry_index < (MAX_FILES + 1); current_dentry_index++){

        int dentry_file_length;
        // Get the file length of current directory entry (since file name need not be NUL terminated, if strlen returns greater than 32, set length to 32)
        dentry_file_length = strlen((int8_t*) (boot_block_ptr->dir_entries[current_dentry_index].file_name));
        if(dentry_file_length > MAX_FILE_NAME) dentry_file_length = MAX_FILE_NAME;

        // If the file name lengths are not equal, then continue to next dentry
        if(input_file_length != dentry_file_length) continue; 

        // Compare the two file names and see if they are the same (if return value of strncmp is 0, it means the two strings are the same);
        if(strncmp((int8_t*) (boot_block_ptr->dir_entries[current_dentry_index].file_name), (int8_t*) fname, MAX_FILE_NAME) == 0){
            // The two names are the same, found the file; Copy the file name into dentry (to be padded with 0s, so up to 32 bytes)
            strncpy((int8_t*) dentry->file_name, (int8_t*) fname, MAX_FILE_NAME);
            // Copy file type and inode number
            dentry->file_type = boot_block_ptr->dir_entries[current_dentry_index].file_type;
            dentry->inode_number = boot_block_ptr->dir_entries[current_dentry_index].inode_number;

            // Return success
            return 0;
        }
    }

    // Have not found the file name in the directory of entries, return failure
    return -1;
}

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
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    // If index is less than 0 or greater than maximum number of files (63), return failure (invalid index)
    if(index < 0 || index > MAX_FILES) return -1;

    // If valid index, then copy file name (to be padded with 0s so up to 32 bytes), file type, and inode number
    strncpy((int8_t*) dentry->file_name, (int8_t*) boot_block_ptr->dir_entries[index].file_name, MAX_FILE_NAME);
    dentry->file_type = boot_block_ptr->dir_entries[index].file_type;
    dentry->file_type = boot_block_ptr->dir_entries[index].inode_number;

    // After copy, return success
    return 0;
}

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
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){ 
    // printf("Read_data: %d\n", length);
    // variable for looping through entire length to read
    int i; 
    // variables to represent the data block we look at and the specific data within each block
    // data_block* current_data_block_ptr;
    // uint32_t*   current_data_block_ptr;
    data_block* current_data_block_ptr;
    uint8_t target_data;

    // declare and initialize the index into the index node to find data block number
    uint32_t data_block_inode_index = offset / FILE_SYSTEM_BLOCK_SIZE;
    // declare and initialize the byte offset in the specific data block by using modulo by 4096 or block size of file system
    uint32_t byte_offset_in_block = offset % FILE_SYSTEM_BLOCK_SIZE;

    // index node for the current file
    index_node* current_index_node = (index_node*) index_nodes_ptr + inode;

    // initialize bytes read count to 0 for this current iteration
    uint32_t bytes_read_count = 0; 

    // check if the index node is within bounds, if not return failure or -1
    if( (inode < 0) || (boot_block_ptr->inode_count <= inode) ){ 
        return -1; 
    } 

    // if the offset into the file is already at or past the file length, return 0 (can't read anymore)
    if(offset >= current_index_node->file_length){ 
        return 0; 
    }

    // printf("begin reading");

    // starting offset of current read_data iteration is within file length, attempt to read the entire desired length
    for(i = 0; i < length; i++){  
        // check if the index into length that we want to read plus original offset is past file length, if so, return bytes read 
        if((i + offset) < current_index_node->file_length){
            
          // printf("Byte: %d", i);

          // if the byte_offset has accumulated to be past the file system block size, then reset it and move to the next data block  
          if(byte_offset_in_block >= FILE_SYSTEM_BLOCK_SIZE){
            byte_offset_in_block = 0;
            data_block_inode_index++;
          }
          
          // if(MAX_DATA_BLOCKS <= data_block_inode_index){                          //check if current block is valid
          //   return bytes_read_count;
          // }

          // check that the block number is valid; if it is not, return -1
          if(boot_block_ptr->data_block_count <= current_index_node->data_block_num[data_block_inode_index]){    
              return -1;
          }

          // find the current data block to read from
          current_data_block_ptr = (data_block*) (data_blocks_ptr + current_index_node->data_block_num[data_block_inode_index]);
            
          // retrieve the target byte from the current data block 
          target_data = current_data_block_ptr->data_byte[byte_offset_in_block];  
        
          // printf("%c", target_data);

          // copy the target data into the buffer accordingly (using i as index)
        //   memcpy((uint8_t*) buf, (uint8_t*) current_data_block.data_byte[byte_offset_in_block],1);
          buf[i] = target_data;
          // memcpy(buf, target_data, 1);

          // increment bytes read and offset 
          //   buf++;
          bytes_read_count++;
          byte_offset_in_block++;                                                  
        } else{
            // printf("Exiting at i = %d", i);
          // if we have read to the end of the file, return bytes we have read into the buffer
          return bytes_read_count;
        }
    } 

    return bytes_read_count;

// /* SMALL FILE READ WORKING VERSION */
//     int i; 
//     uint32_t bytes_read_count = 0; 
//     index_node file_inode;
//     data_block first_data_block;
//     uint32_t file_length;
//     file_inode = index_nodes_ptr[inode];
//     first_data_block = data_blocks_ptr[file_inode.data_block_num[0]];
//     file_length = file_inode.file_length;

//     //copy bytes to buffer 
//     for(i=0; i < length; i++){  
//         if(i < file_length){
//             buf[i] = first_data_block.data_byte[i];
//             bytes_read_count++;
//         } else{
//             buf[i] = NULL;
//         }
//     } 
//     return bytes_read_count;

}

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
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){ 
    //testing
    // printf("File_read: %d\n", nbytes);

    // retrieve the current pcb pointer and file array pointer
    pcb_struct* current_pcb = get_pcb_ptr();
    file_array_struct* current_file_array = current_pcb->file_array;

    // declare variable to store bytes read of current iteration
    int32_t bytes_read;

    // call read data to read into buffer
    bytes_read = read_data(current_file_array[fd].inode_number, current_file_array[fd].file_position, buf, nbytes);

    // check if we read any bytes (if read_data failed sanity check, return -1)
    if(bytes_read < 0){
        return -1;
    }
    
    // update file position
    current_file_array[fd].file_position += bytes_read;

    return bytes_read;
}

/*
 * directory_read()
 *  DESCRIPTION: Function to read all file names (one file at a time) from consecutive directory entries in the file system
 *  INPUTS: fd - file descriptor (index into file array to find entry of directory file type)
 *          buf - buffer to store file name read from consecutive directory entries (one name at a time)
 *          nbytes - number of bytes to read 
 *  OUTPUTS: none
 *  RETURN VALUE: number of bytes read (0 if initial position is at or beyond end of directory entries)
 *  SIDE EFFECT: The filename should be read into the buffer (as much as possible, or all 32 bytes), and
 *               subsequent reads should read from successive directory entries until the last is reached, at which point read should
 *               repeatedly return 0.
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes){

    // retrieve the current pcb pointer and file array pointer
    pcb_struct* current_pcb = get_pcb_ptr();
    file_array_struct* current_file_array = current_pcb->file_array;

    // variable for storing current directory entry 
    dentry_t current_directory_entry;

    // // declare variable to store bytes read of current iteration
    // int32_t bytes_read;

    // declare variable to store file name length that we will return
    int32_t file_name_length;

    // retrieve directory entry information and return value using read_dentry_by_index
    int32_t read_dentry_return;
    read_dentry_return = read_dentry_by_index(current_file_array[fd].file_position, &current_directory_entry);

    // Check the return value; if it is -1, then it means the index is <0 or >63
    if(read_dentry_return == -1){
        return 0;                           // return 0 and don't update file position
    }

    // Get the length of the file_name that we will return (if greater than 32, set to 32)
    file_name_length = strlen((int8_t*) current_directory_entry.file_name);
    if(file_name_length > MAX_FILE_NAME) file_name_length = MAX_FILE_NAME;

    // If return value is not -1, it means we have successfully copied into current_directory_entry, copy file name into buffer
    strncpy((int8_t*) buf, (int8_t*) current_directory_entry.file_name, MAX_FILE_NAME);

    // update file position
    current_file_array[fd].file_position++;


    return file_name_length;
}

/* 
 * file_open
 *  DESCRIPTION: Function to open a file in the file system 
 *  INPUTS: filename - name of file to be opened 
 *  OUTPUTS: none
 *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
 *  SIDE EFFECT: Opens the file if there is still space in the file array and initializes the file descriptor structure for the file
 */
int32_t file_open(const uint8_t* filename){
    return 0;                   // done in systemcall.c open function
}

/* 
 * file_close
 *  DESCRIPTION: Function to close a file in the file system
 *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open file 
 *  OUTPUTS: none
 *  RETURN VALUE: 0 on success, -1 on failure
 *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
 */
int32_t file_close(int32_t fd){
    return 0;                   // done in systemcall.c close function
}

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
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;                  // return -1 to indicate fail because we have a read only file system
}

/* 
 * directory_open()
 *  DESCRIPTION: Function to open a directory file in the file system
 *  INPUTS: filename - name of directory to be opened
 *  OUTPUTS: none
 *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
 *  SIDE EFFECT: Opens the directory if there is still space in the file array and initializes the file descriptor structure for the directory
 */
int32_t directory_open(const uint8_t* filename){
    return 0;                   // done in systemcall.c open function
}

/* 
 * directory_close()
 *  DESCRIPTION: Function to close a directory in the file system
 *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open directory
 *  OUTPUTS: none
 *  RETURN VALUE: 0 on success, -1 on failure
 *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
 */
int32_t directory_close(int32_t fd){
    return 0;                   // done in systemcall.c close function
}

/* 
 * file_write()
 *  DESCRIPTION: Function to write to a directory file in the file system
 *  INPUTS: fd - file descriptor (index into file array to find directory to write into)
 *          buf - contains data to write into file
 *          nbytes - number of bytes to write into file *  OUTPUTS: none
 *  RETURN VALUE: -1 always (because we have a read only file system; cannot write)
 *  SIDE EFFECT: None, because we have a read only file system
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;                  // return -1 to indicate fail because we have a read only file system
}





























////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                     Checkpoint 2 Working Version                                           */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// /* 
//  * file_read
//  *  DESCRIPTION: Function to read data from a file (calls read_data function within)
//  *  INPUTS: fd - file descriptor (index into file array to find file to read from)
//  *          buf - buffer that stores data read from the file
//  *          nbytes - number of bytes to read from the file 
//  *  OUTPUTS: none
//  *  RETURN VALUE: number of bytes read from the file (0 if initial position is at or beyond end of file)
//  *  SIDE EFFECT: Given a filename, a buffer, and a buffer length, will read data from the given
//  *               file into the buffer until the end of file or end of buffer provided, whichever comes first
//  */
// //int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
// int32_t file_read(int32_t fd, void* buf, int32_t nbytes, const uint8_t* fname){ //added another arg cuz fd not yet implemented

//     // /* Small File Read Test */
//     // int32_t bytes_read;

//     // // retrieve the directory entry of the current file 
//     // int32_t read_dentry_return;
//     // dentry_t returned_dentry;
//     // read_dentry_return = read_dentry_by_name(fname, &returned_dentry);
//     // printf("File Name: %s\n", returned_dentry.file_name);
//     // printf("File Inode Number: %d\n", returned_dentry.inode_number);

//     // // read data with offset 
//     // bytes_read = read_data(returned_dentry.inode_number, byte_to_read_from, buf, nbytes);

//     // if (bytes_read == -1){ //if read_data() fail, return -1
//     //     return -1;
//     // }

//     // // UPDATE byte_to_read_from (offset)
//     // // byte_to_read_from += bytes_read;

//     // return bytes_read; //return bytes read from the file


//     int32_t bytes_read;

//     // retrieve the directory entry of the current file 
//     int32_t read_dentry_return;
//     dentry_t returned_dentry;
//     read_dentry_return = read_dentry_by_name(fname, &returned_dentry);

//     // read data with offset (which byte to begin reading from)
//     bytes_read = read_data(returned_dentry.inode_number, byte_to_read_from, buf, index_nodes_ptr[returned_dentry.inode_number].file_length);

//     // UPDATE byte_to_read_from (offset)
//     byte_to_read_from += bytes_read;

// ////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
// /* DONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//  * file_open
//  *  DESCRIPTION: Function to open a file in the file system 
//  *  INPUTS: filename - name of file to be opened 
//  *  OUTPUTS: none
//  *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
//  *  SIDE EFFECT: Opens the file if there is still space in the file array and initializes the file descriptor structure for the file
//  */
// int32_t file_open(const uint8_t* filename){
//     return 0;           // default success for checkpoint 2; have to check file array in the future

//     //Future
//     /*
//     Need to check if the filename exists
//     Need to check if there is space in the file array
//     If either of these are false, then return -1
//     Otherwise, allocate an unused file descriptor index in the file array
//     and set up data structure in file array for the given file type
//     */
// }

// ////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
// /* DONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//  * file_close
//  *  DESCRIPTION: Function to close a file in the file system
//  *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open file 
//  *  OUTPUTS: none
//  *  RETURN VALUE: 0 on success, -1 on failure
//  *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
//  */
// int32_t file_close(int32_t fd){
//     return 0;           // default success for checkpoint 2; have to check file array in the future

//     //Future
//     /*
//     If fd is 0 or 1 (default descriptors) or an invalid descriptor, return failure 
//     Otherwise, close the file descriptor and return success/0
//     */
// }



// ////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
// /* DONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//  * directory_open()
//  *  DESCRIPTION: Function to open a directory file in the file system
//  *  INPUTS: filename - name of directory to be opened
//  *  OUTPUTS: none
//  *  RETURN VALUE: returns file descriptor index on success, -1 on failure (if there is no space in file array or filename doesn't exist)
//  *  SIDE EFFECT: Opens the directory if there is still space in the file array and initializes the file descriptor structure for the directory
//  */
// int32_t directory_open(const uint8_t* filename){
//     return 0;           // default success for checkpoint 2; have to check file array in the future

//     //Future
//     /*
//     Need to check if the filename exists
//     Need to check if there is space in the file array
//     If either of these are false, then return -1
//     Otherwise, allocate an unused file descriptor index in the file array
//     and set up data structure in file array for the given file type
//     */
// }

// ////////////////////// CHECKPOINT 2 don't have to implement file descriptor and file array yet ////////////////////////////////////////
// /* DONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//  * directory_close()
//  *  DESCRIPTION: Function to close a directory in the file system
//  *  INPUTS: fd - file descriptor, or index into the file array, which identifies the open directory
//  *  OUTPUTS: none
//  *  RETURN VALUE: 0 on success, -1 on failure
//  *  SIDE EFFECT: Closes the specified file descriptor and makes it available 
//  */
// int32_t directory_close(int32_t fd){
//     return 0;           // default success for checkpoint 2; have to check file array in the future

//     //Future
//     /*
//     If fd is 0 or 1 (default descriptors) or an invalid descriptor, return failure 
//     Otherwise, close the file descriptor and return success/0
//     */
// }
