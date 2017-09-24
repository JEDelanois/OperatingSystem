#include "filesys_driver.h"
#include "lib.h"
#include "file_descriptor.h"

#define TOTAL_INODES_OFF 0x04 //4 byte offset
#define FILE_OFFSET 0x40	//64 Bytes
#define FILE_NAME_SIZE 0x20	//32 bytes
#define FILE_TYPE_SIZE 0x04	
#define BLOCK_SIZE 4096 // 4kb block 
#define BYTES_IN_FILENAME 32


//Memory struct for the dir entry 
typedef struct __attribute__((__packed__)) dirEntryStruc {
	uint8_t filename[32];
	uint32_t filetype;
	uint32_t inode_num;
	uint8_t reserved[24];
}dirEntryStruc;



/*
//Memory struct for the bootblock
has a size of 4kb 
*/
typedef struct __attribute__((__packed__)) bootBlockStruc {
	uint32_t num_dir_entries;	
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t junk[52];//reserved junk
	dirEntryStruc dirEntrys[63]; 
}bootBlockStruc; 


/*
//Memory struct for inodes
has a size of 4kb 
*/
typedef struct __attribute__((__packed__)) inodeStruc {
	uint32_t length;
	uint32_t data_block_entry[1023];
}inodeStruc;

//4kb
//Memory struct for the data blocks
typedef struct __attribute__((__packed__)) dataStruc {
	uint8_t data[4096];
}dataStruc;

//global variable that is set to be a pointer to the beginning of the file system
int FILE_SYSTEM;

/*
These are the function pointer arrays that are called by the system calls
*/
func_ptr file_func_table[3] = { (func_ptr)file_open, (func_ptr)read_file_sys, (func_ptr)file_write }; 
func_ptr directory_func_table[3] = { (func_ptr)directory_open, (func_ptr)directory_read, (func_ptr)directory_write }; 

/* Function Pointer Array with Different Parameters
		You need to pick a function pointer type to use as a "generic function pointer", use that type to define 
		your array, and use explicit casts. Casting one function pointer type to another and then back again is
		guaranteed to preserve the value. 
		Then to call add, you need to cast it back to the right type */
	
	//'generic' file pointer type
	//typedef int32_t (*func_ptr)(void);
	
	//file operation tables for files and directories
	
	
	
/*file_open
 * 
 *   DESCRIPTION:  this is called by the system call handler and it is used to
					set the give file descriptor array entry
					
 *   INPUTS: 	file_entry_t * file_desc_entry- FDE that is to be set for a file 
				dentry_t * dentry- dentry associated with the file
				
 *   OUTPUTS:  sets up file descrpor array
 *   RETURN VALUE:  returns zero on success
 *   SIDE EFFECTS: none
 */ 	
int32_t file_open(file_entry_t * file_desc_entry, dentry_t * dentry){

		bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
		//inodes start at 4kb after boot block
		inodeStruc * inodestruc = (inodeStruc *)(bootblock + 1);

		//update file descriptor array
		file_desc_entry->fop_table_ptr = file_func_table;	//file op table pointer (function pointer)
		file_desc_entry->inode_ptr = (uint8_t *)&inodestruc[dentry->numInode];	
		file_desc_entry->file_pos = 0;// set offset
		file_desc_entry->flags = 1; //mark as descrptor entry present
		
	return 0;
}

/*file_close
 * 
 *   DESCRIPTION:  meant to close down a file descriptor array when 
					file is no longer in use. We went with a differnt
					implementation and made one generic file close
					so this function does nothing
 *   INPUTS:  none
 *   OUTPUTS:  none
 *   RETURN VALUE: returns 0x04
 *   SIDE EFFECTS: never actually called
 */ 
int32_t file_close(){
	return 0;
}

/*file_write
 * 
 *   DESCRIPTION:  this funtion will be called by the system call handler and 
					it is meant to add text to a file in memory provieded in buf
 
 *   INPUTS:	const void* buf - Buffer with text to add
				int32_t nbytes - the number of bytes to copy from the buffer
 
 *   OUTPUTS: writes to file system
 *   RETURN VALUE:  returns number of bytes written or -1 on fail
 *   SIDE EFFECTS: adds text to a given file
 */ 
int32_t file_write(const void* buf, int32_t nbytes){
	//printf("FILE WRITE WAS CALLED");
	return -1;
}



/*directory_open
 * 
 *   DESCRIPTION:  this is called by the system call handler and it is used to
					set the give file descriptor array entry
					
 *   INPUTS: 	file_entry_t * file_desc_entry- FDE that is to be set for a file 
				dentry_t * dentry- dentry associated with the file
				
 *   OUTPUTS:  sets up file descrpor array
 *   RETURN VALUE:  returns zero on success
 *   SIDE EFFECTS: none
 */ 
int32_t directory_open(file_entry_t * file_desc_entry,dentry_t * dentry)
{

		//update file descriptor array
		file_desc_entry->fop_table_ptr = directory_func_table;	//file op table pointer (function pointer)
		file_desc_entry->inode_ptr = NULL;	// no inode coreesopdning
		file_desc_entry->file_pos = 0;// st offset
		file_desc_entry->flags = 1;// mark present


	return 0;
}


/*directory_close
 * 
 *   DESCRIPTION:  meant to close down a file descriptor array when 
					file is no longer in use. We went with a differnt
					implementation and made one generic file close
					so this function does nothing
 *   INPUTS:  none
 *   OUTPUTS:  none
 *   RETURN VALUE: returns 0x04
 *   SIDE EFFECTS: never actually called
 */ 
int32_t directory_close()
{
	return 0;
}


/*directory_write
 * 
 *   DESCRIPTION: this function should write to the directory and create a new file
					with the name provided to by buffer
					
 *   INPUTS: 	const void* buf - buffer to write from
				int32_t nbytes - number of bytes to write
				
 *   OUTPUTS: adds file to directory
 *   RETURN VALUE: returns -1
 *   SIDE EFFECTS: writes file to directory
 */ 
int32_t directory_write(const void* buf, int32_t nbytes)
{
	return -1;
}

/*directory_read
 * 
 *   DESCRIPTION:  this function should read the file name in the directory and 
					place them into the buffer based on the offsert provided
 *   INPUTS: 	uint32_t fd - index into file descriptor array
				int8_t* buff - buffer to be filled 
				uint32_t offset- fileoffset into directoy
				uint32_t length- length of data to be copied
 *   OUTPUTS: file name into the buffer
 *   RETURN VALUE: returns bytes read into the buffer or 0 if at end of directory
 *   SIDE EFFECTS: none
 */ 
int32_t directory_read(uint32_t fd, int8_t* buff, uint32_t offset, uint32_t length){
	int i;
	PCB_struct_t* pcb;
	pcb = get_PCB();
	
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	//inodes start at 4kb after boot block
	//inodeStruc * inodestruc = (inodeStruc *)(bootblock + 1);
	

	for(i = 0; (i <BYTES_IN_FILENAME)&& (i < length) && (bootblock->dirEntrys[offset].filename[i] != '\0'); i ++){
		buff[i] = bootblock->dirEntrys[offset].filename[i];
	}
	
	//just for testing
	//printf("FILE NAME:%s | SIZE %d\n",bootblock->dirEntrys[offset].filename,inodestruc[bootblock->dirEntrys[offset].inode_num].length);
	
	pcb->file_array[fd].file_pos += 1;
	if(offset < bootblock->num_dir_entries )
		return i;
	else
		return 0;
}


/*tab_help
 * 
 *   DESCRIPTION: this function is used by the tab completion it is very similar to read directory
					its purpose is to fill the buffer provided with the name of the file that corresponds
					to the offset provided
 *   INPUTS: 	int8_t* buff- buffer to be filled with name of corresponding file
				uint32_t offset- offset into the directoy
				uint32_t length- number of bytes to by copied
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   SIDE EFFECTS: none
 */ 
int32_t tab_help(int8_t* buff, uint32_t offset, uint32_t length){
	int i;

	//get bootblock
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
	//copy corresponding file name based of offset
	for(i = 0; (i <BYTES_IN_FILENAME)&& (i < length); i ++){
		buff[i] = bootblock->dirEntrys[offset].filename[i];
	}
	
	//if more files left return i else return 0
	if(offset < bootblock->num_dir_entries )
		return i;
	else
		return 0;
}



/*read_file_sys
 * 
 *   DESCRIPTION: this function is called by the system call handler. it purpose is to 
				read the data in from the corresponding file from the file descriptor array 
				and place it in the buffer given
					
 *   INPUTS: 	int32_t fd - index into file descriptor array
				uint8_t* buff - buffer to be filled with data
				uint32_t offset - offset to begin reading the file from 
				uint32_t length - how many bytes to read into the buffer
 *   OUTPUTS: buffer is filled with the contents requested
 *   RETURN VALUE:  returns number of bytes read, 0 when at end of file, -1 on error
 *   SIDE EFFECTS: file descriptor array is incremente by offset
 */ 
int32_t read_file_sys(uint32_t fd, uint8_t* buff, uint32_t offset, uint32_t length){
	PCB_struct_t* pcb;
	pcb = get_PCB();
	int32_t bytes_read;
	int32_t inode = -1;
	int i ;
	uint8_t* inode_ptr = pcb->file_array[fd].inode_ptr;
	//get bootblock
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
	//inodes start at 4kb after boot block
	inodeStruc * inodestruc = (inodeStruc *)(bootblock + 1);
	
	// get inode number and not inode pointer
	for( i = 0; i < bootblock->num_inodes; i++){
		if( &inodestruc[i] == (inodeStruc*)inode_ptr){
			inode = i;
			break;}
	}
	//if no inode return error
	if(inode == -1)
			return -1;
	// get the data into the buffer and reacord the number of bytes read
	bytes_read = read_data (inode, offset , buff , length);
	
	//if not an error
	//increment the offset intot the file in the file descriptor array
	if(bytes_read >= 0)
		pcb->file_array[fd].file_pos += bytes_read;
	return bytes_read;
	
	
}


/*
function prints file into the buffer
assumes that "length" can fit into the buffer you pass it
*/
/*read_file_to_buffer
 * 
 *   DESCRIPTION: function reads a file that name is provided into the buffer. assumes length	
					can fit inside the buffer. 
 *   INPUTS: 	uint8_t* name- name of the file desired to copy
				uint8_t* buff - buffer to be filled with data
				uint32_t offset - offset into the file to begin
				uint32_t length - number of bytes to read into the file
 *   OUTPUTS: fills buffer
 *   RETURN VALUE: number of bytes read, -1 on failure, 0 if offset is larger than file size
 *   SIDE EFFECTS: none
 */ 
int32_t read_file_to_buffer(uint8_t* name, uint8_t* buff, uint32_t offset, uint32_t length){
	dentry_t dentry;
	int32_t bytes_read;
	
	//get file dentry
	if (read_dentry_by_name (name,&dentry) == -1)
		return -1;
	//read data into buffer
	bytes_read = read_data (dentry.numInode, offset , buff , length);
	
	//return bytes read
	return bytes_read;
	
	
}



/*read_file_to_mem
 * 
 *   DESCRIPTION: this function is used to copy a file specified by name into 
					specific location memory location. it will copy beginning from the 
					given offset to the end of the file. We use this as our loader for the 
					executable code. 
 *   INPUTS: 	uint8_t* name - name of the file to be copied
				uint8_t* dest  - pointer to the location in memory desired 
				uint32_t offset - offset into file to begin the copy
 *   OUTPUTS: given memory location will be ocupied
 *   RETURN VALUE: 0 if success or -1 on error
 *   SIDE EFFECTS: Given location in memory will be filled with entire file specified
 */ 
int32_t read_file_to_mem(uint8_t* name, uint8_t* dest, uint32_t offset){
	uint8_t buff[BLOCK_SIZE];
	dentry_t dentry;
	int32_t error;
	int i;
	uint32_t length;
	dest[20] = 0;
	//get boot block
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
	//inodes start at 4kb after boot block
	inodeStruc * inodestruc = (inodeStruc *)(bootblock + 1);
	
	
	//get dentry info
	error = read_dentry_by_name (name,&dentry);
	if(error == -1)
		return -1;
	//get the length of file from the inode	
	length = inodestruc[dentry.numInode].length;
	
	
	//cant use malloc or memory allocating functions so printing is a b***h
	//get buffer filled with data and print as many times as you need
	for(i = 0; length != 0; i++){
		//
		if( length > (uint32_t)BLOCK_SIZE){
		//dont check for error because if length is too long it will return an error
		//but you need to print in chunks so no way arond this
			error = read_data (dentry.numInode, offset +(uint32_t)(i*BLOCK_SIZE) , buff , BLOCK_SIZE);
			length = length - (uint32_t)BLOCK_SIZE;
			//copy buffer to aproppiate spot in memory
			
			memcpy(dest + (i*BLOCK_SIZE), buff,BLOCK_SIZE);
			// if the function didnt read in a full block
			//the the length requeseted was too much and you reached
			//the end of the file so return
			if(error != BLOCK_SIZE)
				return 0;
		}
		else{//length <= BLOCK_SIZE so this is the last time you need to print
			read_data (dentry.numInode, offset +(uint32_t)(i*BLOCK_SIZE) , buff , length);
			
			//copy buffer to memory
			memcpy(dest + (i*BLOCK_SIZE), buff,length);
			
			length = 0;
		}
		
		
	}
	return 0;
}


/*read_dentry_by_name
 * 
 *   DESCRIPTION: This function populates the given dentry with the correct information
					corresponding to the file name provided
 *   INPUTS: 	const uint8_t* fname - name of file for desired dentry
				dentry_t* dentry - dentry to be filled
 *   OUTPUTS: filled dentry
 *   RETURN VALUE: return 0 on success, -1 on failure
 *   SIDE EFFECTS: filled dentry
 */ 
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{	
	int i;
	
	// check if string is >32 bytes
	int nameLength = strlen((int8_t*)fname);
	if (nameLength > 32)
		return -1;
			
	//get pointer to boot block struct
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;

	//serch for file
	for(i =0; i < bootblock->num_dir_entries; i++){// for all the dir entries
		//if the names match in length and characters
		if((0 == strncmp((int8_t*) fname, (int8_t*)bootblock->dirEntrys[i].filename,nameLength))
			&& (strlen((int8_t*)fname) == strlen((int8_t*)bootblock->dirEntrys[i].filename)) ){
			//get the rest of th data 
			return read_dentry_by_index(i,dentry);
		}
		
		
		
	}
	
	// if we reach this point, then no file exists with that name
	return -1;
	
}

/*read_dentry_by_index
 * 
 *   DESCRIPTION: This function populates the given dentry with the correct information
					corresponding to the index into the directory that is provided
 *   INPUTS: 	uint32_t index - index into directory for desired dentry
				dentry_t* dentry - dentry to be filled
 *   OUTPUTS: filled dentry
 *   RETURN VALUE: return 0 on success, -1 on failure
 *   SIDE EFFECTS: filled dentry
 */ 
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
	// check to make sure index is within bounds
	if (index >= bootblock->num_dir_entries)
		return -1;

	//get file name
	strcpy((int8_t*)(*dentry).fileName, (int8_t*)bootblock->dirEntrys[index].filename);
	
	//get file type
	(*dentry).fileType = bootblock->dirEntrys[index].filetype;
	
	//get inode location
	(*dentry).numInode = bootblock->dirEntrys[index].inode_num;
	
	return 0;
}



/*read_data
 * 
 *   DESCRIPTION: reads data from the file specifed by the given inode into the 
					provided buffer. Read starts "offset" number of bytes into file
					for "lenght" number of bytes
 *   INPUTS: 	uint32_t inode - inode for coresponding file
				uint32_t offset - offset into file at which to begin
				uint8_t* buf - buffer to be filled with data that is copied
				uint32_t length - the number of bytes to read into the buffer
 *   OUTPUTS: a filled buffer with desired data
 *   RETURN VALUE: returns the number of bytes read, 0 if offset > file length, -1 on fail
 *   SIDE EFFECTS: filled buffer
 */ 
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	// NEED TO TEST WITH OFFSETS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*TODO: use offset to determine the offset in data block
			use length to determine bytes to read

			check for offset > file length; look on Piazza
	*/
	//get boot block
	int numDblocks,currb,curri,blocki;
	bootBlockStruc * bootblock = (bootBlockStruc *)FILE_SYSTEM;
	
	//inodes start at 4kb after boot block
	inodeStruc * inodestruc = (inodeStruc *)(bootblock + 1);
	
	//data blocks start after all inodes
	dataStruc * datastruc = (dataStruc *)(inodestruc + (bootblock->num_inodes));
	
	// get number of whole blocks data is in
	 numDblocks = (inodestruc[inode].length) / BLOCK_SIZE;
	 
	 //account for number of partial blocs data is in
	if(((inodestruc[inode].length)% BLOCK_SIZE) != 0)
		numDblocks++;
	//check for error
	if(inode >= (bootblock->num_inodes))
		return -1;
	//check for end of file
	if(offset >= (inodestruc[inode].length))
		return 0;
	// old code that use to remap length to make it the length of the file
	//never reached anymore
	if(((length + offset) > (inodestruc[inode].length))){
		length = (inodestruc[inode].length) - offset;
		}
	
	//				start at correct block	dont pass last block
	for(currb = (offset /(BLOCK_SIZE-1)), curri = 0; currb < numDblocks; currb++){// for all data blocs
		//if first iteration start at offset
		if(currb == (offset /(BLOCK_SIZE-1)))
			blocki = (offset%BLOCK_SIZE);
		else 
			blocki = 0;
		for(; (blocki < BLOCK_SIZE) & (curri < length); blocki++,curri++){//for whole block
		//				datatrucet  | correct inode data b #	            |  data from block
			buf[curri] = datastruc[inodestruc[inode].data_block_entry[currb]].data[blocki];
		}

	}

	return length;
}


