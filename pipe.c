#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  int to_shell[2];
  pipe(to_shell);
  int return_code = fork();
  
  for(int i = 1; i < argc; i++){
    if(i == 1 && i != argc-1){
      if(return_code == 0){
	dup2(to_shell[1],1);
	close(to_shell[0]);
	exec_status = execlp(argv[i], argv[i], NULL);
      }
      else if(return_code > 0){
	close(to_shell[1]);
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
	  exit(errno);
	}
	pipe(to_shell);
	return_code = fork();
      }
      else{
	exit(1);
      }
    }
    else if(i == argc-1){
      if(return_code == 0){
	dup2(3, 0);
	close(to_shell[0]);
	close(to_shell[1]);
	exec_status = execlp(argv[i], argv[i], NULL);
      }
      else if(return_code > 0){
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
	  exit(errno);
	}
	close(3);
	close(to_shell[0]);
	close(to_shell[1]);
      }
      else{
	exit(1);
      }
    }
    else{
      if(return_code == 0){
	dup2(3, 0);
	dup2(to_shell[1], 1);
	close(to_shell[0]);
	exec_status = execlp(argv[i], argv[i], NULL);
      }
      else if(return_code > 0){
	close(3);
	close(to_shell[1]);
	dup2(4, 3);
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
	  exit(errno);
	}
	pipe(to_shell);
	return_code = fork();
      }
      else{
	exit(1);
      }
    }
  }
      
  return 0;
}
