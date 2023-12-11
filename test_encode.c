#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    // Declare an integer variable 'oper' to store the operation type.
    int oper;

    // Declare a structure 'encInfo' for encoding information.
    EncodeInfo encInfo;

    // Declare a structure 'decInfo' for decoding information.
    DecodeInfo decInfo;

    // Check if the command line arguement is valid.
    if (argc <= 2)
    {
	printf("Please pass the command line argument\n");
	return 0;
    }
    // Determine the operation type based on command-line arguments.
    oper = check_operation_type(argv);

    // Check the operation type.
    if (oper == 0)
    {
	// Print a message encoding is selected.
	printf("INFO : Encoding is Selected\n");
	printf("---------->>ENCODING<<-------------\n");

	if(argc >=4 && argc<=5)
	{
	    // Read and validate encoding arguments.
	    int res = read_and_validate_encode_args(argv, &encInfo);

	    // Check if argument validation is successful.
	    if (res == 0)
	    {
		printf("INFO : Read and validation Successfull\n");
		printf("Done\n");
		// Check if encoding is successful.
		if (do_encoding(&encInfo) == 0)
		{
		    // Print an encoding success message.
		    printf("INFO: ## Encoding Done Successfully ##\n");
		    printf("Done\n");
		}
		else
		{
		    // Print an encoding error message.
		    printf("ERROR: INFO: ## Encoding is not Done Successfully ##\n");
		}
	    }
	}
	else
	{
	    printf("Please pass the Required Arguements");
	}
    }
    else if (oper == 1)
    {
	// Print a message indicating decoding is selected.
	printf("INFO : Decoding is Selected\n");
	printf("---------->>DECODING<<-------------\n");

	if(argc >=3 && argc <=4)
	{

	    // Check if argument validation is successful.
	    int res = read_and_validate_decode_args(argv, &decInfo);
	    if (res == 0)
	    {
		printf("INFO : Read and validation is Successfull\n");
		printf("Done\n");

		// Check if decoding is successful.
		if (do_decoding(&decInfo) == 0)
		{
		    // Print a decoding success message.
		    printf("INFO: ## Decoding Done Successfully ##\n");
		    printf("Done\n");
		}
		else
		{
		    // Print a decoding error message.
		    printf("INFO: ## Decoding not Done Successfully ##\n");
		    printf("Done\n");
		}
	    }
	}
	else
	{
	    printf("Please pass the Required Arguements");
	}
    }
    else
    {
	printf("Unsuppoted file\n");
	return 0;
    }

    return 0;
}
OperationType check_operation_type(char *argv[])
{
    // Check if the first argument is "-e".
    if (strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }

    // Check if the first argument is "-d".
    else if (strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }

    //If it is not "-e" or "-d" is found, return e_unsupported.
    else
    {
	return e_unsupported;
    }
}
