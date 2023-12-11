/* Header filr */
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*Function Definitions */
// Function to read and validate decode arguments from the command line
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the stego image file has a ".bmp" extension.
    if(strcmp(strstr(argv[2], ".bmp"),".bmp") == 0)
    {
	decInfo ->stego_image_fname=argv[2];
    }
    else
    {
	return e_failure;
    }

    // Check if the secret file is provided,otherwise use the default name "output.txt".
    if(argv[3] != NULL)
    {
	if(strcmp(strstr(argv[3],".txt"),".txt") == 0)
	{
	    decInfo -> secret_fname=argv[3];
	}
    }
    else
    {
	printf("Default file name added Output.txt\n");
	decInfo -> secret_fname="output.txt";
    }
    return e_success;
}

// Function to perform the decoding procedure
Status do_decoding(DecodeInfo *decInfo)
{
    // Open the files for decoding.
    if(open_files_for_decode(decInfo)== e_success)
    {
	printf("INFO: ## Decoding Procedure Started ##\n");
	printf("Done\n");	

    }
    else
    {
	printf("ERROR: ## Decoding Procedure is not Started ##\n");
	return e_failure;      
    }

    // Decode the magic string signature
    if(decode_magic_string(decInfo) == e_success)
    {
	printf("INFO : Decoding Magic string signature\n");
	printf("DONE\n");
    }	
    else
    {
	printf("ERROR : Does not Decoding Magic string signature\n");
	return e_failure;
    }

    // Decode the file extension size.
    if(decode_secret_file_extn_size(decInfo) == e_success)
    {
	printf("INFO : Decoding File extension Size\n");
	printf("Done\n");	
    }
    else
    {
	printf("INFO : Does not Decoding File extension Size\n");
	return e_failure;
    }


    // Decode the secret file extension.
    if(decode_secret_file_extn(decInfo->extn_size,decInfo) == e_success)
    {
	printf("INFO : Decoding secret File extension\n");
	printf("Done\n");	
    }
    else
    {
	printf("ERROR : Does not Decoding secret File extension\n");
    }

    // Decode the secret file size.
    if(decode_secret_file_size(decInfo) == e_success)
    {
	printf("INFO : Decoded secret File Size\n");
	printf("Done\n");	
    }
    else
    {
	printf("ERROR : Does not Decode secret File Size\n");
	return e_failure;
    }


    // Decode the secret file data.
    if(decode_secret_file_data(decInfo) == e_success)
    {
	printf("INFO : Decoded Secret file data\n");
	printf("Done\n");	
    }
    else
    {
	printf("ERROR : Does not Decode Secret file data\n");
	return e_failure;
    }	 
    // Return e_success to indicate that the decoding procedure was successful.
    return e_success;    
}  


// Function to open stego image file and secret file for decoding
Status open_files_for_decode(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

	return e_failure;
    }
    else
    {
	printf("INFO : Open the stego.bmp\n");
	printf("Done\n");
    }
    // Secret file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

	return e_failure;
    }
    else
    {
	printf("INFO : Open the output.txt\n");
	printf("Done\n");
    }
    // No failure return e_success
    return e_success;
}

// Function to decode the magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_str[3]={0};
    char buffer[8];
    fseek(decInfo -> fptr_stego_image,54,SEEK_SET);
    for(int i=0;i<2;i++)
    {
	fread(buffer,1,8,decInfo -> fptr_stego_image);
	magic_str[i]=decode_lsb_to_bit_into_char(magic_str[i],buffer);
    }
    magic_str[2]='\0';
    // Check if the decoded magic string matches the expected value (MAGIC_STRING)
    if(strcmp(magic_str,MAGIC_STRING)==0)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Function to decode a character from LSB bits
Status decode_lsb_to_bit_into_char(char data, char *buffer)
{

    for(int i=7;i>=0;i--)
    {
	data= data | ((buffer[7-i] & 1) <<i);

    }
    return data;
}

// Function to decode the secret file extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char str[32];
    fread(str,1,32,decInfo -> fptr_stego_image);
    decInfo ->extn_size=decode_int_to_lsb(str);
    return e_success;
}

//Function to decode an integer from LSB bits
Status decode_int_to_lsb(char *image_buffer)
{
    int index=0;
    for(int i=31;i>=0;i--)
    {
	index=index|((image_buffer[31-i] & 1)<<i);
    }
    return index;
}

// Function to decode the secret file extension
Status decode_secret_file_extn(int size,DecodeInfo *decInfo)
{
    int i;
    char str[8];
    char extn[size+1];
    for(i=0;i<size;i++)
    {
	extn[i]=0;
	fread(str,1,8,decInfo ->fptr_stego_image);
	extn[i]=decode_lsb_to_bit_into_char(extn[i],str);
    }
    extn[i] ='\0';
    return e_success;
}

// Function to decode the secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char data[32];
    fread(data,1,32,decInfo ->fptr_stego_image);
    decInfo ->size_secret_file= decode_int_to_lsb(data);
    return e_success;
}

// Function to decode the secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int i;
    char str[8];
    char str1[decInfo ->size_secret_file];
    for(i=0;i<decInfo ->size_secret_file;i++)
    {
	str1[i]=0;
	fread(str,1,8,decInfo ->fptr_stego_image);
	str1[i]=decode_lsb_to_bit_into_char(str1[i],str);
    }
    str1[i]='\0';

    // Write the decoded data to the secret file
    fwrite(str1,1,decInfo ->size_secret_file,decInfo ->fptr_secret);
    return e_success;
}



