#include <cstdlib>
#include <sys/wait.h>
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

int crashes = 0;
int fcs = 0;
typedef unsigned char BYTE;

void fuzz(char *arg0, char** args) {
  int ret;
  pid_t thr_id = fork();
 
  if (thr_id == 0) {
    // we are in child
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execvp(arg0, args);
    fprintf(stderr, "ERROR! execvp failed\n");
  } else {
    // We are in parent
    fcs++;
    do {
      pid_t cpid = waitpid(thr_id, &ret, WUNTRACED | WCONTINUED);
      if (cpid == -1) {
        fprintf(stderr, "waitpid failed\n");
        perror("waitpid");
      }
      if (WIFEXITED(ret)) {
        // pass
      } else if (WIFSIGNALED(ret)) {
        // a crash
        crashes++;
        int exit_status = WTERMSIG(ret);
      } else if (WIFSTOPPED(ret)) {
        // pass
      } else if (WIFCONTINUED(ret)) {
        // pass
      }
    } while (!WIFEXITED(ret) && !WIFSIGNALED(ret));
  }
}

/// Only changes a byte
void mutate(char *input, char *mutated_str) {
  // Change a byte
  BYTE output[strlen(input)+1];
  output[strlen(input)] = 0;
  //std::cout << input << std::endl;
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

void add_byte(char *input, char *mutated_str) {

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

  // Just run inputs through the program!
  std::cout << "Dumb fuzzing time" << std::endl;
  time_t start = time(NULL);

  while (true) {
  for (int i = 0; i < inputs.size(); ++i) {
    std::string command = "./" + std::string(program_name);
    char arg1[500];
    mutate(const_cast<char *>(inputs[i].c_str()), arg1);
    char *arg0 = const_cast<char *>(command.c_str());
    char * args[]  = {arg0, arg1, nullptr};
  
    fuzz(arg0, args);
    time_t elapsed = time(NULL) - start;
    float fcps = (float) fcs / float(elapsed);
    printf("FCPS: %f\tCrashes: %d\t\n", fcps, crashes);
  }
  }

  



  return(0);
}
