
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "IRCServer.h"

int QueueLength = 5;
int countUser =0;
int countRoom;
const char* argsRoomName = (char*)malloc(100*sizeof(char));
char* currentMessage = (char*)malloc(100*sizeof(char));
char * lastMsg = (char*)malloc(20*sizeof(char));


struct User{
	char * username;
	char * password;
	int rooms[50];
};
char username[100];
char password[100];
struct Rooms{
	char * users[100];
	char * roomName;
	char * messages[100];
	int userRoomCount;
	int currentMessageCount;
	int overPop;	

};
typedef Rooms Rooms;
Rooms * RoomArray;
typedef User User;
User * UserArray;


//test

int
IRCServer::open_server_socket(int port) {

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

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
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
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
	
}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	commandLineLength--;
    commandLine[ commandLineLength ] = 0;
    const char * command = strtok(commandLine, " ");
    const char * user = strtok(NULL, " ");
    const char * password = strtok(NULL, " ");
    const char * args = strtok(NULL, " ");
    char * temp = (char*)malloc(20*sizeof(char));
    temp = strtok(NULL, " ");
    int same = 0;
    do{
    	if(temp == NULL)
    		break;
    	if(same == 0){
    		strcpy(currentMessage,temp);
    		temp = strtok(NULL, " ");
    		if(temp == NULL)
    			break;
    		strcat(currentMessage," ");
    		same = 1;
    	}
    	else{
    		strcat(currentMessage,temp);
    		temp = strtok(NULL, " ");
    		if(temp == NULL)
    			break;
    		strcat(currentMessage," ");
    	}
    }while(temp != NULL);
	printf("RECEIVED: %s\n", commandLine);
	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);
	printf("currentMessage=%s\n", currentMessage);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRooms(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		strcpy(lastMsg,args);
		getMessages(fd, user, password, strtok(currentMessage, " "));
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}
	close(fd);	
}

void
IRCServer::initialize() {	
	countUser = 0;
	UserArray = (User *) malloc(1000 * sizeof(User));
	FILE * fd = fopen("password.txt","a+");
	char * c = (char *) malloc (100 * sizeof(char));
	int x = 0;
	while((c = nextWord(fd)) != NULL){
		if(x == 0){
			UserArray[countUser].username = strdup(c);
			x = 1;
			continue;
		}
		else if (x == 1){
			UserArray[countUser].password = strdup(c);
			x = 1;
			countUser++;
			continue;
		}
	}
	countRoom = 0;
	RoomArray = (Rooms *) malloc(100 * sizeof(Rooms));
	for(int y = 0; y < 100; y++){
		for(int z = 0; z < 100; z++)
			RoomArray[y].messages[z] = (char *) malloc(1024 * (sizeof(char)));
	}
	fclose(fd);
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password){
	for(int x = 0; x < countUser; x++){
		if(!strcmp(UserArray[x].username,user)){
			if(!strcmp(UserArray[x].password,password))
				return true;
			else
				return false;
		}
	}
	return false; 
}

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args) {
	int caution = 0;
	FILE * file = fopen("password.txt","a+");
	char * userName = strdup(user);
	char * passWord = strdup(password);
	for(int x = 0; x < countUser; x++){
		if(strcmp(UserArray[x].username, userName) == 0){
			caution = 1;
			break;
		}
	}
	if(caution != 1){
		UserArray[countUser].username = userName;
		UserArray[countUser].password = passWord;
		fprintf(file, "%s %s\n",userName,passWord);
		countUser++;
	}
	fclose(file);
	const char * msg =  "OK\r\n";
	write(fd, msg, strlen(msg));
	return;		
}


void 
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args) {
	int caution = 0;
	if (checkPassword(fd, user, password)) {
		for(int x = 0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName,args)){
				caution = 1;
				printf("args %s, struct %s, args %d, struct %d\n", args, RoomArray[x].roomName, strlen(args), strlen(RoomArray[x].roomName));
				break;
			}
		}
		if( caution != 1){
			RoomArray[countRoom].roomName = strdup(args);
			countRoom++;
			printf("%d\n",countRoom);
			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
		}
		else{
			printf("Room already exists\n");
			const char * str = "NO\r\n";
			write(fd, str, strlen(str));
			return;
		}
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args) {
	int temRoomPosition = 0;
	int cautionA = 0;
	int cautionB = 0;
	char * str;
	if (checkPassword(fd, user, password)) {
		for(int x = 0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName,args)){
				temRoomPosition = x;
				cautionB = 1;
				break;
			}
		}
		if(cautionB != 1 ){	
			const char * msg =  "ERROR (No room)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
		for(int x = 0 ;x < RoomArray[temRoomPosition].userRoomCount; x++){
			if(!strcmp(RoomArray[temRoomPosition].users[x],user)){
				cautionA =1;
				break;
			}
		}
		if( cautionA != 1){	
			RoomArray[temRoomPosition].users[RoomArray[temRoomPosition].userRoomCount] = strdup(user);
			RoomArray[temRoomPosition].userRoomCount++;
			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
		}
		else{
			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
		}
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}	
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args) {
	int temRoomPosition = 0;
	int temUserPosition = 0;
	int cautionA = 0;
	int cautionB = 0;
	char * message = (char *) malloc(40 * sizeof(char));
	if (checkPassword(fd, user, password)) {
		for(int x = 0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName, args)){
				temRoomPosition = x;
				cautionA =1;
				break;
			}
		}
		if(cautionA != 1){
			printf("Room doesn't exists\n");
			const char * str = "NO\r\n";
			write(fd, str, strlen(str));
			return;
		}
		else{
			for(int x = 0; x < RoomArray[temRoomPosition].userRoomCount;x++){
				if(!strcmp(RoomArray[temRoomPosition].users[x],user)){
					temUserPosition = x;
					cautionB = 1;
					break;
				}
			}
			if(cautionB == 1){
				for(int x = temUserPosition; x < RoomArray[temRoomPosition].userRoomCount; x++)
					RoomArray[temRoomPosition].users[x] = RoomArray[temRoomPosition].users[x+1];
				RoomArray[temRoomPosition].userRoomCount--;
				const char * str = "OK\r\n";
				write(fd, str, strlen(str));

			}
			else{
				const char * str = "ERROR (No user in room)\r\n";
				write(fd, str, strlen(str));
				return;
			}
		}
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args) {
	int temRoomPosition = 0;
	int cautionA = 0;
	int cautionB = 0;
	char * tempMessage = (char *) malloc (1000 * sizeof(char));
	if (checkPassword(fd, user, password)) {
		for(int x = 0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName,args)){
				temRoomPosition = x;
				cautionA = 1;
				break;
			}
		}
		if(cautionA != 1){
			printf("Room doesn't exists\n");
			const char * str = "NO\r\n";
			write(fd, str, strlen(str));
			return;
		}
		for(int x = 0; x < RoomArray[temRoomPosition].userRoomCount; x++){
			if (!strcmp(RoomArray[temRoomPosition].users[x],user)){
				cautionB = 1;
				break;
			}
		}
		if(cautionB != 1){
			const char * str = "ERROR (user not in room)\r\n";
			write(fd, str, strlen(str));
			return;
		}
		if(RoomArray[temRoomPosition].currentMessageCount < 100){
			char num[5];
			toString(num, RoomArray[temRoomPosition].currentMessageCount);
			strcpy(tempMessage,num);
			strcat(tempMessage, " ");
			strcat(tempMessage,user);
			strcat(tempMessage, " ");
			strcat(tempMessage, currentMessage);
			RoomArray[temRoomPosition].messages[RoomArray[temRoomPosition].currentMessageCount] = strdup(tempMessage);
			RoomArray[temRoomPosition].currentMessageCount++;
			const char * messg = "OK\r\n";
			write(fd, messg, strlen(messg));

		}
		else{
			int x;
			for(x = 1; x < 100; x++)
				RoomArray[temRoomPosition].messages[x] = RoomArray[temRoomPosition].messages[x-1];
			strcpy(tempMessage, user);
			strcat(tempMessage, " ");
			strcat(tempMessage, currentMessage);
			RoomArray[temRoomPosition].messages[x] = strdup(tempMessage);
			RoomArray[temRoomPosition].currentMessageCount++;
			RoomArray[temRoomPosition].overPop++;
			const char * messg = "OK\r\n";
			write(fd, messg, strlen(messg));
		}
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args) {
	int temRoomPosition = 0;
	int cautionA = 0 ;
	int cautionB = 0;
	int lastMessageNum;
	lastMessageNum = atoi(lastMsg) +1;
	char * msg = (char *) malloc (1000 * sizeof(char));
	if (checkPassword(fd, user, password)) {
		for(int x = 0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName, args))
			{
				temRoomPosition = x;
				cautionA = 1;
				break;
			}
		}
		if(cautionA != 1){
			printf("Room doesn't exists\n");
			const char * str = "NO\r\n";
			write(fd, str, strlen(str));
			return;
		}
		for(int x = 0; x < RoomArray[temRoomPosition].userRoomCount; x++){
			if (!strcmp(RoomArray[temRoomPosition].users[x],user)){
				cautionB = 1;
				break;
			}
		}
		if(cautionB != 1){
			const char * str = "ERROR (User not in room)\r\n";
			write(fd, str, strlen(str));
			return;
		}
		int cautionC = 0;
		for(int x = lastMessageNum; x < RoomArray[temRoomPosition].currentMessageCount;x++){
			cautionC = 1;
			strcpy(msg, RoomArray[temRoomPosition].messages[x]);
			strcat(msg, "\r\n");
			write(fd, msg, strlen(msg));
			printf("%s\n",msg);
		}
		if(cautionC != 1){
			strcpy(msg, "NO-NEW-MESSAGES\r\n");
			write(fd,msg,strlen(msg));
			return;
		}
		strcpy(msg,"\r\n");
		write(fd,msg,strlen(msg));
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}


void
IRCServer::listRooms(int fd, const char * user, const char * password, const char * args){
	if (checkPassword(fd, user, password)) {
		for(int x =0; x< countRoom;x++){
			printf("%s ",RoomArray[x].roomName);
			printf("%d\n",countRoom);
			char * str = (char *) malloc(40 * sizeof(char));
			strcpy(str,RoomArray[x].roomName);
			strcat(str,"\r\n");
			write(fd,str,strlen(str));
			printf("message to user: %s\n",str);
		}
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args) {
	int temRoomPosition;
	int caution = 0;
	if (checkPassword(fd, user, password)) {
		for(int  x =0; x < countRoom; x++){
			if(!strcmp(RoomArray[x].roomName,args)){
				temRoomPosition = x;
				caution =1;
				break;
			}
		}
		int cautionB = 0;
		if(caution != 1)
			return;
		char * tempUserName = (char *) malloc (80 * sizeof(char));
		for(int x = 0; x < RoomArray[temRoomPosition].userRoomCount -1; x++){
			for(int y = x +1; y < RoomArray[temRoomPosition].userRoomCount; y++){
				if(strcmp(RoomArray[temRoomPosition].users[x],RoomArray[temRoomPosition].users[y]) > 0){
					tempUserName = strdup(RoomArray[temRoomPosition].users[x]);
					RoomArray[temRoomPosition].users[x] = strdup(RoomArray[temRoomPosition].users[y]);
					RoomArray[temRoomPosition].users[y] = strdup(tempUserName);
				}
			}
		}
		char * str = (char *) malloc (80 * sizeof(char));
		for(int x = 0; x < RoomArray[temRoomPosition].userRoomCount; x++){
			strcpy(str,RoomArray[temRoomPosition].users[x]);
			strcat(str, "\r\n");
			write(fd, str,strlen(str));
			printf("%s\n",RoomArray[temRoomPosition].users[x]);
		}
		strcpy(str,"\r\n");
		write(fd,str,strlen(str));
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void 
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args) {
	char * usernameTemp = (char *) malloc(100 * sizeof (char));
	char * passwordTemp = (char *) malloc(100 * sizeof (char));
	if(checkPassword(fd,user,password)){
		for(int x = 0; x < countUser-1; x++){
			for(int y = x +1; y < countUser; y++){
				if(strcmp(UserArray[x].username,UserArray[y].username) > 0){
					usernameTemp = strdup(UserArray[x].username);
					passwordTemp = strdup(UserArray[x].password);
					UserArray[x].username = strdup(UserArray[y].username);
					UserArray[x].password = strdup(UserArray[y].password);
					UserArray[y].username = strdup(usernameTemp);
					UserArray[y].password = strdup(passwordTemp);
				}
			}
		}
		for(int x = 0;  x < countUser; x++){
			char * str = (char *) malloc (80 * sizeof(char));
			strcpy(str,UserArray[x].username);
			strcat(str,"\r\n");
			write(fd, str, strlen(str));
			printf("%s\r\n",UserArray[x].username);
		}
		write(fd,"\r\n",strlen("\r\n"));
		
	}
	else{
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
}



#define MAXWORD 200
char word[MAXWORD];
int wordLength;
char * IRCServer::nextWord (FILE * fd) {
	int x = 0;
	int y = 0;
	while ((x = fgetc(fd)) != EOF) {
		if (x == ' ' || x == '\t' || x == '\n' || x == '\r') {
			if (y > 0) {
				word[y] = '\0';
				return word;
			}
		} 
		else {
			word[y] = x;
			y++; 
		}
	}
	if (y > 0) {
		word[y] = '\0';
		return word;
	}
	return NULL;
}

void 
IRCServer::toString(char str[], int num) {
    int x;
    int rem;
    int length = 0;
    int value;
    value = num;
    if (value == 0) {
    	str[length] = '0';
		str[length + 1] = '\0';
		return;
    }
	while (value != 0) {
		length++;
		value /= 10;
	}
	for (x = 0; x < length; x++) {
		rem = num % 10;
		num = num / 10;
		str[length - (x + 1)] = rem + '0';
	}
	str[length] = '\0';
}




