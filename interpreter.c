/**
 * Brainfuck Interpreter
 * 
 * Takes a single input file and runs the program in the command line.
 * 
 * @author Sabarna Chakravarty
 * @license MIT License
 */

//Header files 
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include "struct.h"

/**
 * The program variable containing all the needed details of the program currently being interpreted.
 */
struct program_details program ;

/**
 * The error function to handle any errors encountered.
 */
void error_procedure ( ) {
	fprintf ( program . progout , "\n\n------------------" ) ;
	fprintf ( program . progout , "\nERROR : %s" , program . file . message ) ;
	fprintf ( program . progout , "\nLOCATION : Byte %llu" , program . file . pointer . position ) ;
	fprintf ( program . progout , "\n------------------\n\n" ) ;
	exit ( program . file . status ) ;
}

/**
 * The initializer function that sets up the program variable.
 */
void interpreter_init ( char * , char * , char * ) ;

/**
 * The incremental function that increments the program stack pointer.
 */
void increment_program_list_pointer ( ) ;

/**
 * The decremental function that decrements the program stack pointer.
 */
void decrement_program_list_pointer ( ) ;

/**
 * The incremental function that increments the program stack pointer value.
 */
void increment_program_list_pointer_value ( ) ;

/**
 * The decremental function that decrements the program stack pointer value.
 */
void decrement_program_list_pointer_value ( ) ;

/**
 * The output function that shows the output the program stack pointer value.
 */
void output_program_list_pointer_value ( ) ;

/**
 * The input function that takes input for the program stack pointer value.
 */
void input_program_list_pointer_value ( ) ;

/**
 * The execution function that performs the interpretation and execution of the program.
 */
void interpreter_exec ( ) ;

/**
 * The cleanup function that deinitializes the program list.
 */
void clean_program_list ( ) ;

/**
 * The cleanup function that deinitializes the loop list.
 */
void clean_loop_stack ( ) ;

/**
 * The cleanup function that deinitializes everything in the program variable.
 */
void interpreter_clean ( ) ;

/**
 * The main function that starts the interpreter.
 */
int main ( int argc , char * argv [ ] ) {
	if ( ( argc < 2 ) || ( argc > 5 ) ) {																			//Check if parameters have been provided for interpretation and execution of a program.
		printf ( "Usage: %s <source file> [<program input file>] [<program output file>] [time]"
			, argv [ 0 ] ) ;																						//If an invalid number of arguments have been provided, the correct usage is shown.
		getchar ( ) ;																								//Take any character input ( this is blocking so the program is paused ).
		return -1 ;																									//Terminate the interpreter since nothing else can be done.
	}
	char input [ 65536 ] , output [ 65536 ] ;																		//Two character pointers which will help determine the input and outputs of the program on initialization.
	if ( ( argc >= 3 ) && ( strcmp ( argv [ 2 ] , "stdout" ) ) ) {													//Check if a second argument was provided which is supposed to be the file into which program output is written.
		strcpy ( output , argv [ 2 ] ) ;																			//Set the file as the program output since it was passed as a parameter.
	}
	else {
		output [ 0 ] = '\0' ;																						//Set the file as the program output which means to just use the standard output.
	}
	if ( ( argc >= 4 ) && ( strcmp ( argv [ 3 ] , "stdin" ) ) ) {													//Check if a third argument was provided which is supposed to be the file from which program input is read.
		strcpy ( input , argv [ 3 ] ) ;																				//Set the file as the program input since it was passed as a parameter.
	}
	else {
		input [ 0 ] = '\0' ;																						//Set the file as the program input which means to just use the standard input.
	}
	
	interpreter_init ( argv [ 1 ] , output , input ) ;																//Initialize the interpreter.

	clock_t start = clock ( ) ;																						//Get the start clock just before execution.
	interpreter_exec ( ) ;																							//Execute the interpreter.
	if ( ( argc >= 5 ) && ( ! strcmp ( argv [ 4 ] , "time" ) ) ) {													//Check if a fourth argument was provided which is supposed to tell the interpreter to show the final execution time.
		double cpu_time_used = ( ( double ) ( clock ( ) - start ) ) / CLOCKS_PER_SEC ;								//Calculate the total CPU time used for execution.
		fprintf ( program . progout , "\n\n------------------" ) ;
		fprintf ( program . progout , "\nCPU time used : %f" , cpu_time_used ) ;									//Output the total CPU time used.
		fprintf ( program . progout , "\n------------------\n\n" ) ;
	}

	interpreter_clean ( ) ;																							//Clean up after the interpreter.
	
	return program . file . status ;																				//Return the final program status.
}

void interpreter_init ( char * program_file , char * output , char * input ) {
	program . file . source = fopen ( program_file , "r" ) ;														//Open the program source file.
	if ( ! program . file . source ) {																				//Check if the file has not been opened.
		strcpy ( program . file . message , "Source file doesn't exist." ) ;										//We have hit an unrecoverable error. Set error status and message and call the error procedure function.
		program . file . status = 5 ;
		error_procedure ( ) ;
	}
	program . file . pointer . position = 0 ;																		//Set the position of the input pointer to 0 ( the start ).
	program . list . list_position = ( struct program_list_node * ) 
		malloc ( sizeof ( struct program_list_node ) ) ;															//Create a program list node for the program to operate on. More nodes will be added as required.
	program . list . list_position -> value = 0 ;																	//Set the value of the current program list node to 0.
	program . list . list_position -> next_node = NULL ;															//Set the next node after the current list node to NULL.
	program . list . list_position -> previous_node = NULL ;														//Set the previous node after the current list node to NULL.
	program . list . loop_top = NULL ;																				//Set the loop stack to empty ( since there is no loop running at the start ).
	if ( output [ 0 ] != '\0' ) {																					//Check if a program output file was specified.
		program . progout = fopen ( output , "w" ) ;																//Set the program output as the given file.
	}
	else {
		program . progout = stdout ;																				//Set the program output as the standard output.
	}
	if ( input [ 0 ] != '\0' ) {																					//Check if a program input file was specified.
		program . progin = fopen ( input , "r" ) ;																	//Set the program input as the given file.
		if ( ! program . progin ) {																					//Check if the file has not been opened.
			strcpy ( program . file . message , "Program input file doesn't exist." ) ;								//We have hit an unrecoverable error. Set error status and message and call the error procedure function.
			program . file . status = 5 ;
			error_procedure ( ) ;
		}
	}
	else {
		program . progin = stdin ;																					//Set the program output as the standard output.
	}
	program . file . status = 0 ;																					//Set the status to 0 ( meaning executing ).
	strcpy ( program . file . message , "Interpreting and executing" ) ;											//Set the status message to "Interpreting and executing".
}

void increment_program_list_pointer ( ) {
	if ( program . list . list_position -> next_node != NULL ) {													//Check if a node after the current node already exists.
		program . list . list_position = program . list . list_position -> next_node ;								//Move to the next node since it exists already.
	}
	else {																											//A node next to the current node doesn't exist.
		program . list . list_position -> next_node = ( struct program_list_node * )
			malloc ( sizeof ( struct program_list_node ) ) ;														//Create a program list node after the current node for the program to operate on.
		program . list . list_position -> next_node -> previous_node = program . list . list_position ;				//Set the previous node of the new node as the current node.
		program . list . list_position = program . list . list_position -> next_node ;								//Set the current node as the newly created node.
		program . list . list_position -> value = 0 ;																//Set the value of the new current node as 0, since it has just been created.
		program . list . list_position -> next_node = NULL ;														//Set the next node of the new current node as NULL, since no node after the current node exists.
	}
}

void decrement_program_list_pointer ( ) {
	if ( program . list . list_position -> previous_node != NULL ) {												//Check if a node before the current node already exists.
		program . list . list_position = program . list . list_position -> previous_node ;							//Move to the previous node since it exists already.
	}
	else {																											//A node previous to the current node doesn't exist.
		program . list . list_position -> previous_node = ( struct program_list_node * )
			malloc ( sizeof ( struct program_list_node ) ) ;														//Create a program list node before the current node for the program to operate on.
		program . list . list_position -> previous_node -> next_node = program . list . list_position ;				//Set the next node of the new node as the current node.
		program . list . list_position = program . list . list_position -> previous_node ;							//Set the current node as the newly created node.
		program . list . list_position -> value = 0 ;																//Set the value of the new current node as 0, since it has just been created.
		program . list . list_position -> previous_node = NULL ;													//Set the previous node of the new current node as NULL, since no node after the current node exists.
	}
}

void increment_program_list_pointer_value ( ) {
	program . list . list_position -> value ++ ;																	//Increment the value of the current program stack node.
}

void decrement_program_list_pointer_value ( ) {
	program . list . list_position -> value -- ;																	//Decrement the value of the current program stack node.
}

void output_program_list_pointer_value ( ) {
	fputc ( program . list . list_position -> value , program . progout ) ;											//Output the value of the current program stack node.
}

void input_program_list_pointer_value ( ) {
	program . list . list_position -> value = fgetc ( program . progin ) ;											//Take input of the value of the current program stack node.
}

int check_start_loop ( ) {
	if ( program . list . list_position -> value != 0 ) {															//Check if the value of the current program stack node is not 0.
		return 1 ;																									//Return 1 ( true ) as the value of the current program stack node is not 0.
	}
	else {
		return 0 ;																									//Return 0 ( false ) as the value of the current program stack node is 0.
	}
}

void start_loop ( ) {
	if ( program . list . loop_top == NULL ) {																		//Check if a loop stack has been started.
		program . list . loop_top = ( struct loop_stack_node * ) malloc ( sizeof ( struct loop_stack_node ) ) ;		//Create a new loop node to signify the start of a loop.
		program . list . loop_top -> outer_loop_node = NULL ;														//Set the inner loop of the current loop as NULL, since no inner loop has been started yet.
	}
	else if ( program . list . loop_top -> start_pointer_position != program . file . pointer . position ) {		//Check whether the current loop node is not the loop we're starting at.
		struct loop_stack_node * tmp = ( struct loop_stack_node * ) malloc ( sizeof ( struct loop_stack_node ) ) ;	//Create a new loop node to which will be at the top of the loop stack ( since it is the one within which all further executions will occur under ).
		tmp -> outer_loop_node = program . list . loop_top ;														//Set the outer loop of the newly created loop node as the current loop top node.
		program . list . loop_top  = tmp ;																			//Set the current loop top node as the newly created loop node.
	}
	program . list . loop_top -> start_pointer_position = program . file . pointer . position ;						//Set the start position of the new current loop top node as the position of the input pointer.
}

void skip_loop ( ) {
	short int skip_flag = 0 ;																						//Set a flag to check whether the skip is successful as 0 ( false )
	long long inner_loops = 0 ;																						//Set a counter for counting the presence of inner loops within the current loop.
	if ( ( program . list . loop_top != NULL ) &&
		( program . list . loop_top -> start_pointer_position == program . file . pointer . position ) ) {			//Check whether the current loop node is the loop we're starting at.
		struct loop_stack_node * tmp = program . list . loop_top ;													//Store the current loop node in a temporary variable for deletion.
		program . list . loop_top = program . list . loop_top -> outer_loop_node ;									//Set the current loop as the outer loop node.
		free ( tmp ) ;																								//Delete the old loop node from the stack.
	}
	while ( ( program . file . pointer . value = fgetc ( program . file . source ) ) != EOF ) {						//Read a character from the program source code.
		program . file . pointer . position = ftell ( program . file . source ) ;									//Get the file cursors current position.
		if ( program . file . pointer . value == '[' ) {															//Check if we reached the the start of an inner loop.
			inner_loops ++ ;																						//Increment the inner loop counter.
		}
		else if ( ( inner_loops != 0 ) && ( program . file . pointer . value == ']' ) ) {							//Check if we reached the end of an inner loop.
			inner_loops -- ;																						//Decrement the inner loop counter.
		}
		else if ( ( inner_loops == 0 ) && ( program . file . pointer . value == ']' ) ) {							//Check if we reached the end of the started loop.
			skip_flag = 1 ;																							//Set the loop skipped flag to 1 ( true )
			break ;																									//Break from the input parsing loop.
		}
	}
	if ( ! skip_flag ) {																							//Check if the loop has not been skipped.
		strcpy ( program . file . message , "Unmatched start loop encountered." ) ;									//We have hit an unrecoverable error. Set error status and message and call the error procedure function.
		program . file . status = 2 ;
		error_procedure ( ) ;
	}
}

void end_loop ( ) {
	if ( program . list . loop_top == NULL ) {																		//Check if a loop stack has been started.
		strcpy ( program . file . message , "Unmatched end loop encountered." ) ;									//We have hit an unrecoverable error. Set error status and message and call the error procedure function.
		program . file . status = 3 ;
		error_procedure ( ) ;
	}
	else {
		fseek ( program . file . source
			, ( program . list . loop_top -> start_pointer_position - 1 )
			, SEEK_SET ) ;																							//Set the file pointer to the position just behind the start of the loop.
		program . file . pointer . position = program . list . loop_top -> start_pointer_position - 1 ;				//Set the pointer position to the position just behind the start of the loop.
	}
}

void interpreter_exec ( ) {
	while ( ( program . file . pointer . value = fgetc ( program . file . source ) ) != EOF ) {						//Read a character from the program source code.
		program . file . pointer . position = ftell ( program . file . source ) ;									//Get the file cursors current position.
		switch ( program . file . pointer . value ) {																//Decide the action to perform depending on the character read from the program.
			case '>' : increment_program_list_pointer ( ) ; break ;													//Increment the program list pointer and break from the switch.
			case '<' : decrement_program_list_pointer ( ) ; break ;													//Decrement the program list pointer and break from the switch.
			case '+' : increment_program_list_pointer_value ( ) ; break ;											//Increment the program list pointer value and break from the switch.
			case '-' : decrement_program_list_pointer_value ( ) ; break ;											//Decrement the program list pointer value and break from the switch.
			case '.' : output_program_list_pointer_value ( ) ; break ;												//Output the program list pointer value and break from the switch.
			case ',' : input_program_list_pointer_value ( ) ; break ;												//Input the program list pointer value and break from the switch.
			case '[' : if ( check_start_loop ( ) ) start_loop ( ) ;													//Check if a loop should be started using the current program list pointer value and break from the switch.
					   else skip_loop ( ) ;																			//If yes, start a loop. If no, skip the loop and break from the switch.
					   break ;
			case ']' : end_loop ( ) ; break ;																		//End the current loop and break from the switch.
			default : break ;																						//Do nothing because every other character is treated as a comment and is to be ignored.
		}
	}
}

void clean_program_list ( ) {
	struct program_list_node * looper = program . list . list_position ;											//Variable that is gonna be used to loop through the list to deinitialize the nodes.
	struct program_list_node * tmp ;																				//A temporary variable to hold node address that will be deinitialized.
	while ( looper -> previous_node != NULL ) {																		//Loop through all the nodes previous to the current node.
		tmp = looper -> previous_node ;																				//Store the address of the node in a temporary variable for deinitialized.
		looper -> previous_node = looper -> previous_node -> previous_node ;										//Shift out the node to be deinitialized.
		free ( tmp ) ;																								//Deinitialized the node.
	}
	while ( looper -> next_node != NULL ) {																			//Loop through all the nodes next to the current node.
		tmp = looper -> next_node ;																					//Store the address of the node in a temporary variable for deinitialized.
		looper -> next_node = looper -> next_node -> next_node ;													//Shift out the node to be deinitialized.
		free ( tmp ) ;																								//Deinitialized the node.
	}
	free ( program . list . list_position ) ;																		//Delete the final node.
}

void clean_loop_stack ( ) {
	struct loop_stack_node * looper = program . list . loop_top ;													//Variable that is gonna be used to loop through the list to deinitialize the nodes.
	struct loop_stack_node * tmp ;																					//A temporary variable to hold node address that will be deinitialized.
	while ( looper -> outer_loop_node != NULL ) {																	//Loop through all the nodes previous to the current node.
		tmp = looper -> outer_loop_node ;																			//Store the address of the node in a temporary variable for deinitialized.
		looper -> outer_loop_node = looper -> outer_loop_node
			-> outer_loop_node ;																					//Shift out the node to be deinitialized.
		free ( tmp ) ;																								//Deinitialized the node.
	}
	free ( program . list . loop_top ) ;																			//Delete the final node.
}

void interpreter_clean ( ) {
	fclose ( program . file . source ) ;																			//Close the program source file.
	program . file . status = 1 ;																					//Set the status to 1 ( meaning completed ).
	strcpy ( program . file . message , "Completed" ) ;																//Set the status message to "Completed".
	clean_program_list ( ) ;																						//Free the program list from memory.
	if ( program . list . loop_top != NULL ) {																		//Check if an loop stack exists.
		clean_loop_stack ( ) ;																						//Free the loop list from memory since it exists.
	}
	if ( program . progout != stdout ) {																			//Check if the program output is not the standard output.
		fclose ( program . progout ) ;																				//Close the program output file.
	}
	if ( program . progin != stdin ) {																				//Check if the program input is not the standard input.
		fclose ( program . progin ) ;																				//Close the program input file.
	}
}
