#include <cstdlib>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
// C++ stuff
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// Planning to use this with the LAVA-M dataset or
//  other self-injected bugs just to verify it works.
// The plan is not to make this have great perf. or 
//  a great mutation engine.


typedef unsigned char BYTE;

/// Only changes a byte
void mutate(char *input, char *mutated_str) {
  // Change a byte
  BYTE output[strlen(input)+1];
  output[strlen(input)] = 0;
  std::cout << input << std::endl;
  for (int i = 0; i < strlen(input); ++i) {
    output[i] = input[i];
  }
  int byte_2_change = rand() % strlen(input);
  BYTE new_byte = rand() % 255;
  output[byte_2_change] = new_byte;

  for (int i = 0; i < strlen(input) + 1; ++i) {
    mutated_str[i] = (char) output[i];
  }
}

int main(int argc, char **argv) {
  // Should get the following arguments:
  //  * program to fuzz
  //  * beginning input
  //  * output directory
  // Use get_opt later -> for now, dumb arg parsing
  if (argc != 4) {
    fprintf(stderr, 
        "Incorrect number of arguments.\n./tinyfuzz $input $output $target\n");
    exit(1);
  }
  char *input_directory = argv[1];
  char *output_directory = argv[2];
  char *program_name = argv[3];

  srand (time(NULL));

  // Get all files (except those starting with '.') in input_directory
  std::vector <std::string> inputs;
  DIR *dir;
  dirent *pdir;
  dir=opendir(input_directory);
  while((pdir = readdir(dir)) != NULL) {
    if (pdir->d_name[0] == '.') {
      continue;
    }

    std::string line;
    std::string fullpath = std::string(input_directory) \
                           + std::string("/") \
                           + std::string(pdir->d_name);


    std::ifstream inputfile(fullpath);
    if (inputfile.is_open()) {
      while (getline(inputfile, line)) {
        inputs.push_back(line);
      }
      inputfile.close();
    }
  }

  // No threading yet
  //int threads = 2;
  //for (int i = 0; i < threads; ++i) {
  //  
  //}
  
  // Just run inputs through the program!
  std::cout << "Dumb fuzzing time" << std::endl;
  for (int i = 0; i < inputs.size(); ++i) {
    std::string command = "./" + std::string(program_name);
    char arg1[inputs[i].size()+1];
    mutate(const_cast<char *>(inputs[i].c_str()), arg1);
    std::cout << arg1 << std::endl << std::endl;
    char *arg0 = const_cast<char *>(command.c_str());
    char * args[]  = {arg0, arg1, nullptr};
   

    int ret = execvp(arg0, args);
    // Shouldn't return so only one thing will run
    std::cout << ret << std::endl; 
  }

  



  return(0);
}
