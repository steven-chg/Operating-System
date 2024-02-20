#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "i8259.h"
#include "systemcall.h"
#include "terminal.h"
#include "keyboard.h"
#include "rtc.h"
#include "paging.h"
#include "idt.h"
#include "file_system.h"


#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* i8259_enable_test
 * 
 * Inputs: NONE
 * Outputs: mask the interupts of RTC and keyboard
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */
int i8259_enable_test() {
	TEST_HEADER;
	//enable RTC (IRQ8) and keyboard (IRQ1)
	enable_irq(1); 
	enable_irq(8);
	return PASS;
	
}
/* i8259_disable_test
 * 
 * Inputs: NONE
 * Outputs: unmask the interupts of RTC and keyboard
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int i8259_disable_test() {
	TEST_HEADER;
	//disable RTC (IRQ8) and keyboard (IRQ1)
	disable_irq(1);
	disable_irq(8);
	return PASS;
	
}

/* i8259_garbage_test
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int i8259_garbage_test() {
	TEST_HEADER;
	//test garbage irq numbers
	enable_irq(20);
	enable_irq(1000);
	disable_irq(33);
	disable_irq(88);
	return PASS;
}
/* system_call_test
 * 
 * Inputs: NONE
 * Outputs: printf a string 
 * Side Effects: None
 * Coverage: systemcall_handler
 * Files: systemcall.h/c
 */
int system_call_test() {
	TEST_HEADER;
	__asm__("int $0x80");	//0x80 for system call
	return PASS;
}

/* divide_by_zero
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: raise Divide Error Exception
 * Coverage: exception handler
 * Files: exception handler.h/c
 */
// int divide_by_zero() {
// 	TEST_HEADER;
// 	int test;
// 	test = 20;
// 	test = 20/0;
// 	return PASS;
// }

/* page_fault
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: raise page fault Exception when we try to dereference NULL pointer
 * Coverage: exception handler, pages
 * Files: exception handler.h/c, pages.h/c
 */
int page_fault() {
	TEST_HEADER;
	int * a = NULL;
	*a = *a;
	return PASS;
}

/* page_test_inbound
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: tests if we can dereference locations that should be accessible
 * Coverage: exception handler, pages
 * Files: exception handler.h/c, pages.h/c
 */
int page_test_inbound(){
	TEST_HEADER;
	uint8_t * pointer;
	int temp;

	// Test kernel memory location
	pointer = (uint8_t*)0x400000;
    temp = *pointer;

	// Test video memory
    pointer = (uint8_t*)0xB8000;             
	temp = *pointer;
	
    return PASS;
}

/* page_test_outboundkernel
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: tests if we can dereference locations that should NOT be accessible
 * Coverage: exception handler, pages
 * Files: exception handler.h/c, pages.h/c
 */
int page_test_outboundkernel(){
	TEST_HEADER;
	uint8_t * pointer;
	int temp;

	// Test kernel memory location that is out of bound
	pointer = (uint8_t*)0x3FFFEE;		// less than 0x400000
    temp = *pointer;

	pointer = (uint8_t*)0x800011;		// greater than 0x7FFFFF
	temp = *pointer;
	
    return FAIL;
}

/* page_test_outboundvideo
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: tests if we can dereference locations that should NOT be accessible
 * Coverage: exception handler, pages
 * Files: exception handler.h/c, pages.h/c
 */
int page_test_outboundvideo(){
	TEST_HEADER;
	uint8_t * pointer;
	int temp;

	// Test video memory location that is out of bound
	pointer = (uint8_t*)0xB7532;		// less than 0xB8000
    temp = *pointer;

	pointer = (uint8_t*)0xB932;		// greater than 0xB8FFF
	temp = *pointer;
	
    return FAIL;
}

/* test_rtc
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: infinite loop calling rtc_handler to test test_interrupts
 * Coverage: rtc, test_interrupts
 * Files: rtc.h/c
 */
int test_rtc() {
	while(1){
		rtc_handler();
	}
	return PASS;
}

/* test_rtc_freq
 * 
 * Inputs: NONE
 * Outputs: NONE
 * Side Effects: infinite loop calling rtc_handler to test test_interrupts
 * Coverage: rtc, test_interrupts
 * Files: rtc.h/c
 */
int test_rtc_freq() {
	TEST_HEADER;
	int i;
	int count;
	int32_t rtc_freq;
	clear();
	for (i = 2; i <= 1024; i++) {
		//clear();
		count = 0;
		rtc_freq = i;
		if(rtc_write(0,&rtc_freq, 0) == -1) {
			//printf("bad input\n");
		} else {
			printf("testing RTC freq: %d\n", i);
			while (count < 20) {
				//test_interrupts();
				putc('a');
				//RTC_test_enable = 1;
				rtc_read();
				count++;
			}
		}
	}
	return PASS;
}


// add more tests here










// /* Checkpoint 2 tests */

// /* test_file_open_close_write
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that file_open, file_close, file_write is successful
//  * Coverage: file_open, file_close, file_write
//  * Files: file_system.h/c
//  */
// int test_file_open_close_write(){
// 	// uint8_t* test_file_name[MAX_FILE_NAME] = {"frame0.txt"};
// 	uint8_t* buffer[FILE_SYSTEM_BLOCK_SIZE];		
// 	// Check return values of file_open, file_write, and file_close (should be 0, -1, 0 respectively)
// 	if(!file_open((uint8_t* )"frame0.txt")){
// 		if(file_write(0, buffer, 0) == -1){
// 			if(!file_close(0)){
// 				return PASS;
// 			}
// 		}
// 	} 
// 	return FAIL;
// }

// /* test_dir_open_close_write
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that directory_open, directory_close, directory_write is successful
//  * Coverage: directory_open, directory_close, directory_write
//  * Files: file_system.h/c
//  */
// int test_dir_open_close_write(){
// 	// uint8_t* test_file_name[MAX_FILE_NAME] = {"."};
// 	uint8_t* buffer[MAX_FILE_NAME];
// 	// Check return valuees of directory_open, directory_write, and directory_close (should be 0, -1, 0 respectively)
// 	if(!directory_open((uint8_t* )".")){
// 		if(directory_write(0, buffer, 0) == -1){
// 			if(!directory_close(0)){
// 				return PASS;
// 			}
// 		}
// 	}
// 	return FAIL;
// }

// /* test_read_dentry_by_name_valid
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that read_dentry_by_name can read a valid name
//  * Coverage: read_dentry_by_name
//  * Files: file_system.h/c
//  */
// int test_read_dentry_by_name_valid(){
// 	// uint8_t* test_file_name[MAX_FILE_NAME] = {"frame0.txt"};
// 	dentry_t test_dentry;
// 	// Read by name and see if return value is 0 (success)
// 	if(read_dentry_by_name((uint8_t* )"frame0.txt", &test_dentry) == 0){
// 		// If 0, check the name in dentry and see if it is same as the test_file_name
// 		printf("File Name: %s\n", test_dentry.file_name);
// 		printf("File Type: %d\n", test_dentry.file_type);
// 		printf("File Length: %d\n", index_nodes_ptr[test_dentry.inode_number].file_length);
// 		return PASS;

// 		// if(test_dentry.file_name == "frame0.txt"){
// 		// 	return PASS;
// 		// }
// 	}
// 	return FAIL;
// }

// /* test_read_dentry_by_name_invalid
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that read_dentry_by_name cannot read an invalid name
//  * Coverage: read_dentry_by_name
//  * Files: file_system.h/c
//  */
// int test_read_dentry_by_name_invalid(){
// 	// uint8_t* test_file_name[MAX_FILE_NAME] = {"invalidname.txt"};
// 	dentry_t test_dentry;
// 	// Read by name and see if return value is -1 (failed read by name) since file name is invalid
// 	if(read_dentry_by_name((uint8_t* )"invalidname.txt", &test_dentry) == -1){
// 		return PASS;
// 	}
// 	return FAIL;
// }

// /* test_read_dentry_by_index_valid
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that read_dentry_by_index can read a valid index
//  * Coverage: read_dentry_by_index
//  * Files: file_system.h/c
//  */
// int test_read_dentry_by_index_valid(){
// 	uint32_t test_file_index = 0;
// 	dentry_t test_dentry;
// 	// Read by name and see if return value is 0 (success)
// 	if(read_dentry_by_index(test_file_index, &test_dentry) == 0){
// 		printf("File Name: %s\n", test_dentry.file_name);
// 		printf("File Type: %d\n", test_dentry.file_type);
// 		return PASS;
// 	}
// 	return FAIL;
// }

// /* test_read_dentry_by_index_invalid
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that read_dentry_by_index cannot read an invalid index
//  * Coverage: read_dentry_by_index
//  * Files: file_system.h/c
//  */
// int test_read_dentry_by_index_invalid(){
// 	uint32_t test_file_index = 65;
// 	dentry_t test_dentry;
// 	// Read by name and see if return value is -1 (failed read by name) since file name is invalid
// 	if(read_dentry_by_index(test_file_index, &test_dentry) == -1){
// 		return PASS;
// 	}
// 	return FAIL;
// }

// /* test_directory_read
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that we can read directory and read file names one at a time
//  * Coverage: directory_read
//  * Files: file_system.h/c
//  */
// int test_directory_read(){
// 	clear();
// 	// Initialize counter to 0 (start at file index 0 of directory entries)
// 	directory_read_counter = 0;
// 	// Declare buffer to hold file name (max size of 32 characters)
// 	uint8_t buffer[MAX_FILE_NAME];	
// 	while(1){
// 		int d_read_return;
// 		d_read_return = directory_read(0, buffer, 0);
// 		if(d_read_return != 0){
// 			// Only print if the number of bytes read into the buffer is non zero
// 			printf("file_name: ");
// 			int character_loop;
// 			// OR LOOP TO MAX_FILE_NAME? 
// 			for(character_loop = 0; character_loop < d_read_return; character_loop++){
// 				if(buffer[character_loop] == NULL){
// 					printf(" ");
// 				} else{
// 					putc(buffer[character_loop]);
// 				}
// 			}
// 			dentry_t test_dentry;
// 			int dentry_read_return;
// 			dentry_read_return = read_dentry_by_name(buffer, &test_dentry);
// 			if(dentry_read_return == 0){
// 				// Valid dentry, print file type and file size (length)
// 				printf(", file_type: %d, file_size: %d", test_dentry.file_type, index_nodes_ptr[test_dentry.inode_number].file_length);
// 			}
// 			printf("\n");
// 		}
// 	}
// }

// /* test_smallfile_read
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that we can read a small file
//  * Coverage: file_read, read_data
//  * Files: file_system.h/c
//  */
// int test_smallfile_read(){
// 	clear();

// 	//Declare buffer to hold file data (max size of ...)
// 	uint8_t buffer[FILE_SYSTEM_BLOCK_SIZE];

// 	//single read test for small files
// 	int file_read_test;
// 	file_read_test = file_read(0, buffer, FILE_SYSTEM_BLOCK_SIZE, (uint8_t*) "frame1.txt");
// 	if(file_read_test!=0){
// 		int character_loop;
// 		for(character_loop = 0; character_loop < file_read_test; character_loop++){
// 			if(buffer[character_loop] == NULL){
// 				//printf(" ");
// 			} 
// 			else{
// 				putc(buffer[character_loop]);
// 			}
// 		}	
// 	}
// 	return PASS;
// }


// /* test_file_read
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: Check that we can read all files
//  * Coverage: file_read, read_data
//  * Files: file_system.h/c
//  */
// int test_file_read(){
// 	clear(); 

// 	// Initialize offset to 0 (start reading at byte 0)
// 	byte_to_read_from = 0;

// 	// Declare buffer to hold file data (max size of 4096 bytes)
// 	uint8_t buffer[FILE_SYSTEM_BLOCK_SIZE];

// 	// declare file_read_return 
// 	int file_read_return;

// 	printf("File Name: verylargetextwithverylongname.tx\n");

// 	while(1){
// 		file_read_return = file_read(0, buffer, FILE_SYSTEM_BLOCK_SIZE, (uint8_t*)"verylargetextwithverylongname.tx");
// 		if(file_read_return != 0){
// 			int character_loop;
// 			for(character_loop = 0; character_loop < file_read_return; character_loop++){
// 				if(buffer[character_loop] == NULL){
// 					// printf(" ");
// 				} 
// 				else{
// 					putc(buffer[character_loop]);
// 				}
// 			}	
// 		} else{
// 			break;
// 		}
// 	}

// 	return PASS;
// }

// /* test_rtc_read_write
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: loop through every possible freq and putc 1 on the screen
//  * Coverage: rtc_read, rtc_write
//  * Files: rtc.h/c
//  */
// int test_rtc_read_write() {
// 	TEST_HEADER;
// 	int count;
// 	int i;
// 	int rtc_freq;
	
// 	clear();
// 	for(i = 1024; i>=0; i--) {
// 		rtc_freq = i;
// 		if (rtc_write(0,&rtc_freq) == -1) {
// 		//printf("bad input\n");
// 		}else{
// 			count = 0;
// 			while (count!=30) {
// 			rtc_read();
// 			putc('1');
// 			count++;

// 			}
// 			printf("\n");
// 		}
// 	}
	
// 	return PASS;
// }

// /* test_rtc_open_close
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: N/A
//  * Coverage: rtc_open, rtc_close
//  * Files: rtc.h/c
//  */
// int test_rtc_open_close() {
// 	TEST_HEADER;
	
// 	if (rtc_open(0)!=0){
// 		return FAIL;
// 	}

// 	if (rtc_close(0)!=0){
// 		return FAIL;
// 	}
	
// 	return PASS;
// }

// /* test_terminal_read_write
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: if terminal read "Bryan", terminal write Bryan to terminal and exit
//  * Coverage: terminal_write, terminal_read
//  * Files: terminal.c.h
//  */
// int test_terminal_read_write() {
// 	TEST_HEADER;
// 	char buf[128];
	
// 	terminal_open(0);
// 	printf("\ntest_negative_byte: ");
// 	terminal_write(0,"1234abcd", -1);
// 	printf("\ntest_less_byte: ");
// 	terminal_write(0,"1234abcd", 3);
// 	printf("\ntest_equal_byte: ");
// 	terminal_write(0,"1234abcd", 8);
// 	printf("\ntest_more_byte: ");
// 	terminal_write(0,"1234abcd", 100);
// 	printf("\n");

// 	while(1) {
// 		printf("enter user name: ");
// 		terminal_read(0, buf, 128);
// 		terminal_write(0,buf, 128);
// 		// if(strncmp("Bryan",buf,buf.size())){
// 		// 	terminal_write(0, buf, buf.size());
// 		// 	break;
// 		// }
// 	}
	
// 	terminal_close(0);
// 	return PASS;
// }

// /* test_terminal_read
//  * 
//  * Inputs: NONE
//  * Outputs: NONE
//  * Side Effects: if terminal gets NULL buffer, it should return -1
//  * Files: terminal.c.h
//  */

// int test_terminal_read() {
// 	TEST_HEADER;
// 	terminal_open(0);
	
// 	if(terminal_read(0, NULL, 128)==-1)return PASS;

// 	terminal_close(0);
// 	return FAIL;
// }


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
//	TEST_OUTPUT("idt_test", idt_test());
//	TEST_OUTPUT("i8259_test_enable", i8259_enable_test());
//	TEST_OUTPUT("i8259_test_disable", i8259_disable_test());
//	TEST_OUTPUT("i8259_garbage_disable", i8259_garbage_test());
//	TEST_OUTPUT("systemcall_test", system_call_test());
//	TEST_OUTPUT("divide_by_zero", divide_by_zero());
//	TEST_OUTPUT("test_page_fault", page_fault());git 
//	TEST_OUTPUT("test_should_dereference", page_test_inbound());
//	TEST_OUTPUT("test_should_not_deref_kernel", page_test_outboundkernel());
//	TEST_OUTPUT("test_should_not_deref_video", page_test_outboundvideo());

/* CHECKPOINT 2 TESTS */

	/* FILE SYSTEM TESTS */
	// TEST_OUTPUT("test_file_open_write_close", test_file_open_close_write());
	// TEST_OUTPUT("test_directory_open_write_close", test_dir_open_close_write());
	// TEST_OUTPUT("test_read_dentry_by_name_valid", test_read_dentry_by_name_valid());
	// TEST_OUTPUT("test_read_dentry_by_name_invalid", test_read_dentry_by_name_invalid());
	// TEST_OUTPUT("test_read_dentry_by_index_valid", test_read_dentry_by_index_valid());
	// TEST_OUTPUT("test_read_dentry_by_index_invalid", test_read_dentry_by_index_invalid());

	// Will stay in while loop
	// TEST_OUTPUT("directory_read_test", test_directory_read());

	
	// TEST_OUTPUT("small_file_read_test", test_smallfile_read());
	// TEST_OUTPUT("file_read_test", test_file_read());



	// launch your tests here

	//-------------------------RTC TESTS----------------------------//
	//clear();
	//both must be turned on to test interrput/ turn RTC_test_enable on and RTC_test_change_freq on 
	//RTC_test_enable = 1;
	//RTC_test_enable_int = 1;
	//TEST_OUTPUT("RTC_test_change_freq", test_rtc_freq());
	// TEST_OUTPUT("RTC_test_read_write", test_rtc_read_write());
	//TEST_OUTPUT("RTC_test_open_close", test_rtc_open_close());
	//-------------------------RTC TESTS----------------------------//


	//-------------------------TERMINAL TESTS----------------------------//
	//TEST_OUTPUT("Terminal_READ_test", test_terminal_read());
	//TEST_OUTPUT("Terminal_test", test_terminal_read_write());
	//-------------------------TERMINAL TESTS----------------------------//

	


}
