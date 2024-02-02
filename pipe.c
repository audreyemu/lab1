#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  if(argc == 1){
    perror("Invalid argument");
    return EINVAL; // if only one argument (just pipe) return error
  }
  int valid_dup2 = 0;
  int to_shell[2];
  pipe(to_shell);
  int return_code = fork(); // create initial pipe and fork
  
  for(int i = 1; i < argc; i++){ // for each argument
    if(i == 1 && i != argc-1){ // if it's the first one and not the last
      if(return_code == 0){ // if it's the child
	valid_dup2 = dup2(to_shell[1],1); // redirect output to write end of pipe
	if(valid_dup2 < 0){
	  perror("Dup2 failed");
	  return errno;
	}
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
	perror("Child process failed");
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	close(to_shell[1]); 
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  perror("Child process failed");
	  return WEXITSTATUS(status); // return error
	}
	pipe(to_shell);
	return_code = fork(); // create pipe and fork
      }
      else{
	perror("Fork failed");
	return errno; // if fork failed, exit
      }
    }
    else if(i == argc-1){ // if last argument
      if(return_code == 0){ // if it's the child
	valid_dup2 = dup2(3, 0); // redirect input to read end of pipe (don't worry about stdout)
	if(valid_dup2 < 0){
	  perror("Dup2 failed");
	  return errno;
	}
	close(to_shell[0]);
	close(to_shell[1]);
	execlp(argv[i], argv[i], NULL);
	perror("Child process failed");
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  perror("Child process failed");
	  return WEXITSTATUS(status); // return error
	}
	close(3);
	close(to_shell[0]);
	close(to_shell[1]); // clean up and don't make pipe/fork
      }
      else{
	perror("Fork failed");
	return errno; //if fork failed, exit
      }
    }
    else{ // if middle argument
      if(return_code == 0){ // if it's the child
	valid_dup2 = dup2(3, 0); // redirect input to read end of pipe
	if(valid_dup2 < 0){
	  perror("Dup2 failed");
	  return errno;
	}
	valid_dup2 = dup2(to_shell[1], 1); // redirect output to write end of pipe
	if(valid_dup2 < 0){
	  perror("Dup2 failed");
	  return errno;
	}
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
	perror("Child process failed");
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	close(3);
	close(to_shell[1]);
	valid_dup2 = dup2(4, 3); // so fd 3 saves read end of previous pipe
	if(valid_dup2 < 0){
	  perror("Dup2 failed");
	  return errno;
	}
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  perror("Child process failed");
	  return WEXITSTATUS(status); // return error
	}
	pipe(to_shell);
	return_code = fork(); // create pipe and fork
      }
      else{
	perror("Fork failed");
	return errno; // if the fork failed, exit
      }
    }
  }
      
  return 0;
}
