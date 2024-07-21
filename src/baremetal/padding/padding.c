#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Function to convert a string to its hexadecimal representation
void stringToHex(const char *input, char *output) {
    int index = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        index += sprintf(output + index, "%02X", input[i]);
    }
}


// Function to convert hexadecimal character to binary
char* hexCharToInt(char hex) {
    switch(hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': case 'a': return "1010";
        case 'B': case 'b': return "1011";
        case 'C': case 'c': return "1100";
        case 'D': case 'd': return "1101";
        case 'E': case 'e': return "1110";
        case 'F': case 'f': return "1111";
        default: return NULL;
    }
}


//Function to convert a hexadecimal string into a binary string
int hexToBin(char *input, char *output) {

  output[0]='\0';
  int len = strlen(input);
  
  for(int i=0; i<len; i++) {
    char* binary = hexCharToInt(input[i]);
    //Check in case of an error
    if (binary == NULL) {
      return 1;  
    }
    strcat(output, binary);
  }
  return 0;
}


//Function used to pad the input for the MD5
int padding(char *input, char *output){

  int numb_bits = strlen(input);
  //Check overflow
  if(numb_bits > 447)
    return 1;
  
  for(int i = 0; i <= numb_bits; i++) {
    output[i]=input[i];
  }

  //Add the 1 at the end
  strcat(output,"1");
  
  //Fill the remaining part of zeroes
  int end = 512-1-64-numb_bits;
  for(int i = 0; i < end; i++) {
    strcat(output,"0");
  }

  //I have to add the last 64 bits
  int value[64];
  int num = numb_bits;
  //Convert the int into bin and appended to the string
  int i=0;
  while (num > 0) {
      value[i++] = num % 2;
      num /= 2;
   }
  for(int i=63; i>=0;i--){
    if (value[i]==1)
      strcat(output, "1");
    else
      strcat(output, "0");
  }
  
  return 0;
}


//Function used to format correctly the input for the cryptocore
void formattingToCore (const char *input, char *output) {
  
  int offset=0;
  
  for(int j=0; j<512/32; j++) {
    for(int i=3; i>=0; i--) {
      for(int z=0; z<8; z++) {
	output[8*(3-i)+z+offset]=input[8*i+z+offset];
      }
    }
    offset +=32;
  }
  output[512]='\0';
}


int main() {

  char input_string[55];
  char str_hex[111];
  char str_bin[447];
  char binary_padded[512];
  char binary_padded_formatted[512];
  int error;

  //-------------------------------------------------------------
  // CODE USED TO DEBUG 
  //Read the string from console
  /*printf("\nInsert the string to pad: ");
  scanf("%s", input_string);

  stringToHex(input_string, str_hex);
  printf("hex:\n %s\n", str_hex);
  
  error = hexToBin(str_hex, str_bin);
  if (error != 0)
    printf("!Char Unrecognized!\n");
  else
    printf("binary:\n %s\n", str_bin);

  error = padding(str_bin, binary_padded);
  if (error != 0)
    printf("!Char Overflow!\n");
  else
    printf("binary padded:\n %s\n", binary_padded);

  formattingToCore(binary_padded, binary_padded_formatted);
  if (error != 0)
    printf("!!\n");
  else
    printf("binary padded and formatted:\n %s\n", binary_padded_formatted);

  printf("\n"); */
  //END CODE USED TO DEBUG */
  //-------------------------------------------------------------
  /* CODE USED BY CONSOLE */
  printf("\nInsert the string to pad: ");
  scanf("%s", input_string);
  stringToHex(input_string, str_hex);
  error = hexToBin(str_hex, str_bin);
  error = padding(str_bin, binary_padded);
  formattingToCore(binary_padded, binary_padded_formatted);
  int start_index = 0;
  int length = 32;
  for(int i=0;i<16;i++) {
    printf("%.*s\n", length, binary_padded_formatted + start_index);
    start_index += 32;
  }
  printf("binary padded and formatted:\n %s\n", binary_padded_formatted);
  /*END CODE USED BY CONSOLE */
  //-------------------------------------------------------------
  /* CODE USED BY FILES 
  FILE *file;
  file = fopen("input.txt", "r"); // INSERT THE PATH OF THE INPUT FILE
  if (file == NULL) {
      printf("Error opening file!\n");
      return 1;
  }
  if (fgets(input_string, sizeof(input_string), file) == NULL) {
    printf("Unable to read from file.\n");
    return 1;
  }
  fclose(file);
  
  stringToHex(input_string, str_hex);
  error = hexToBin(str_hex, str_bin);
  error = padding(str_bin, binary_padded);
  formattingToCore(binary_padded, binary_padded_formatted);

  file = fopen("output.txt", "w"); // INSERT THE PATH OF THE OUTPUT FILE
  if (file == NULL) {
      printf("Error opening file!\n");
      return 1;
  }
  fprintf(file, "%s", binary_padded_formatted);
  fclose(file);
  //END CODE USED BY FILES */
  //-------------------------------------------------------------
  return 0;
}
