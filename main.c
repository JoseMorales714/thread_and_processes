#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// typedef struct {
//   int mythreadindex;
//   int numberofchildren;
// } ThreadParams;

void helper();
void process1(int n);
void *sec_display(int arg);
void process2(int n);
void threadfirstpath(int its);
void threadsecpath(int its);

int main(int argc, char *argv[]) {

  // the below activates the help page

  if (strcmp(argv[1], "-h") == 0 ||
      strcmp(argv[1], "-help") == 0) { // try to get ./main only
    helper();
    printf("help cmd was triggered\n");
    return -1;
  }

  // activates debugger messages

  if (strcmp(argv[1], "-d") == 0) {
    // activate debugger
  }

  // protects against invalid n arguements

  if (argc < 4 || argc <= 0) {
    helper();
    printf("not enough commands\n");
    return -1;
  }

  int num_of_things = atoi(argv[1]), pattern = atoi(argv[3]), counter = 1;

  char *wp = argv[2];

  int t_word = strcmp(wp, "thread"), p_word = strcmp(wp, "process");

  bool is_process;

  // choose which method and sets bool var

  if (t_word == 0 || p_word == 0) {
    printf("valid thread/pattern\n");
    if (t_word == 0) {
      is_process = false;
      printf("Creating: Thread\n");
    } else {
      is_process = true;
      printf("Creating: Process\n");
    }
  } else {
    helper();
    printf("invalid thread/pattern number\n");
    return -1;
  }

  // range must be [1,256]

  if (num_of_things < 1 || num_of_things > 256) {
    helper();
    printf("invalid num_of_things range\n");
    return -1;
  } else {
    printf("valid num_of_things\n");
  }

  // might be able to delete this if

  if (pattern == 1 || pattern == 2) {
    printf("valid pattern: %d\n", pattern);
  } else {
    helper();
    printf("invalid pattern");
    return -1;
  }

  // pathway for processes
  // still need to do the if patterns

  if (is_process == true) {
    if (pattern == 1) {
      process1(num_of_things);
    } else if (pattern == 2) {
      process2(num_of_things);
    }
  } else if (is_process == false) { // pathway for threads
    if (pattern == 1) {
      printf("THREAD 1111\n");
      threadfirstpath(num_of_things);
    } else if (pattern == 2) {
      printf("THREAD 22222\n");
      threadsecpath(num_of_things);
    }
  }

  printf("\nexe\n");
  return 0;
}

// this prints out help information including syntax

void helper() {

  printf("Syntax:\n ./main <optional -h> <optional -help> <int "
         "1-256> <only valid entries 'thread' or 'process'> <requiered pattern "
         "number '1' or '2?\n");
  printf("------Example------\n");
  printf("./main -h\n ./main -help\n ./main -d 25 thread 1\n ./main 30 process "
         "2\n");
}

// pattern 2 means creating a root/main thread in which
// each child thread is originated from (linearly)

void threadfirstpath(int its) {

  // its++;
  pthread_t it_thread[500];
  int t_threads_nums[500];

  printf("Main TID: %u\n", getpid());

  // purpose of the for loop below is creating ix threads
  // each thread then executes the sec display function by pasing ix values
  // the purpose of the sec display function is to print out the # thread
  // along with its TID number

  for (int ix = 0; ix < its; ix++) {
    int to_sleep = rand() % (8 + 1 - 1) + 1; // range [1,8]
    t_threads_nums[ix] = ix;
    printf("Sleeping for: %d seconds\n", to_sleep);
    sleep(to_sleep);
    pthread_create(&it_thread[ix], NULL, sec_display, &t_threads_nums[ix]);
  }

  // purpose for the for loop below is joining each threads that is created
  for (int iy = 0; iy < its; iy++) {
    pthread_join(it_thread[iy], NULL);
    printf("Joined %d and terminated.\n", iy + 1);
  }
}

// straight forwards, prints out thread # along with its TID
// this is used by threads

void *sec_display(int arg) {

  printf("Thread #%d starting\nNew thread with pid: %lu\n", arg + 1,
         syscall(SYS_gettid));
  sleep(1);
  return 0;
}

// the below functions is for proccess pattern 1 in which there is 1 root thread
// and each thread after that is created soley from that root thread

void process1(int n) {

  pid_t process[n];

  for (int ix = 0; ix < n; ix++) {
    int to_sleep = rand() % (8 + 1 - 1) + 1; // range [1,8]
    printf("Sleeping for: %d seconds\n", to_sleep);
    sleep(to_sleep);
    process[ix] = fork();
    if (process[ix] < 0) {
      fprintf(stderr, "Fork failed, run ./main -h");
      exit(-1);
    } else if (process[ix] == 0) {
      printf("(2nd fork) Creating thread #%d, PID: %d\n", ix, getpid());
      sleep(1);
      exit(0);
    } else {
      // wait(NULL);
      // exit(0);
    }
  }
  for (int iy = 0; iy < n; iy++) {
    wait(NULL);
    printf("Process #%d terminated\n", iy);
  }
}

// the below function is for process pattern 2 in which there is 1 root thread
// that creates other processes as children, it is in a linear form

void process2(int n) {

  printf("\n");

  int counter = 1;

  for (int ix = 0; ix < n; ix++) {
    int to_sleep = rand() % (8 + 1 - 1) + 1; // range [1,8]
    pid_t proc;
    proc = fork();

    if (proc < 0) {
      fprintf(stderr, "Fork failed");
      exit(-1);
    } else if (proc == 0) { // child process
      fprintf(stderr, "Creating thread #%d", counter);
      counter++;
      fprintf(stderr, " PID: %d\n", getppid());
      printf("Sleepng for %d seconds\n", to_sleep);
      sleep(to_sleep);
    } else { // parent process
      fprintf(stderr, "This is parent process %d", getpid());
      fprintf(stderr, "\n");
      wait(NULL);
      exit(0);
    }
  }
}

// the below function is for threads pattern 2 in which there is 1 main thread
// then the children are created from the children therefore creating a
// linear tree with children originating children threads

void threadsecpath(int its) {

  printf("START sec PATH\n");

  for (int ix = 0; ix < its; ix++) {
    pthread_t *inner_thread;
    inner_thread = malloc(sizeof(pthread_t) * its);
    int *thread_ids = malloc(sizeof(int) * its);
    thread_ids[ix] = ix;
    pthread_create(&inner_thread[ix], NULL, sec_display, thread_ids[ix]);
    if (ix >= its) {
      for (int iy = 0; iy < its; iy++) {
        pthread_join(inner_thread[iy], NULL);
      }
    }
  }
}