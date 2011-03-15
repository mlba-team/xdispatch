/*
* Copyright (c) 2010 MLBA. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*     http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @MLBA_OPEN_LICENSE_HEADER_END@
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#ifndef WIN32
#	include <unistd.h>
#	include <sys/time.h>
#	include <sys/resource.h>
#	include <sys/wait.h>
#else
#	include <process.h>
#endif

#include "typedefs.h"
#include "MUnit_tools.h"
#include "MUnit_runner.h"
#include "private.h"

#ifdef WIN32
const char* LIST_TESTS = "/l";
const char* SHOW_HELP = "/?";
const char* RUN_TEST = "/t";
const char* KEEP_RUNNING = "/k";
#else
const char* LIST_TESTS = "-l";
const char* SHOW_HELP = "-h";
const char* RUN_TEST = "-t";
const char* KEEP_RUNNING = "-k";
#endif

void printHeader() {
	printf("\n   MUnit Test Framework with independent processes\n");
	printf("   (c) 2011 MLBA\n\n");
}

void MU_initFramework(){
	suite = createList();
	suite->data=NULL;
}

void MU_printHelp(){
	printHeader();
	printf("Either call the programm directly or pass an additional argument:\n");
	printf("<program> <arg>\n\n");
	printf("Argument\t Meaning\n");
	printf("-----------------------------------------------------------------\n");
	printf("none\tExecute all available tests and stop when the first failed\n");
	printf("%s\tExecute all available tests even if some of them fail\n",KEEP_RUNNING);
	printf("%s\tPrint this help\n",SHOW_HELP);
	printf("%s\tList all available tests\n",LIST_TESTS);
	printf("%s [no]\tRun only the test with the given [no]\n",RUN_TEST);
	printf("\n");
}

int MU_main(int argc, char *argv[]){
	int test_no = 0;
	bool keep_going = false;

	if(argc==2){
		if(strcmp(argv[1],SHOW_HELP)==0) {
			MU_printHelp();
			return 0;
		} else if(strcmp(argv[1],LIST_TESTS)==0) {
			MU_printTests();
			return 0;
		} else if(strcmp(argv[1],KEEP_RUNNING)==0) {
			keep_going = true;
		}
	} else if (argc==3) {
		if(strcmp(argv[1],RUN_TEST)==0) {
			test_no = atoi(argv[2]);
			//printHeader();
			//printf("Calling Test No %u\n",test_no);
			MU_runTest(test_no);
		}
		return 0;
	}

	return MU_runAllTests(argv[0],keep_going);
}

int run(int no, const char* bin){
#ifdef WIN32
	intptr_t ret;
	char test[5];
#ifdef WIN32_VS
	sprintf_s(test,5,"%u",no);
#else
        sprintf(test,"%u",no);
#endif
        printf("(%u) ",no);
	ret = _spawnl(_P_WAIT,bin,bin, RUN_TEST, test, NULL);
	if(ret==EXIT_SUCCESS)
		return 0;
	else
		return EXIT_FAILURE;
#else
	pid_t pid;
	int res = 0;
	char test[5];
	sprintf(test,"%u",no);
	printf("(%u) ",no);
        fflush(stdout);
	pid = fork();
	if (pid == -1) { // failed
		printf("Error forking test: %s\n", strerror(errno));
		exit(1);
	} else if (pid == 0) { // child process
		// launch process
		if (execlp(bin, bin, RUN_TEST, test, NULL) < 0) {
			printf("Error running test: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		exit(0);
	} else {
		if(wait(&res)==pid){
			if(WEXITSTATUS(res)==EXIT_SUCCESS)
				return 0;
			else
				return EXIT_FAILURE;
		}
	}
	return 0;
#endif
}

int MU_runAllTests(const char* bin, bool keep_running){
        item_t* curr;
	int res;
	int passed = 0, failed = 0;
	int no = 0;

	printHeader();
	printf("Running all Tests\n");
	printf("==================================\n");

	curr = suite;

	// iterate over all tests
	while(curr->next!=NULL){
		curr = curr->next;
		no++;

		// clear cache first
		fflush(stdout);
		// then run test
		res = run(no,bin);
		if(res!=EXIT_SUCCESS) {
			failed++;
			if(!keep_running){
				printf("==================================\nTEST FAILURE\n");
				printf("----------------------------------\nPassed: %u\tFailed: %u\n\n",passed,failed);
				return res;
			}
		} else
			passed++;
	}
	printf("==================================\n%s\n",failed==0?"ALL PASSED.":"!!! NOT ALL PASSED !!!");
	printf("----------------------------------\nPassed: %u\tFailed: %u\n\n",passed,failed);
	return 0;
}

void MU_runTest(int no){
	int ct = 0;
	item_t* curr = suite;
	mu_test_t* test = NULL;

	for(ct=0;ct<no;ct++){
		curr = curr->next;
	}

	if(curr==NULL){
		printHeader();
		printf(" ! Test (%u) does not exist\n\n", no);
		return;
	}

	test = (mu_test_t*)curr->data;
	test->function();
}

void MU_printTests(){
        int no = 0;
	item_t* curr;
	mu_test_t* test;

	printHeader();
	printf("Available Tests\n");
	printf("----------------------------------\n");

	curr = suite;

	while(curr->next!=NULL){
		curr = curr->next;
                no++;

                // clear cache first
                fflush(stdout);
                // then print the info
		test = (mu_test_t*)curr->data;
		printf("%u\t%s\n",no,test->name);
	}
	printf("\n");
}
