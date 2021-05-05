#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include "communicator.hpp"

#include <SFML/Network.hpp>

bool requestConnection(char* address, int port,char* page, char* id, char* code){
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    std::string body="id=";
    body=body+(std::string)id+"&code="+code+"&mobile=true";

    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);

    if(response.getBody().find("REQUEST ACCEPTED")!=std::string::npos)
        return true;
    else
        return false;
}

int getNumOfClients(char* address, int port, char* page, char* userName, char* password){
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    std::string body="userName=";
    body=body+(std::string)userName+"&password="+password+"&mobile=true&num=true";

    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);
    std::string b=response.getBody();

    int ret=0;
    
    char *ptr=(char*)b.c_str();

    ret=strtol(ptr, (char**) NULL, 10);

    return ret;
}

void getClientList(char* address, int port, char* page, char* id, char* code, int size, char* r[]){
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    std::string body="userName=";
    body=body+(std::string)id+"&password="+code+"&mobile=true";

    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);
    std::string b=response.getBody();

    std::vector<std::string> vec;

    while(b.length()>0){
        int del=b.find("\n");
        vec.push_back(b.substr(0,del));
        b.replace(0, b.substr(0,del).length()+1,"");
    }

    for(int i=0;i<size;i++){
        char* o=(char*)malloc(vec[i].length() * sizeof(char));
        strcpy(o, vec[i].c_str());
        r[i]= o;
    }

}

int getNumOfMessages(char* address, int port,char* page, char* userName, char* password, 
char* chatWith, int reference){
    
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    char buf[20];
    sprintf(buf,"%d",reference);
    char* ptr=strtok(buf, "\0");

    std::string body="userName=";
    body=body+(std::string)userName+"&password="+password+"&mobile=true&num=true&chatWith="+chatWith;
    body+="&reference=";
    body+=ptr;
    
    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);
    std::string b=response.getBody();

    int ret=0;
    
    char *p=(char*)b.c_str();

    ret=strtol(p, (char**) NULL, 10);

    return --ret;
}

void getMessages(char* address, int port,char* page, char* userName, char* password,
    char* chatWith, int reference, char* sender[], char* message[]){
    
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    char buf[20];
    sprintf(buf,"%d",reference);
    char* ptr=strtok(buf, "\0");

    std::string body="userName=";
    body=body+(std::string)userName+"&password="+password+"&mobile=true&chatWith="+chatWith;
    body+="&reference=";
    body+=ptr;

    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);
    std::string b= response.getBody();

    std::vector<std::string> source;
    std::vector<std::string> messages;

    while(b.length()>0){
        int del=b.find("\n");
        int sep=b.find("|||||");

        std::string part=b.substr(0,del);

        if(b.substr(0,del).find("|||||")!=std::string::npos){
            source.push_back(part.substr(0,sep));
            messages.push_back(part.substr(sep+5,del-1));
        }
            
        b.replace(0, b.substr(0,del).length()+1,"");
    }

    for(int i=0;i< (int)source.size();i++){
        const char* first=source[i].c_str();
        const char* second=messages[i].c_str();
        char* s=(char*)malloc(sizeof(char) * source[i].length());
        char* m=(char*)malloc(sizeof(char) * messages[i].length());
        strcpy(s, first);
        strcpy(m, second);
        sender[i]=s;
        message[i]=m;
    }

}

void sendMessages(char* address, int port, char* page, char* userName, char* password,
    char* chatWith, char* message){
    
    sf::Http add(address, port);
    sf::Http::Request request(page, sf::Http::Request::Post);

    std::string body="userName=";
    body=body+(std::string)userName+"&password="+password+"&mobile=true&reference=-1&submit=true&message="+
    (std::string)message+"&chatWith="+(std::string)chatWith;

    request.setBody(body);
    sf::Http::Response response= add.sendRequest(request);
    std::string b=response.getBody();
}