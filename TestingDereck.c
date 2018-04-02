#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct request {
    int value;
    struct request *next;
} request;

request *requests = NULL;

void add_request(request **requests, int *value)
{
	if (*requests == NULL)
	{
		*requests = malloc(sizeof(request));
		(*requests)->value = value;
		(*requests)->next = NULL;
	}
	else
	{
		request *current = (*requests);
		while (current->next != NULL) 
		{
        	current = current->next;
    	}
    	current->next = malloc(sizeof(request));
    	current->next->value = value;
    	current->next->next = NULL;
	}
}

void remove_first(request **requests) {
	request *old = *requests;
	*requests = (*requests)->next;
	free(old);
}

void print_requests(request **requests) {
	printf("%d\n",(*requests)->value);
	request *current = (*requests);
	while (current->next != NULL) {
		printf("%d\n",current->next->value);
        current = current->next;
    }
}

int main(int argc, char const *argv[])
{

	add_request(&requests,5);
	add_request(&requests,8);
	print_requests(&requests);
	remove_first(&requests);
	print_requests(&requests);
	return 0;
}




