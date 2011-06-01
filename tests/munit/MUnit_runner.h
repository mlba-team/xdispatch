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

#ifndef MUNIT_RUNNER_H_
#define MUNIT_RUNNER_H_

/**
  Will print a help information
  */
void MU_printHelp();

/**
  Provides a fully implenented main
  function, testing for supported arguments
  and automatically selecting a specific, single
  test or printing the help
 */
int MU_main(int argc, char* argv[]);

/**
  Will run the test with the given number,
  counting starts at 1
  */
void MU_runTest(int no);

/**
  Will run all tests and return the return
  value of the first test to fail or 0
  if all tests succeeded.
  @param bin The path to the test binary
  @param keep_running Pass true to force running of all tests,
					  false if you want to stop after the first failure
  */
int MU_runAllTests(const char* bin, char keep_running);

/**
  Will print a list of all available tests
  */
void MU_printTests();

void MU_initFramework();

#endif /* MUNIT_RUNNER_H_ */
