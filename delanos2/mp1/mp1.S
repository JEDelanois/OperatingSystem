
.data					# section declaration

        # Useful offset constants for accessing members of a 
        # struct mp1_blink_struct structure
        LOCATION   = 0    
        ON_CHAR    = 2
        OFF_CHAR   = 3 
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12

        STRUCT_SIZE = 16

# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0

.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl

# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX
mp1_poke:
        
	movl    vmem_base_addr(,1),%edx
	movb    %cl,(%edx,%eax,1)
	ret

mp1_rtc_tasklet:
#void mp1_rtc_tasklet(usigned long)
#Interface: every time intrupt is called tasklet will run
#inputs: doesnt require input usigned long is just junk
#outputs:void funtion no returns
#purpose is to run through the generated string and
#decrement countdown until it reaches zero
#once zero the tasklet will flip the character and 
#outpout it to the screen
#registers
#ebx - node pointer
#edx - junk
	push %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi

	movl mp1_list_head, %ebx 	# get head of list

	tasklet_loop:
	cmpl $0, %ebx
	je tasklet_done		#if current node is null then finished
	decw COUNTDOWN(%ebx)	#dec countdown
	cmpw $0, COUNTDOWN(%ebx)
	jne tasklet_next	#countdown not zero move to next elem
	cmpw $0, STATUS(%ebx)
	jne status_one
	#here maeans status is zero so display on char
	movb ON_CHAR(%ebx), %cl
	movzwl LOCATION(%ebx), %eax
	addl %eax,%eax
	pushl %edx
	call mp1_poke		#call poke with oposite character
	popl %edx		#turned it on
	movw $1, STATUS(%ebx)  #flip status bar
	movw ON_LENGTH(%ebx),%dx
	movw %dx, COUNTDOWN(%ebx)	#set countdown again
	jmp tasklet_next	#go to next element
	status_one:
	#here meas status is one so display off char
	movb OFF_CHAR(%ebx), %cl
	movzwl LOCATION(%ebx), %eax
	addl %eax,%eax
	pushl %edx
	call mp1_poke		#call poke with oposite character
	popl %edx		#turned it off
	movw $0, STATUS(%ebx)  #flip status bar
	movw OFF_LENGTH(%ebx),%dx
	movw %dx, COUNTDOWN(%ebx)

	tasklet_next:
	movl NEXT(%ebx), %ebx
	jmp tasklet_loop

	tasklet_done:
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

mp1_ioctl:
#int mp1_ioctl(unsigned long arg, unsigned long cmd)
#The purbose of the dispatcher is to redirect the code to the apropriat
#function called.
#inputs - command_number
#outputs - returns -1 if invalid input and directs code to proper function
#registers
#eax - temp holds command_number
	movl 8(%esp),%eax
	cmpl $3,%eax
	jbe jmp_tbl1

	movl $-1,%eax 		#if gets here then invalid input
	ret
	
	jmp_tbl1:
	movl 8(%esp),%eax
	jmp *jmp_tbl(,%eax,4)	#jump to approprate function
	jmp_tbl:
	.long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find,mp1_ioctl_sync


mp1_ioctl_add:
#int mp1_ioctl_add(unsigned long arg)
#inputs - an mp1_blink_structure
#output - 0 when succesfull -1 when invalid
#this function creates a malloced copy of the sturct passed to it
#and inserts it into the head of the linked list
#ebx - pointer to malloced memory
#esi - pointer from user
#ecx - junk

	push %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl 8(%ebp), %esi	#get arg
	cmp $0, %esi
	je Neg1			#if passed null pointer return -1
	cmpw $2000, LOCATION(%esi) 
	jae Neg1		#iflocation is out of range return -1
	pushl %edx
	pushl %ecx
	pushl $STRUCT_SIZE	#push on size of struct
	call mp1_malloc
	popl %ecx
	popl %ecx
	popl %edx
	movl %eax,%ebx		#move pointer to ebx
	cmpl $0 ,%ebx
	je Neg1			#if malloc returns null then ret neg1


	pushl %edx
	pushl %ecx
	pushl $STRUCT_SIZE
	pushl %esi
	pushl %ebx
	call mp1_copy_from_user
	popl %ecx		#now the user has been copied
	popl %ecx
	popl %ecx
	popl %ecx
	popl %edx
	cmp $0,%eax
	jne Bfree		#if copy didnt work free mem and ret -1
	movw ON_LENGTH(%ebx), %cx
	movw %cx, COUNTDOWN(%ebx)	 # countdown = on_length
	movw $1, STATUS(%ebx)		#status = 1
		
	movl mp1_list_head, %ecx
	movl %ecx, NEXT(%ebx)		#move the head pointer
	movl %ebx, mp1_list_head

	#calling poke need 2*location and on_char right?

	
	movzwl LOCATION(%ebx),%eax
	addl %eax,%eax
	movb ON_CHAR(%ebx),%cl
	push %edx
	call mp1_poke
	pop %edx

	Done:
	popl %edi
	popl %esi
	popl %ebx
	movl $0 ,%eax
	leave
	ret

	Bfree:
	
	pushl %edx
	pushl %ecx
	pushl %ebx
	call mp1_free	#free memory
	pop %eax
	pop %ecx
	pop %edx

	Neg1:	
	movl $-1, %eax
	jmp Done
        
mp1_ioctl_remove:
#int mp1_ioctle_remove(unsigned long arg)
#inputs - takes an integer loction
#outputs - 0 when sucessful -1 when not
#purpose is for the funtion to look through the link list and
#remove the element with the matching location
#ebx -pointer to remove
#edx - prev pointer
#ecx - temp
	pushl %ebp
	movl %esp, %ebp
	push %ebx
	push %esi
	push %edi

	movl 8(%ebp),%ebx
	pushl %ecx	
	pushw %bx
	call loc_node_match
	popw %cx	#after here eax-remove edx-prev
	popl %ecx
	movl %eax, %ebx
	cmpl $0, %ebx
	je removeneg1	# if pointer is returned null no mem location exst
	cmpl mp1_list_head, %ebx
	jne removeNH
	movl mp1_list_head,%esi
	movl NEXT(%esi),%ecx
	pushl %edx
	pushl %ecx
	pushl mp1_list_head
	call mp1_free
	popl %ecx
	popl %ecx
	popl %edx
	movl %ecx, mp1_list_head
	jmp removeZero

	removeNH:
	movl NEXT(%ebx), %ecx
	movl %ecx, NEXT(%edx)
	pushl %edx
	pushl %ecx
	pushl %ebx
	call mp1_free
	popl %ebx
	popl %ecx
	popl %edx

	
	removeZero:
	movl $0, %eax

	removedone:
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret
	removeneg1:
	movl $-1, %eax
	jmp removedone
        
mp1_ioctl_find:
#mp1_ioctl_find(unsigned long arg)
#inputs - pointer to a blink_struct
#outputs - zero when success -1 when fail
#the purpose is to find the location in the link list that
#matches the location from the struct passed. Then copy the data
#from the link list back to the user
#ebx - pointer to user passed struct
#esi - pointer to malloced 
	
	push %ebp
	mov %esp, %ebp
	push %ebx
	push %esi
	push %edi
	
	movl 8(%ebp), %ebx
	cmpl $0, %ebx
	je Findneg1	#if passed invalid pointer return neg1
	pushl %edx
	pushl %ecx
	pushw LOCATION(%ebx)
	call loc_node_match	#find node that matches
	popw %cx
	popl %ecx
	popl %edx
	movl %eax, %esi
	cmpl $0, %esi
	je Findneg1	#if no location matches return neg1
	pushl %edx
	pushl %ecx
	pushl $STRUCT_SIZE
	pushl %esi
	pushl %ebx
	call mp1_copy_to_user
	popl %ecx
	popl %ecx
	popl %ecx
	popl %ecx
	popl %edx
	cmpl $0, %eax	#if eax is not zero then a bad copy happend
	jne Findneg1


	Finddone:
	pop %edi
	pop %esi
	pop %ebx
	leave
	ret

	Findneg1:
	movl $-1, %eax
	jmp Finddone

mp1_ioctl_sync:
#mp1_ioctl_sync(unsigned long arg)
#inputs - locations on two separate characters
#outputs - 0 when sucess -1 when fail
#the purpose of this funtion is to find the two locations given in
#the linked list then copy the data from the first to the second
#and sycronyze their blinking
#cx - first loc
#dx - second loc
#ecx - fist pointer
#edx - second pointer

	push %ebp
	mov %esp, %ebp
	push %ebx
	push %esi
	push %edi



	movl 8(%ebp), %ecx
	movw %cx, %dx	#get and setup parameters from stack
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	shrl %ecx
	pushl %edx
	pushl %ecx
	pushw %cx
	call loc_node_match	#get location of first
	popw %cx
	popl %ecx
	popl %edx
	cmp $0, %eax	#if function returned null then retrun neg1
	je Syncneg1
	movl %eax, %ecx	#ecx now has pointer to first node
	pushl %edx
	pushl %ecx
	pushw %dx
	call loc_node_match	#get location of first
	popw %dx
	popl %ecx
	popl %edx
	cmp $0, %eax	#if function returned null then retrun neg1
	je Syncneg1
	movl %eax, %edx	#edx now has pointer to second node
	#here is where the first info is copied to the second
	movw ON_LENGTH(%ecx), %ax
	movw %ax, ON_LENGTH(%edx)
	movw OFF_LENGTH(%ecx), %ax
	movw %ax, OFF_LENGTH(%edx)
	movw COUNTDOWN(%ecx), %ax
	movw %ax, COUNTDOWN(%edx)
	movw STATUS(%ecx), %ax
	movw %ax, STATUS(%edx)
	#all of the syc info ins copied now display 2nd on screen
	#DONT NEED ECX ANY MORE
	cmpw $0, STATUS(%edx)
	jne syncS1
	movb OFF_CHAR(%edx),%cl
	jmp syncCall
	syncS1:
	movb ON_CHAR(%edx), %cl
	syncCall:
	movzwl LOCATION(%edx),%eax
	add %eax,%eax
	pushl %edx
	call mp1_poke
	popl %edx
	movl $0,%eax 	#since successful return 0

	Syncdone:
	pop %edi
	pop %esi
	pop %ebx
	leave
	ret

	Syncneg1:
	movl $-1, %eax
	jmp Syncdone

loc_node_match:
#this is not a standard c funtion
#Input - takes a location passed to from the stack
#outputs - returns a pointer to that location in eax and pt to preav in edx
#the purpose of this function is to go through the linked list and return 
#a pointer to the node that matches the current location
#additionally it returns a pointer to the previous location to make 
#life simpler
	push %ebp
	mov %esp, %ebp
	push %ebx
	push %esi
	push %edi
#di location
#ebx current node
	movw 8(%ebp),%di
	mov mp1_list_head, %ebx	#get head

	LNMloop:
	cmpl $0, %ebx	#if at end and havent found it return null
	je LNMzero
	# not nul then compare locations
	cmpw LOCATION(%ebx), %di
	je LNMfound		#if found return current node
	movl %ebx,%edx		#returns pointer to prev
	movl NEXT(%ebx), %ebx	#if not found look at next node
	jmp LNMloop

	LNMfound:
	movl %ebx, %eax

	LNMdone:
	pop %edi
	pop %esi
	pop %ebx
	leave
	ret

	LNMzero:
	movl $0, %eax
	jmp LNMdone

.end
