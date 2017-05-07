# ifndef BRAINF_STRUCT_FILE
	# define BRAINF_STRUCT_FILE 0

	# include <stdio.h>
	
	/**
	 * A node of the main list which is manipulated by the program.
	 */
	struct program_list_node {
		long long value ;														//The value of the node in the list.
		struct program_list_node * previous_node ;								//The pointer to the previous node in the list, if any.
		struct program_list_node * next_node ;									//The pointer to the next node in the list, if any.
	} ;
	
	/**
	 * A node of the loop list of the program.
	 */
	struct loop_stack_node {
		long long start_pointer_position ;										//The input pointer position for the start of the loop.
		struct loop_stack_node * outer_loop_node ;								//The pointer to the outer loop of the loop, if any.
	} ;
	
	/**
	 * The lists that are being manipulated by the program as well as the loops currently being run within the program.
	 */
	struct program_list {
		struct program_list_node * list_position ;								//The pointer to the current position in the program list.
		struct loop_stack_node * loop_top ;										//The pointer to the current position in the loop.
	} ;
	
	/**
	 * An input pointer of the program file being read.
	 */
	struct input_pointer {
		long long position ; 													//Current position of the input pointer.
		char value ;		 													//Value of the character in the current input position.
	} ;
	
	/**
	 * The program file being read and interpreted.
	 */
	struct input_file {
		FILE * source ;															//The program file being interpreted.
		struct input_pointer pointer ;											//The current input position details.
		short int status ;														//The status of the program being interpreted.
		char message [ 65536 ] ;												//A message about the current status of the program being interpreted.
	} ;
	
	/**
	 * The details of the program being interpreted.
	 */
	struct program_details {
		struct input_file file ;												//The program file.
		struct program_list list ;												//The program list
		FILE * progin ;															//The program input file.
		FILE * progout ;														//The program output file.
	} ;

#endif
