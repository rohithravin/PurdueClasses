const char * usage =
"                                                               \n"
"usage: http-server                                             \n"
"                                                               \n"
"Different Concurrency options [-f|-f|-p] or empty              \n"
"	[Empty] - Runs a basic server with no concurrency           \n"
"	-f - Create a new Process for each request                  \n"
"	-t Create a new Thread for each request                     \n"
"	-p Use a Pool of Threads                                    \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"    if no Port is specified then a default port 1087 is used   \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <dlfcn.h>
#include <unordered_map>

//global variables
int QueueLength = 5;
pthread_mutex_t mutexT;
pthread_mutexattr_t mutexAttribute;
unsigned long * numberOfRequests;
time_t start_time;
double * min_time;
double * max_time;
std::unordered_map<std::string,void*> openlibraries;

//helper methods
void processTimeRequest( int socket, sockaddr_in clientIPAddress );
void poolSlave(int socket);
void processRequestWithThread(int socket);
void logFile(struct sockaddr_in client, char * req);
void getPath(char docpath[], char cwd[]);
void getFileType(char docpath[], char typeOfContent[]);
bool cgiBinCase(int fd, char docpath[], char cwd[]);
void fileNotNull(int fd);
bool statsPage(int fd, char docpath[]);
void logRequest(struct sockaddr_in client, char * request);
void directoryPage(int fd, char cwd[], DIR * dirs);

int main( int argc, char ** argv ) {
  int port;
  int serverMode = -1;
 // Print usage if not enough arguments
 if ( argc < 2 ) {
   fprintf( stderr, "%s", usage );
   exit( -1 );
 }
 //IF INPUT IS JUST PORT NUMBER
 else if (argc == 2){
   port = atoi( argv[1] );
   serverMode = 0;
 }
 //IF INPUT IS PORT NUMBER AND FLAH
 else if (argc == 3){
   if (argv[1][0] == '-') {
     //FLAG FOR NEW PROCESSES
     if (argv[1][1] == 'f') {
       serverMode = 1;
     }
     //FLAG FOR NEW POOL OF THREADS
     else if (argv[1][1] == 'p') {
       serverMode = 3;
     }
     //FLAG FOR NEW THREADS
     else if (argv[1][1]  == 't') {
       serverMode = 2;
     }
     else {
       fprintf(stderr, "%s\n", usage);
       exit(1);
     }
   }
   //GET PORT NUMBER
   port = atoi(argv[2]);
 }
 // Get the port from the arguments


 // Set the IP address and port for this server
 struct sockaddr_in serverIPAddress;
 memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
 serverIPAddress.sin_family = AF_INET;
 serverIPAddress.sin_addr.s_addr = INADDR_ANY;
 serverIPAddress.sin_port = htons((u_short) port);

 // Allocate a socket
 int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
 if ( masterSocket < 0) {
   perror("socket");
   exit( -1 );
 }

 // Set socket options to reuse port. Otherwise we will
 // have to wait about 2 minutes before reusing the sae port number
 int optval = 1;
 int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR,
          (char *) &optval, sizeof( int ) );

 // Bind the socket to the IP address and port
 int error = bind( masterSocket,
       (struct sockaddr *)&serverIPAddress,
       sizeof(serverIPAddress) );
 if ( error ) {
   perror("bind");
   exit( -1 );
 }

 // Put socket in listening mode and set the
 // size of the queue of unprocessed connections
 error = listen( masterSocket, QueueLength);
 if ( error ) {
   perror("listen");
   exit( -1 );
 }

 numberOfRequests = (unsigned long *)mmap(NULL, sizeof(unsigned long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
 min_time = (double *)mmap(NULL, sizeof(double), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
 max_time = (double *)mmap(NULL, sizeof(double), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
 *numberOfRequests = 0;
 *min_time = 1000000000;
 *max_time = 0;
 time(&start_time);




//IF MODE IS REQUESTING FOR A POOL OF THREADS
if (serverMode == 3){
    pthread_t tid[QueueLength]; // CREATE THREAD ARRAY
    pthread_attr_t attr;

    pthread_mutexattr_init(&mutexAttribute); // CREATE MUTEX LOCKS AND ATTRIBUTES
    pthread_mutex_init(&mutexT, &mutexAttribute);

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); // MAKE SURE IT'S ON SCOPE_SYSTEM

    for (int i = 0; i < QueueLength; i++) {
        pthread_create(&tid[i], &attr, (void * (*)(void *))poolSlave, (void *)masterSocket); // FOR EACH THREAD, HAVE IT START AT POOLSLAVE METHOD
    }
    pthread_join(tid[0], NULL);
}
else{
  while ( 1 ) {
    // Accept incoming connections
    struct sockaddr_in clientIPAddress;
    int alen = sizeof( clientIPAddress );
    int slaveSocket = accept( masterSocket,
            (struct sockaddr *)&clientIPAddress,
            (socklen_t*)&alen);

    if ( slaveSocket < 0 ) {
      perror( "accept" );
      exit( -1 );
    }
    //NORMAL SERVER MODE
    if (serverMode == 0){
      processTimeRequest( slaveSocket, clientIPAddress );
      close( slaveSocket );
    }
    //FOR PROCESS MODE, FORK AND CALL SAME METHODS IN CHILD PROCESS
    else if (serverMode == 1){
      pid_t id = fork();
      if(id == 0){
        processTimeRequest( slaveSocket, clientIPAddress );
        close( slaveSocket );
        exit(1);
      }
      close( slaveSocket );
    }
    //FOR THREADS, CREATE A SEPARATE THREAD REQUEST METHOD AND CREATE THREAD AND RUNS ON THAT METHOD
    else if(serverMode == 2){
      pthread_t tid; // NEW THREAD
      pthread_attr_t attr; // NEW THREAD ATTRIBUTE
      pthread_attr_init(&attr);
      pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
      pthread_create(&tid, &attr, (void * (*)(void *)) processRequestWithThread, (void *) slaveSocket);
    }
  }
}
}

//run() thread method that will do the same thing normal process
void processRequestWithThread(int socket) {
  struct sockaddr_in clientIPAddress;
    processTimeRequest(socket, clientIPAddress ); // SAME THING AS A NORMAL PROCESS REQUEST WOULD DO
    close(socket);
}

//to handle a pool of threads ex. 5
void poolSlave(int socket){
  while(1){
    pthread_mutex_lock(&mutexT); // ALLOWS ONLY ONE THREAD TO GO THROUGH TO ACCEPT SOCKER AND LISTENING FOR REQUEST
    struct sockaddr_in clientIPAddress;
    int ipAddressLength = sizeof(clientIPAddress); // SAME STUFF AS GIVEN IN THE MAIN FUCNTION FROM EXAMPLE CODE
    int slaveSocket = accept(socket, (struct sockaddr *)&clientIPAddress,(socklen_t*)&ipAddressLength);
    pthread_mutex_unlock(&mutexT);
    processTimeRequest(slaveSocket, clientIPAddress); // NORMAL METHOD TO PARSE METHOD
    shutdown(slaveSocket,1);
		close(slaveSocket);
  }
}

//parese the request to get the request of the file needed by the client
int getRequest (char * s) {
    char * t = s;
    int count = 0;
    while(*t != '\0'){ // WHILE THE POINT IS NOT NULL
      if (*t == 'H' && *(t+1) == 'T' && *(t+2) == 'T' && *(t+3) == 'P') // FINDS POINTER THAT STARTS TEH 'HTTP'
        return count; //RETURNS THE INDICIES
      count++;
      t++;
    }
}

//method to handle anything that is inside the cgiBin
bool cgiBinCase(int fd, char docpath[], char cwd[]){
  //method to handle any request inside cgiBin
  //first need to check and extract the query information from the docpath and save it
  char * rpath = &docpath[0] + 14;
  char * query = strchr(rpath,'?');
  if (query != NULL){
    *query = 0;
    query++;
  }
  //check if dochpath has a cgi-bin in the path
  //else return false
  if(strstr(docpath, "cgi-bin")){
    //check if using a loading module with extension as .so
     if(strstr(docpath, ".so")){
       //send 200 resposne
       write(fd, "HTTP/1.1 200 Document follows", 29);
       write(fd, "/r/n", 2 );
       write(fd, "Server: MyServer/1.0", 20);
       write(fd, "/r/n", 2 );
       //check if hello.so module is aleady opened, if not, open it and load the module
  	 	if (openlibraries["hello.so"] == NULL) {
  	 		void * dl = dlopen("./hello.so", RTLD_LAZY);
  	 	  openlibraries["hello.so"] = dl;
  	 	  if(dl == NULL){
  	 	  	return true;
  	 	  }
  	 	}

      //loading the modules
  	 	void (*hrun)(int, char *);
  	 	hrun = (void(*)(int, char*))dlsym(openlibraries["hello.so"], "httprun");
  	 	if(hrun == NULL){
  	 		return true;
  	 	}
      //run the module to the file descriptor
  	 	(*hrun)(fd, query);
  		return true;
    }
    else{
      //when using normal cgi-bin
      int ret = fork();
      if(ret == 0){
        setenv("REQUEST_METHOD", "GET", 1);
        char * query;
        //get the query from the docpath
        if((query = strstr(docpath, "?"))){
          *(query++) = '\0';
          char * queryString = strdup(query);
          setenv("QUERY_STRING", queryString, 1);
          char * pathEnd = strstr(cwd, "?");
          *pathEnd = '\0';
        }
        //send 200 response
        write(fd, "HTTP/1.1 200 Document follows", 29);
        write(fd, "/r/n", 2 );
        write(fd, "Server: MyServer/1.0", 20);
        write(fd, "/r/n", 2 );
        dup2(fd, 1);
        //execute the cgi-bin execuable to the fd
        execvp(cwd, NULL);
        perror("execvp");
        _exit(1);
        return true;
      }
      //wait for the child process to finish completing and then return true
      int s;
      waitpid(ret, &s, 0);
      write(fd, "/r/n", 2 );
      write(fd, "/r/n", 2 );
      return true;
  }
  return false;
  }
}

//from the request create a full path of the document/file that is requested
void getPath(char docpath[], char cwd[]) {
    //get the full path of the request
    //comapre and check if it is in one of the folders and add the folders and then the requested
    //else cehck for the special cases such as / .. and /logs
    if (!strncmp(docpath, "/icons", strlen("/icons"))  || !strncmp(docpath, "/htdocs", strlen("/htdocs")) || !strncmp(docpath, "/cgi-bin", strlen("/cgi-bin"))) {
            strcat(cwd, "/http-root-dir");
            strcat(cwd, docpath);
    }
    else {
        if (!strcmp(docpath, "/")) {
            strcpy(docpath, "/index.html");
            strcat(cwd, "/http-root-dir/htdocs");
            strcat(cwd, docpath);
        }
        else if (!strcmp(docpath, "..")) {
            strcat(cwd, "/http-root-dir/htdocs");
            strcat(cwd, docpath);
        }
        else if (!strcmp(docpath, "/logs")){
            strcat(cwd, "/http-log.txt");
        }
        else {
            strcat(cwd, "/http-root-dir/htdocs");
            strcat(cwd, docpath);
            }
        }
}

//method for determining the file type of the given request from the source
void getFileType(char docpath[], char typeOfContent[]){
  //compare the docpath to see if any of of the file extensions exists in docpath,
  //then add it to the char []
  if (strstr(docpath, ".html") != 0) {
      strcpy(typeOfContent, "text/html");
  } else if (strstr(docpath, ".jpg") != 0) {
      strcpy(typeOfContent, "image/jpeg");
  } else if (strstr(docpath, ".gif") != 0) {
      strcpy(typeOfContent, "image/gif");
  }else if (strstr(docpath, ".svg") != 0) {
      strcpy(typeOfContent, "image/svg+xml");
  }else {
      strcpy(typeOfContent, "text/plain");
  }
}



//default header for sending a file
void fileNotNull(int fd, char typeOfContent[]){
  //normal header for sending a 200 response
  const char * text1 = "HTTP/1.1 200 Document follows";
  const char * text2 = "Server: MyServer/1.0";
  const char * text3 = "Content-Type: ";

  write(fd, text1,strlen(text1));
  write(fd, "\r\n", 2);
  write(fd, text2 ,strlen(text2));
  write(fd, "\r\n", 2);
  write(fd, text3 ,strlen(text3));
  write(fd, typeOfContent, strlen(typeOfContent));
  write(fd, "\r\n", 2);
  write(fd, "\r\n", 2);
}

//method to product an html page about the stats of the server
bool statsPage(int fd, char docpath[], clock_t t){
  if(strstr(docpath, "stats")){
    // get the current time
    time_t now;
    time(&now);
    char	*timeString = ctime(&now);

      fileNotNull(fd, "text/plain");
      //open as file with fd
      FILE * f = fdopen(fd, "w");
      fprintf(f, "Name: Rohith Ravindranath\n");
      fprintf(f, "Uptime: %f\n", difftime(now,start_time));
      fprintf(f, "Num Requests: %lu\n", *numberOfRequests);
      fprintf(f, "Min Request Time: %f\n", *min_time);
      fprintf(f, "Max Request Time: %f\n", *max_time);
      // output all the informatin needed
      write(fd, "/r/n", 2 );
      write(fd, "/r/n", 2 );
      fflush(f);
      fclose(f);

      return true;
  }
  return false;
}

//NOT COMPLETE
//method to handle directory page
void directoryPage(int fd, char cwd[], DIR * dirs){
  printf("directory link selected!!\n" );
}

//main method that will process the request and call the helper functions
void processTimeRequest( int fd, sockaddr_in clientIPAddress) {
 // Buffer used to store the name received from the client

 const int MaxName = 1024;
 char name[ MaxName + 1 ];
 int n;
 char docpath[MaxName];
 int doclength = 0;
 bool getDone = false;

 unsigned char newChar;
 unsigned char lastChar1 = 0;
 unsigned char lastChar2 = 0;
 unsigned char lastChar3 = 0;
 unsigned char lastChar4 = 0;

//read the request from the client using this algorithm
 while (( n = read( fd, &newChar, sizeof(newChar) ) )) {

   lastChar4 = lastChar3;
   lastChar3 = lastChar2;
   lastChar2 = lastChar1;
   lastChar1 = newChar;

   if(getDone){
     if (lastChar2 == '\r' && lastChar1 == '\n' && lastChar4 == '\r' && lastChar3 == '\n' ){
       docpath[doclength++] = '\0';
       break;
     }
     else if (newChar != ' '){
       docpath[doclength++] = newChar;
     }
   }
   if(!getDone){
     if (lastChar1 == 'T' && lastChar2 == 'E' && lastChar3 == 'G'){
       getDone = true;
     }
   }
 }

 //get and parse the request for the file
 printf("GET REQUEST=%s\n", docpath);
 int token = getRequest(docpath);
 docpath[token] = '\0';

 printf("GET NEW REQUEST=%s\n", docpath);
 //get the full path of the request and save it
 char cwd[256] = {0};
 getcwd(cwd, 256);
 getPath(docpath, cwd);

 //log the request into the log text file
 logRequest(clientIPAddress, docpath);

 //get and determine the type file so we can send it to the header
 printf( "NEW PATH=%s\n", cwd);
 char typeOfContent[MaxName + 1] = {0};
 getFileType(docpath, typeOfContent);

 //increase number of requests
 (*numberOfRequests)++;

 //check and hangle request from the cgi bin
 if(cgiBinCase(fd,docpath,cwd)){
    return;
 }

 //check and hangle request from the open directory
 DIR * dirs = opendir(cwd);
 if (dirs != NULL){
   directoryPage(fd, cwd, dirs);
   return;
 }

 clock_t t;
 //uptate the clock and min and max time
 t = clock() - t;
 double ptime = t * 1.0/CLOCKS_PER_SEC;
 if(ptime < *min_time){
   *min_time = ptime;
 }
 if(ptime > *max_time){
   *max_time = ptime;
 }

//check and hangle request from the stats page
if(statsPage(fd, docpath, t)){
   return;
 }

//open file to send the response back to the client
 FILE * file = fopen(cwd, "rb");
 if (file != NULL) {
   //printer header for the resposne 200
   fileNotNull(fd, typeOfContent);
   long count = 0;
   char c;
   //send file to the client
   while (count = read(fileno(file), &c, sizeof(c))) {
     if (write(fd, &c, sizeof(c)) != count) {
       perror("write");
       return;
     }
   }
   //end the reposne
   write(fd, "\r\n", 2);
   write(fd, "\r\n", 2);
   //close file
   fflush(file);
   fclose(file);
    }
    else {
      //when file doens't exist, send the error repsonse
      const char * message = "File not found";
      const char * message1 = "HTTP/1.1 404 File Not Found";
      const char * message2 = "Server: MyServer/1.0";
      const char * message3 = "Content-Type: text/html";

      write(fd, message1, strlen(message1));
      write(fd, "\r\n", 2);
      write(fd, message2, strlen(message2));
      write(fd, "\r\n", 2);
      write(fd, message3, strlen(message3));
      write(fd, "\r\n", 2);
      write(fd, "\r\n", 2);
      write(fd, message, strlen(message));
    }
}

//method to log request the server gets,
//will ouput the source and the request received
void logRequest(struct sockaddr_in client, char request[]){
  //create a file to output the logs of request
  FILE * logF = fopen("./http-log.txt", "a");
  //buffer for the ip address
  char buffer[4097];
	buffer[strlen(buffer)-1] = 0;
	fprintf(logF, "%d.%d.%d.%d %s %s\n", (client.sin_addr.s_addr) & 0xFF,(client.sin_addr.s_addr >> 8 ) & 0xFF,(client.sin_addr.s_addr >> 16) & 0xFF,(client.sin_addr.s_addr>> 24) & 0xFF, buffer, request);
	fclose(logF);
}
