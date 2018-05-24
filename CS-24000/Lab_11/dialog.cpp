/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:

**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include "dialog.h"
#include <time.h>
//#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char * host;
char * room;
char * user;
char * password;
char * sport = "2035";
char * message;
char * args;
int port;
int logRoom = 0;
int logUser = 0;
char  * userInput;
char * passInput;
char * roomNameInput;
char * userMessage;

QLineEdit * enterUserButton;
QLineEdit * enterPasswordButton;
 QLineEdit * enterRoomName;

#define MAX_MESSAGES 10000
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

int lastMessage = 0;

int open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet
    socketAddress.sin_family = AF_INET;

    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
        perror("gethostbyname");
        exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
        perror("getprotobyname");
        exit(1);
    }

    // Create a tcp socket
    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
    if (connect(sock, (struct sockaddr *)&socketAddress,
            sizeof(socketAddress)) < 0) {
        perror("connect");
        exit(1);
    }

    return sock;
}

int sendCommand(char * host, int port, char * command, char * user,
        char * password, char * args, char * response) {
    int sock = open_client_socket( host, port);

    // Send command
    write(sock, command, strlen(command));
    write(sock, " ", 1);
    write(sock, user, strlen(user));
    write(sock, " ", 1);
    write(sock, password, strlen(password));
    write(sock, " ", 1);
    write(sock, args, strlen(args));
    write(sock, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    int n = 0;
    int len = 0;
    while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
        len += n;
    }

    //printf("response:%s\n", response);

    close(sock);
}

void Dialog::sendAction()
{
    userInput = strdup(enterUserButton->text().toStdString().c_str());
    passInput = strdup(enterPasswordButton->text().toStdString().c_str());
    roomNameInput = strdup(enterRoomName->text().toStdString().c_str());
    userMessage = strdup(inputMessage->toPlainText().toStdString().c_str());
    char response[MAX_RESPONSE];
    memset(response,'\0',MAX_RESPONSE);
    char input [MAX_RESPONSE];
    strcpy(input, roomNameInput);
    strcat(input, " ");
    strcat(input, userMessage);
    sendCommand(host, port, "SEND-MESSAGE", userInput, passInput, input, response);
    if(!strcmp(response,"OK\r\n")){
        //printf("hello\n");
        memset(input,'\0',MAX_RESPONSE);
        memset(response,'\0',MAX_RESPONSE);
        strcpy(input, "-1");
        strcat(input, " ");
        strcat(input, roomNameInput);
        printf("hello\n");
        sendCommand(host, port, "GET-MESSAGES", userInput, passInput, input, response);
        allMessages->clear();
        allMessages->append(response);

    }
    else if (!strcmp(response,"NO\r\n")){
        printf("Message could not be sent\n");
        logUser = 1;

    }





    printf("%s\n",input);
}


void Dialog::createNewRoom(){
    userInput = strdup(enterUserButton->text().toStdString().c_str());
    passInput = strdup(enterPasswordButton->text().toStdString().c_str());
    roomNameInput = strdup(enterRoomName->text().toStdString().c_str());
    char response[MAX_RESPONSE];
    memset(response,'\0',MAX_RESPONSE);
    sendCommand(host, port, "CREATE-ROOM", userInput, passInput, roomNameInput, response);
    if(!strcmp(response,"OK\r\n")){
        printf("Room %s added\n", roomNameInput);
        roomsList->addItem(roomNameInput);
        logUser = 1;
    }
    else if (!strcmp(response,"NO\r\n")){
        printf("Room could not be added\n");
        logUser = 1;
    }

}

void Dialog::newUserAction()
{
    userInput = strdup(enterUserButton->text().toStdString().c_str());
    passInput = strdup(enterPasswordButton->text().toStdString().c_str());
    char response[MAX_RESPONSE];
    char * input;
    memset(response,'\0',MAX_RESPONSE);
    sendCommand(host, port, "ADD-USER", userInput, passInput, "args", response);
    if(!strcmp(response,"OK\r\n")){
        printf("User %s added\n", userInput);
        memset(response,'\0',MAX_RESPONSE);
        sendCommand(host, port, "GET-ALL-USERS", userInput, passInput, "args", response);
        logUser = 1;
        usersList->clear();
        usersList->addItem(response);
        memset(response,'\0',MAX_RESPONSE);

        sendCommand(host, port, "LIST-ROOMS", userInput, passInput, "args", response);
        roomsList->clear();
        if(strcmp(response,"\r\n")){
         int count = 0;
         while(1){
             if(count == 0){
                 input = strtok(response, "\r\n");
                 count = 1;
             }
             else{
                 input = strtok(NULL, "\r\n");
             }
             if(input == NULL)
                 break;
             else
                 roomsList->addItem(input);
         }
        }
    }
    else if (!strcmp(response,"NO\r\n")){
        printf("User %s is logged in\n", userInput);
        logUser = 1;
    }
    printf("New User Button.\n");

}

void Dialog::enterRoom(){
    userInput = strdup(enterUserButton->text().toStdString().c_str());
    passInput = strdup(enterPasswordButton->text().toStdString().c_str());
    roomNameInput = strdup(roomsList->currentItem()->text().toStdString().c_str());
    char response[MAX_RESPONSE];
    char input [MAX_RESPONSE];
    memset(response,'\0',MAX_RESPONSE);
    sendCommand(host, port, "ENTER-ROOM", userInput, passInput, roomNameInput, response);
    if(!strcmp(response,"OK\r\n")){
        memset(response,'\0',MAX_RESPONSE);
        sendCommand(host, port, "GET-USERS-IN-ROOM", userInput, passInput, roomNameInput, response);
        usersList->clear();
        usersList->addItem(response);
        memset(response,'\0',MAX_RESPONSE);
        strcpy(input, "-1");
        strcat(input, " ");
        strcat(input, roomNameInput);
        sendCommand(host, port, "GET-MESSAGES", userInput, passInput, input, response);
        allMessages->clear();
        allMessages->append(response);
    }
   else if (!strcmp(response,"ERROR (No room)\r\n")){
        printf("User %s is logged in\n", userInput);
        logUser = 1;
   }
    printf("New User Button.\n");
}


void Dialog::timerAction()
{
   // printf("Timer wakeup\n");

    if(enterUserButton->text().toStdString().c_str() != NULL && enterPasswordButton->text().toStdString().c_str() != NULL && enterRoomName->text().toStdString().c_str() != NULL ){
        userInput = strdup(enterUserButton->text().toStdString().c_str());
        passInput = strdup(enterPasswordButton->text().toStdString().c_str());
        roomNameInput = strdup(enterRoomName->text().toStdString().c_str());
        //userMessage = strdup(inputMessage->toPlainText().toStdString().c_str());
        if(userInput != NULL && passInput != NULL && roomNameInput != NULL){
            char response[MAX_RESPONSE];
            memset(response,'\0',MAX_RESPONSE);
            char  input[MAX_RESPONSE];
            memset(input,'\0',MAX_RESPONSE);
            strcpy(input, "-1");
            strcat(input, " ");
            strcat(input, roomNameInput);
            printf("userInput: %s\n", userInput);
            printf("passInput: %s\n", passInput);
            printf("input: %s\n", input);
            printf("response: %s\n", response);
            if(strcmp(input, "-1 ")){
                sendCommand(host, port, "GET-MESSAGES", userInput, passInput, input, response);
                 allMessages->clear();
                allMessages->append(response);
                memset(response,'\0',MAX_RESPONSE);
                sendCommand(host, port, "GET-USERS-IN-ROOM", userInput, passInput, roomNameInput, response);
                usersList->clear();
                usersList->addItem(response);
                memset(response,'\0',MAX_RESPONSE);
                memset(input,'\0',MAX_RESPONSE);
                sendCommand(host, port, "LIST-ROOMS", userInput, passInput, "args", response);
                roomsList->clear();
                char * input2;
                if(strcmp(response,"\r\n")){
                 int count = 0;
                 while(1){
                     if(count == 0){
                         input2 = strtok(response, "\r\n");
                         count = 1;
                     }
                     else{
                         input2 = strtok(NULL, "\r\n");
                     }
                     if(input2 == NULL)
                         break;
                     else
                         roomsList->addItem(input2);
                 }
                }


            }
        }
    }

}

Dialog::Dialog()
{
    createMenu();

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout * newUser = new QHBoxLayout();
    QLabel * newUserLabel = new QLabel("Username: ");
   enterUserButton = new QLineEdit("");

    newUser ->addWidget(newUserLabel);
    newUser -> addWidget(enterUserButton);

    QHBoxLayout * enterPass = new QHBoxLayout();
    QLabel * passLabel = new QLabel("Password: ");
    enterPasswordButton = new QLineEdit("");
    enterPass ->addWidget(passLabel);
    enterPass -> addWidget(enterPasswordButton);

    QHBoxLayout * login = new QHBoxLayout();
    QPushButton * loginButton = new QPushButton("Create Account");
    login -> addWidget(loginButton);

    QHBoxLayout * newRoom = new QHBoxLayout();
    QLabel * enterRoom = new QLabel("Create New Room");
    enterRoomName = new QLineEdit("");
    QPushButton * enterRoomNameButtom = new QPushButton("Create Room");
    newRoom -> addWidget(enterRoom);
    newRoom -> addWidget(enterRoomName);
    newRoom -> addWidget(enterRoomNameButtom);

    // Rooms List
    QVBoxLayout * roomsLayout = new QVBoxLayout();
    QLabel * roomsLabel = new QLabel("Rooms");
    roomsList = new QListWidget();
    roomsLayout->addWidget(roomsLabel);
    roomsLayout->addWidget(roomsList);

    // Users List
    QVBoxLayout * usersLayout = new QVBoxLayout();
    QLabel * usersLabel = new QLabel("Users");
    usersList = new QListWidget();
    usersLayout->addWidget(usersLabel);
    usersLayout->addWidget(usersList);

    // Layout for rooms and users
    QHBoxLayout *layoutRoomsUsers = new QHBoxLayout;
    layoutRoomsUsers->addLayout(roomsLayout);
    layoutRoomsUsers->addLayout(usersLayout);

    // Textbox for all messages
    QVBoxLayout * allMessagesLayout = new QVBoxLayout();
    QLabel * allMessagesLabel = new QLabel("Messages");
    allMessages = new QTextEdit;
    allMessagesLayout->addWidget(allMessagesLabel);
    allMessagesLayout->addWidget(allMessages);

    // Textbox for input message
    QVBoxLayout * inputMessagesLayout = new QVBoxLayout();
    QLabel * inputMessagesLabel = new QLabel("Type your message:");
    inputMessage = new QTextEdit;
    inputMessagesLayout->addWidget(inputMessagesLabel);
    inputMessagesLayout->addWidget(inputMessage);

    // Send and new account buttons
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton * sendButton = new QPushButton("Send");
   // QPushButton * newUserButton = new QPushButton("New Account");
    layoutButtons->addWidget(sendButton);
    //layoutButtons->addWidget(newUserButton);

    // Setup actions for buttons
    connect(sendButton, SIGNAL (released()), this, SLOT (sendAction()));
    connect(loginButton, SIGNAL (released()), this, SLOT (newUserAction()));
    connect(enterRoomNameButtom, SIGNAL (released()), this, SLOT (createNewRoom()));
    connect(roomsList, SIGNAL(itemClicked(QListWidgetItem*)), this , SLOT(enterRoom()));

    // Add all widgets to window
    mainLayout->addLayout(layoutRoomsUsers);
    mainLayout->addLayout(allMessagesLayout);
    mainLayout->addLayout(inputMessagesLayout);
    mainLayout->addLayout(layoutButtons);
    mainLayout -> addLayout(newUser);
    mainLayout -> addLayout(enterPass);
    mainLayout -> addLayout(login);
    mainLayout -> addLayout(newRoom);

    // Populate rooms
 //   for (int i = 0; i < 20; i++) {
 //       char s[50];
 //       sprintf(s,"Room %d", i);
 //       roomsList->addItem(s);
    //}

    // Populate users
 //   for (int i = 0; i < 20; i++) {
 //       char s[50];
 //       sprintf(s,"User %d", i);
 //       usersList->addItem(s);
 //   }

//    for (int i = 0; i < 20; i++) {
//        char s[50];
//        sprintf(s,"Message %d", i);
//        allMessages->append(s);
//    }

    // Add layout to main window
    setLayout(mainLayout);

    setWindowTitle(tr("CS240 IRC Client"));
    //timer->setInterval(5000);

    messageCount = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL (timeout()), this, SLOT (timerAction()));
    timer->start(5000);
}


void Dialog::createMenu()

{
    menuBar = new QMenuBar;
    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

