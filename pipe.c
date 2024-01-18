#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  int to_shell[2];
  pipe(to_shell);
  int return_code = fork();
  for(int i = 1; i < argc; i++){
    if(i == 1){
      if(return_code == 0){
	dup2(to_shell[1],1);
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
      }
      else{
	close(to_shell[1]);
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	pipe(to_shell);
	return_code = fork();
	//close(3);
      }
    }
    else if(i == argc-1){
      if(return_code == 0){
	dup2(3, 0);
	close(to_shell[0]);
	close(to_shell[1]);
	execlp(argv[i], argv[i], NULL);
      }
      else{
	close(3);
	close(to_shell[0]);
	close(to_shell[1]);
      }
    }
    else{
      if(return_code == 0){
	dup2(3, 0);
	dup2(to_shell[1], 1);
	close(to_shell[0]);
	execlp(argv[i], argv[i], NULL);
      }
      else{
	close(3);
	close(to_shell[1]);
	dup2(4, 3);
	int pid = return_code;
	int status = 0;
	waitpid(pid, &status, 0);
	pipe(to_shell);
	return_code = fork();
	//close(3);
      }
    }
  }
  printf("Done done done\n");
      
  return 0;
}
