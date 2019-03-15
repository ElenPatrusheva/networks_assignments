#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define True 1
#define False 0
#define BUF_SIZE 512
#define ARRAY_MAX_SIZE 1024
#define MAX_COMMAND_LENGTH 40

struct Stack{
	int is_active;// set 1 when stack is created
	int stack_size;
	int array[ARRAY_MAX_SIZE];
}stack;

//stack features
void create(){
	printf("Stack was created successfully.\n");
	stack.stack_size = 0;
	stack.is_active = 1;
}

int peek(){
	if (empty(stack)){
		printf("Stack is empty, there is nothing to peek.\n");
		return NULL;
	}
	return stack.array[stack.stack_size - 1];
}

void push(int data){
	if (full(stack)){
		printf("Stack is full.");
	}
	stack.array[stack.stack_size] = data;
	stack.stack_size++;
}

void pop(){
	if (empty(stack)){
		printf("Stack is empty, there is nothing to pop.\n");
		return 0;	
	}
	stack.stack_size--;
}

int empty(){
	if (stack.stack_size == 0){
		return True;
	}
	return False;
}

void display(){
	if (empty()){
		printf("Stack is empty.\n");
		return 0;
	}
	int i = 0;
	while (i < stack.stack_size){
		printf("%d ",stack.array[i]);
		i++;
	}
	printf("\n");
}

int stack_size(){
	printf("%d\n", stack.stack_size);
}

int full(){
	if (stack.stack_size == ARRAY_MAX_SIZE){
		return 1;
	}
	return 0;
}


int check_activity(){
	if (stack.is_active){
		return True;			
	}
	printf("Stack wasn't created yet.\n");
	return False;
}

void process_command(char *str){
	if (strlen(str) >= MAX_COMMAND_LENGTH | strlen(str) == 0){
		printf("No such command: %s.\n", str);
	}
	char input[MAX_COMMAND_LENGTH];
	strcpy(input, str);
	char *token = strtok(input, " ");
	char * command[2];
	int count = 0;
	while (token != NULL){
		command[count] = token;
		token = strtok(NULL, " ");
		count++;
	}
	if (token != NULL){
		printf("No such command: %s.\n", str);
		return 0;
	}
	if (count == 1){

		if (strcmp(command[0], "peek") == 0){
			if (check_activity()){
				int top = peek();
				if (top != NULL){
					printf("%d\n", top);
				}
			}
		}

		else if(strcmp(command[0], "pop") == 0){
			if (check_activity()){
				pop();
			}
		}

		else if(strcmp(command[0], "stack_size") == 0){
			if (check_activity()){
				stack_size();
			}
		}

		else if(strcmp(command[0], "display") == 0){
			if (check_activity()){
				display();
			}
		}

		else if(strcmp(command[0], "empty") == 0){
			if (check_activity()){
				if (empty()){
					printf("Stack is empty.\n");
				}
				else{
					printf("Stack is not empty.\n");
				}
			}
		}

		else if(strcmp(command[0], "create") == 0){
			create();
		}

		else {
			printf("No such command: %s.\n", str);
		}
	}

	else if (count == 2 & strcmp(command[0], "push") == 0){
		if (check_activity()){
			int number = atoi(command[1]);
			if (!(strcmp("0", command[1]) == 0) & number == 0){
				printf("It is not possible to push: %s. Only integers are allowed\n", command[1]);
				return 0;
			}
			else{
				push(number);
			}
		}
	}
	else{
		printf("No such command: %s.\n", str);
	}
}


void server_process(int fd){
	char buffer[BUF_SIZE];
	while (True){
		read(fd, buffer, BUF_SIZE);
		process_command(buffer);
	}
}

void client_process(int fd){
	char buffer[BUF_SIZE];
	while (True){
		gets(buffer);
		write(fd, buffer, BUF_SIZE);
	}
}



int main(){
	
	int fd[2];
	pipe(fd);
	int pid = 0;
	pid = fork();

	if (pid < 0){
		printf("Fork failed");
		exit(1);
	}
	else if (pid==0){
		server_process(fd[0]);
	}
	else {
		client_process(fd[1]);
	}
	return 0;

}