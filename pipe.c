#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  if(argc == 1){
    return EINVAL; // if only one argument (just pipe) return error
  }
  int to_shell[2];
  pipe(to_shell);
  int return_code = fork(); // create initial pipe and fork
  
  for(int i = 1; i < argc; i++){ // for each argument
    if(i == 1 && i != argc-1){ // if it's the first one and not the last
      if(return_code == 0){ // if it's the child
	dup2(to_shell[1],1); // redirect output to write end of pipe
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	close(to_shell[1]); 
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  return WEXITSTATUS(status); // return error
	}
	pipe(to_shell);
	return_code = fork(); // create pipe and fork
      }
      else{
	exit(1); // if fork failed, exit
      }
    }
    else if(i == argc-1){ // if last argument
      if(return_code == 0){ // if it's the child
	dup2(3, 0); // redirect input to read end of pipe (don't worry about stdout)
	close(to_shell[0]);
	close(to_shell[1]);
	execlp(argv[i], argv[i], NULL);
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  return WEXITSTATUS(status); // return error
	}
	close(3);
	close(to_shell[0]);
	close(to_shell[1]); // clean up and don't make pipe/fork
      }
      else{
	exit(1); //if fork failed, exit
      }
    }
    else{ // if middle argument
      if(return_code == 0){ // if it's the child
	dup2(3, 0); // redirect input to read end of pipe
	dup2(to_shell[1], 1); // redirect output to write end of pipe
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
	return errno; // if execution doesn't work, return error
      }
      else if(return_code > 0){ // if it's the parent
	close(3);
	close(to_shell[1]);
	dup2(4, 3); // so fd 3 saves read end of previous pipe
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WEXITSTATUS(status)){ // if child's exit status is not 0
	  return WEXITSTATUS(status); // return error
	}
	pipe(to_shell);
	return_code = fork(); // create pipe and fork
      }
      else{
	exit(1); // if the fork failed, exit
      }
    }
  }
      
  return 0;
}
