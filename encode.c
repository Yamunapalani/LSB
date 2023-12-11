/* Header file */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
// Function to read and validate encoding arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(strstr(argv[2], ".bmp"),".bmp") == 0)
    {
	encInfo ->src_image_fname=argv[2];	
    }
    else
    {
	return e_failure;
    }

    // Check if the secret file name ends with ".txt" 
    if(strcmp(strstr(argv[3],".txt"),".txt")== 0)
    {
	encInfo ->secret_fname=argv[3];
	strcpy(encInfo -> extn_secret_file, ".txt");
    }
    else
    {
	printf("Please pass the secret file\n");
	return e_failure;
    }

    // Check if the stego image file name is provided,otherwise set a default name.    
    if(argv[4] != NULL)
    {
	encInfo -> stego_image_fname=argv[4];
    }
    else
    {
	printf("Default file name added as stego.bmp\n");
	encInfo -> stego_image_fname="stego.bmp";
    }
    return e_success;
}

// Function to perform the encoding process
Status do_encoding(EncodeInfo *encInfo)
{
    // Open the files and print an prompt message if successful.
    if(open_files(encInfo)== e_success)
    {
	printf("INFO: ## Encoding Procedure Started ##\n");
    }
    else
    {
	printf("ERROR: ## Encoding Procedure is not Started ##\n");
	return e_failure;
    }

    // Check the capacity of the image for encoding.
    if(check_capacity(encInfo)== e_success)
    {
	printf("INFO : Correct capacity\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR : Not correct capacity\n");
	return e_failure;
    }

    // Copy the BMP image header for encoding.  
    if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)== e_success)
    {
	printf("INFO: Copying Image Header\n");
	printf("Done\n");
    }
    else
    {
	printf("EEROR: Does not Copying Image Header\n");	
	return e_failure;
    }

    // Encode the magic string signature.
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
    {
	printf("INFO: Encoding Magic String Signature\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROr: Does not Encoding Magic String Signature\n");
	return e_failure;
    }


    // Encode the size of the secret file extension 
    if(encode_file_extn_size(strlen(encInfo -> extn_secret_file),encInfo) == e_success)
    {
	printf("INFO: Encoding Secret.txt Extenstion Size\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR: Does not Encoding Secret.txt Extenstion Size\n");
	return e_failure; 
    }

    // Encode the secret file extension
    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
    {
	printf("INFO: Encoding secret.txt File Extenstion\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR: Does not Encoding secret.txt File Extenstion\n");
	return e_failure;
    }

    // Encode the size of the secret file
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
	printf("INFO: Encoding secret.txt File Size\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR: Does not Encoding secret.txt File Size\n");
    }

    // Encode the data from the secret file
    if(encode_secret_file_data(encInfo) == e_success)
    {
	printf("INFO: Encoding secret.txt File Data\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR: Does not Encoding secret.txt File Data\n");
    }

    // Copy the remaining image data from the source to the stego image
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
	printf("INFO: Copying Left Over Data\n");
	printf("Done\n");
    }
    else
    {
	printf("ERROR: Does not Copying Left Over Data\n");
    }
    return e_success;
}


// Function to check the capacity of the image for encoding
Status check_capacity(EncodeInfo *encInfo)
{
    // Calculate the image capacity and secret file size.
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    // Check if the image capacity is sufficient
    if(encInfo -> image_capacity >= ((strlen(MAGIC_STRING) * 8) + 32 +(strlen(encInfo -> extn_secret_file) *8) +32 + encInfo ->size_secret_file *8))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Function to copy the BMP header 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];

    // Rewind the source image file pointer
    rewind(fptr_src_image);

    // Read 54 bytes from the source image and write it to the stego image.
    fread(arr,1,54,fptr_src_image);
    fwrite(arr,1,54,fptr_dest_image);
    return e_success;
}

// Function to encode the magic string
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{

    // Call encode_data_to_image to encode the magic string.
    if((encode_data_to_image(magic_string,strlen(magic_string),encInfo ->fptr_src_image,encInfo ->fptr_stego_image))==e_success)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Function to encode data to image
Status encode_data_to_image(char *data,int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];

    // Loop the data, read from the source image
    for(int i=0;i< size;i++)
    {
	fread(arr,1,8,fptr_src_image);
	encode_byte_to_lsb(data[i],arr);
	fwrite(arr,1,8,fptr_stego_image);
    }
    return e_success;
}

// Function to encode a byte into the LSB
Status encode_byte_to_lsb(char data, char *image_buffer)
{

    for(int i=0;i<8;i++)
    {
	// Encode each bit of the data into the LSB of the image buffer.
	image_buffer[i]=((unsigned)(data & (1 <<(7-i))) >> (7-i)) | (image_buffer[i] & (~1));
    }
    return e_success;
}

// Function to encode the size of the secret file extension
Status encode_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    // Read 32 bytes from the source image.
    fread(buffer,1,32,encInfo -> fptr_src_image);
    // call the size into the LSBs of the buffer.
    encode_size_to_lsb(size,buffer);
    // Write the modified buffer to the stego image.
    fwrite(buffer,1,32,encInfo -> fptr_stego_image);
    return e_success;
}

// Function to encode a size into the LSB
Status encode_size_to_lsb(int size, char *image_buffer)
{
    // Loop through 32 bits of the size and encode them into the LSB
    for(int i=0;i<32;i++)
    {
	image_buffer[i]=((unsigned)(size & (1 <<(31-i))) >> (31-i)) | (image_buffer[i] & (~1));

    }
    return e_success;
}

// Function to encode the secret file extension
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    // Call encode_data_to_image to encode the file extension.
    if((encode_data_to_image(file_extn,strlen(file_extn),encInfo ->fptr_src_image,encInfo ->fptr_stego_image)) == e_success)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Function to encode the size of the secret file
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    // Read 32 bytes from the source image.
    fread(buffer,1,32,encInfo -> fptr_src_image);
    // call the size into the LSBs of the buffer.
    encode_size_to_lsb(file_size,buffer);
    // Write the modified buffer to the stego image.
    fwrite(buffer,1,32,encInfo -> fptr_stego_image);
    return e_success;
}

// Function to encode the data from the secret file
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo ->size_secret_file];

    // Read the secret data from the secret file.
    rewind(encInfo -> fptr_secret);
    fread(str,1,encInfo ->size_secret_file, encInfo ->fptr_secret);

    // call the data to image function.

    if(encode_data_to_image(str,encInfo -> size_secret_file,encInfo ->fptr_src_image,encInfo ->fptr_stego_image))
    {
	return e_success;
    }
    return e_success;

}

// Function to copy any remaining image data from the source to the stego image
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char remain_data;

    // Copy data one byte at a time
    while (fread(&remain_data,1,1,fptr_src_image))
    {
	fwrite(&remain_data,1,1,fptr_stego_image);
    }
    return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Function to get file size */
uint get_file_size(FILE *fptr_image)
{
    fseek(fptr_image,0,SEEK_END);
    return ftell(fptr_image);
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    else
    {
	printf("INFO : Opened the beautiful.bmp file\n");
	printf("Done\n");
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    else
    {
	printf("INFO : Opened the secret.txt file\n");
	printf("Done\n");
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    else
    {
	printf("INFO : Opened the stego.bmp file\n");
	printf("Done\n");
    }

    // No failure return e_success
    return e_success;
}
