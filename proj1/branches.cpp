#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "branches.h"

using namespace std;

void two_bit_helper(int table[], char* branch, int* predicted_right, int index){
  int state = table[index];

  if(strcmp(branch, "NT") == 0){
    if(state == 0){
      //Strongly NonTaken
      (*predicted_right)++;
    } else if(state == 1){
      //Weakly NonTaken
      (*predicted_right)++;
      table[index] = 0;
    } else if(state == 2){
      //Weakly Taken
      //didn't predict right
      table[index] = 1;
    } else if (state == 3){
      //Strongly Taken
      //didn't predict right
      table[index] = 2;
    } else {
      fprintf(stderr, "Something went wrong with the Two Bit Predictor.\n");
      exit(1);
    }
  } else if(strcmp(branch, "T") == 0){
    if(state == 0){
      table[index] = 1;
    } else if (state == 1){
      table[index] = 2;
    } else if(state == 2){
      (*predicted_right)++;
      table[index] = 3;
    } else if (state == 3){
      (*predicted_right)++;
    } else {
      fprintf(stderr, "Something went wrong with the Two Bit Predictor.\n");
      exit(1);
    }
  }
}

unsigned int get_mask(unsigned int size){
  //create a mask based off of size of the history size
  unsigned int retVal = 0;
  switch(size)
    {
    case 3:
      retVal = 0b0000000000000111;
      break;
    case 4:
      retVal = 0b0000000000001111;
      break;
    case 5:
      retVal = 0b0000000000011111;
      break;
    case 6:
      retVal = 0b0000000000111111;
      break;
    case 7:
      retVal = 0b0000000001111111;
      break;
    case 8:
      retVal = 0b0000000011111111;
      break;
    case 9:
      retVal = 0b0000000111111111;
      break;
    case 10:
      retVal = 0b0000001111111111;
      break;
    case 11:
      retVal = 0b0000011111111111;
      break;
    default:
      fprintf(stderr, "Unknown error in gshare predictor.\n");
      exit(1);
    }
  return retVal;
}

void gshare_helper(int table[], char* branch, int index, unsigned int* history, int* predicted_right){

  int state = table[index];

    if(strcmp(branch, "NT") == 0){
      if(state == 0){
        //Strongly NonTaken
        (*predicted_right)++;

        //pad with 0
        *history <<= 1;
      } else if(state == 1){
        //Weakly NonTaken
        (*predicted_right)++;
        table[index] = 0;
        //pad with 0
        *history <<= 1;
      } else if(state == 2){
        //Weakly Taken
        //didn't predict right

        //pad with 0
        *history <<= 1;

        //go to Weak NT
        table[index] = 1;
      } else if (state == 3){
        //Strongly Taken
        //didn't predict right

        //pad with 0
        *history <<= 1;

        //go to Weak T
        table[index] = 2;
      } else {
        fprintf(stderr, "Something went wrong with the Gshare Predictor.\n");
        exit(1);
      }

    } else if(strcmp(branch, "T") == 0){
      if(state == 0){
        //Strongly NonTaken

        //add one at the end of the history
        *history <<= 1;
        *history |= 1;

        //go to Weakly NonTaken
        table[index] = 1;
      } else if (state == 1){
        //Weakly NonTaken

        //add one at the end of the history
        *history <<= 1;
        *history |= 1;

        //go to Weakly Taken State
        table[index] = 2;
      } else if(state == 2){
        // Weakly Taken
        (*predicted_right)++;

        //add one to history
        *history <<= 1;
        *history |= 1;

        //go to Strongly Taken
        table[index] = 3;
      } else if (state == 3){
        //Strongly Taken
        (*predicted_right)++;

        //add one to history
        *history <<= 1;
        *history |=1;
      } else {
        fprintf(stderr, "Something went wrong with the Gshare Predictor.\n");
        exit(1);
      }
    }
}

void gshare(int history_size, ifstream* in){
  string line;
  int table[2048];
  const char delim[2] = " ";
  char* token;
  int predicted_right = 0;
  int counter = 0;
  unsigned int history = 0;
  unsigned int mask = 0;

  //create a mask based off of size of the history size
  mask = get_mask(history_size);

  //initialize table to TT
  for(int i = 0; i < 2048; i++){
    table[i] = 3;
  }

  while(getline(*in, line)){
    char* new_line = (char*)line.c_str();
    token = strtok(new_line, delim);
    unsigned int address = hexadecimalToDecimal(token);
    unsigned int new_history = history & mask;
    int index = (address % 2048) ^ new_history;

    token = strtok(NULL, delim);

    gshare_helper(table, token, index, &history, &predicted_right);

    counter++;
  }

  printf("%d,%d; ", predicted_right, counter);

}

void two_bit(int size, std::ifstream* in){
  std::string line;
  int table[size];
  const char delim[2] = " ";
  char* token;
  int predicted_right = 0;
  int counter = 0;

  //initialize table to Taken (1).
  for (int i = 0; i < size; i++){
    table[i] = 3;
  }

  while(getline(*in, line)){
    char* new_line = (char*)line.c_str();
    token = strtok(new_line, delim);
    unsigned int address = hexadecimalToDecimal(token);
    int index = address % size;

    token = strtok(NULL, delim);

    two_bit_helper(table, token, &predicted_right, index);

    counter++;
  }
  printf("%d,%d; ", predicted_right, counter);
}

void one_bit(int size, std::ifstream* in){
  std::string line;
  int table[size];
  const char delim[2] = " ";
  char* token;
  int predicted_right = 0;
  int counter = 0;

  //initialize table to Taken (1).
  for (int i = 0; i < size; i++){
    table[i] = 1;
  }

  while(getline(*in, line)){
    char* new_line = (char*)line.c_str();
    token = strtok(new_line, delim);
    unsigned int address = hexadecimalToDecimal(token);
    int index = address % size;
    int state = table[index];

    token = strtok(NULL, delim);

    if(strcmp(token, "NT") == 0){
      if(state == 0){
        predicted_right++;
      } else {
        table[index] = 0;
      }
    } else if(strcmp(token, "T") == 0){
      if(state == 1){
        predicted_right++;
      } else {
        table[index] = 1;
      }
    }
    counter++;
  }
  printf("%d,%d; ", predicted_right, counter);
}

void always(std::string n_t, std::ifstream* in){
  std::string line;
  char* new_n_t = (char*) n_t.c_str();
  const char delim[2] = " ";
  char* token;
  int predicted_right = 0;
  int counter = 0;
  while(std::getline(*in,line)){
    char* new_line = (char*)line.c_str();
    token = strtok(new_line, delim);
    token = strtok(NULL, delim);
    counter++;
    if(strcmp(token, new_n_t) == 0){
      predicted_right++;
    }
  }
  //TODO: make it out put to file
  printf("%d,%d;\n", predicted_right, counter);
}


//retrieved function from:
// https://www.geeksforgeeks.org/program-hexadecimal-decimal/
unsigned int hexadecimalToDecimal(char hexVal[]) {
  for(unsigned int i = 2; i < strlen(hexVal); i++){
    hexVal[i-2] = hexVal[i];
  }
  hexVal[strlen(hexVal) - 2] = '\0';
  int len = strlen(hexVal);
  // Initializing base value to 1, i.e 16^0
  int base = 1;
  unsigned int dec_val = 0;

  // Extracting characters as digits from last character
  for (int i=len-1; i>=0; i--) {
    // if character lies in '0'-'9', converting
    // it to integral 0-9 by subtracting 48 from
    // ASCII value.
    if (hexVal[i]>='0' && hexVal[i]<='9') {
      dec_val += (hexVal[i] - 48)*base;
      // incrementing base by power
      base = base * 16;
    }
    // if character lies in 'A'-'F' , converting
    // it to integral 10 - 15 by subtracting 55
    // from ASCII value
    else if (hexVal[i]>='a' && hexVal[i]<='f') {
      dec_val += (hexVal[i] - 87)*base;
      // incrementing base by power
      base = base*16;
    }
  }
  return dec_val;
}


void print_binary(unsigned short n) {
  unsigned i;
  for (i = 1 << 31; i > 0; i = i / 2)
    (n & i)? printf("1"): printf("0");
}
