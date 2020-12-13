#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <regex.h>
#define PORT_NUMBER 80
#define URLLENGTH 10000
#define SLASH "/"
#define tagpatten "<(a|A).*(h|H)(R|r)(E|e)(F|f).*=.*(\"|\').+(\"|\').*</(a|A)>"
#define herfpatten "(h|H)(R|r)(E|e)(F|f).*=.*(\"|\')[^(\"|\')]+(\"|\')"
#define quotationmarkpatten "\"[^(\"|\')]+\""
#define STATUSPATTEN "^HTTP/1.1 \\w{3}"
#define STATUSCODEPATTEN " \\w{3}"
#define CONTENTLENGTHPATTEN "Content-Length: \\w*"
#define CONTENTTYPEPATTEN "^Content-Type:.*"
#define LOCATIONPATTEN "^Location:.*"
#define TEXTTYPE "(text|TEXT)"
#define HTMLTYPE "(HTML|html)"
#define LENGTHPATTEN " .*$"

typedef struct node node_t;
struct node{
    char *url;
    node_t* next;
};

typedef struct{
  node_t* head;
  node_t* trail;
}linked_list;


void
creat_a_linked_list(linked_list* queue){
  queue -> head = queue -> trail = NULL;
}

void
insert_at_head(linked_list* queue, char* buffer){
  int i = strlen(buffer) + 1;
  char* data = (char*)malloc(i*(sizeof(char)));
  node_t* current = (node_t*)malloc(sizeof(node_t));
  node_t* tempel = queue -> head;
  current -> next = queue->head;
  current -> url = data;
  strcpy(data,buffer);
  queue -> head = current;
  if(queue -> trail == NULL){
    queue -> trail = current;
  }
}

void
insert_at_the_trail(linked_list* queue, char* buffer){
  int i = strlen(buffer) + 1;
  char* data = (char*)malloc(i*(sizeof(char)));
  node_t* current = (node_t*)malloc(sizeof(node_t));
  current -> next = NULL;
  current -> url = data;
  strcpy(data,buffer);
  if(queue -> head == NULL ){
     queue ->head = queue ->trail = current;
  }else{
    queue-> trail -> next = current;
    queue -> trail = current;
  }
}

void
free_a_linked_list(linked_list* list){
  while(list -> head != NULL){
    node_t* temple = list->head;
    free(list -> head -> url);
    list->head = list->head->next;
    free(temple);
  }
}


int
pop_a_linkedist(linked_list* queue,char* buffer) {
  if (queue->head != NULL) {
    strcpy(buffer,queue->head->url);
    node_t* temp = queue -> head;
    queue->head = queue->head->next;
    free(temp->url);
    free(temp);
    return 1;
  }else{
    return 0;
  }
}

/*regex1 indentifies weather a given regular expression can be find in the buffer,
set the mordify flag to 0: it only tells u did u find the RE
set teh mordify flag to 1: it will modify the buffer to no more than whatever only match the RE*/
int
regex1(char buffer[URLLENGTH], char* patten, int mordify_flag){
  regex_t regex;
  size_t nmatch=100; 
  int i = 0;
  int return_value;
  int cfflag = REG_EXTENDED;
  int ismatched = 0;
  regmatch_t match[100];
  return_value = regcomp(&regex,patten,cfflag);
  return_value = regexec(&regex, buffer,nmatch, match, cfflag);
  int gg = 0;
  return_value = regexec(&regex, buffer,nmatch, match, 0);
  if(return_value == 0){
    ismatched = 1;
    if(mordify_flag){
      for(int z = match[0].rm_so;z<match[0].rm_eo;++z){
        buffer[gg] = buffer[z];
        gg++;
      }
      buffer[gg] = '\0';
    }
  }else if(return_value == REG_NOMATCH){
  //didnt match anything, do nothing
  }else{
    printf("An error occured.\n");
  }
  regfree(&regex);
  return ismatched;
}
  
void
remover(char* astring, char target){
  int i = 0;
  int g;
  for(g = 0; g < strlen(astring); g++){
    if(astring[g] == target){
      continue;
    }else{
      astring[i] = astring[g];
      i++; 
      }
    } 
  astring[i] = '\0';
}

void 
addingtofront(char* astring, char* staticstring){
  char buffer[URLLENGTH];
  strcpy(buffer,astring);
  strcpy(astring,staticstring);
  strcat(astring,buffer);
}

void filler(char* astring, char* hostname,char* previous_page){
  char buffer[URLLENGTH];
  if(regex1(astring,"^//",0)){        //cope with //host/page things
    addingtofront(astring,"http:");   
  }else if(regex1(astring,"^/",0)){   //cope with /page things
    addingtofront(astring,hostname);
    addingtofront(astring,"http://");
  }else if(regex1(astring,"^\\.",0)){ // cope with relative url
    char* tok = astring+2;
    strcpy(buffer,tok);
    addingtofront(buffer,previous_page);
    strcpy(astring,buffer);
  }else if((regex1(astring,"^http://",0))){// the url has been filled to a absolute, full url
    return;
  }else{
    char* tok = strrchr(previous_page,'/'); // fill the abosulute url with a form "a.html"
    tok[0] = '\0';
    strcpy(buffer,previous_page);
    strcat(buffer,"/");
    strcat(buffer,astring);          
    strcpy(astring,buffer);
    return;
  }
}

void deletethelastslash(char* astring){
  int i = strlen(astring) - 1;
  if(astring[i] == '/'){
      astring[i] = '\0';
  }
}

int
parse301(char* input, char* Location){
  char buffer[URLLENGTH];
  strcpy(buffer,input);
  if(regex1(input,LOCATIONPATTEN,0)){
    char* temple = strchr(buffer,' ')+1;
    char* temple2 = strtok(temple,"\r");
    strcpy(Location,temple2);
    return 1;
  }
  return 0;
}

int
creat_a_socket(char* a_valid_url){
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent * server;
  server = gethostbyname(a_valid_url);
  if (server == NULL){
    return -1;
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(PORT_NUMBER);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    perror("ERROR opening socket");
    return -1;
  }
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    exit(0);
  }
  return sockfd;
}

void
Authorization(char* page, char* host,int sockfd){
  char requestheader[URLLENGTH];
  bzero(requestheader,URLLENGTH); 
  char* format = "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nUser-Agent: haoyub\r\nAuthorization: Basic aGFveXViOnBhc3N3b3Jk\r\n\r\n\0";
  int j = snprintf(requestheader, 10000, format,page,host);  
  int status = write (sockfd,requestheader , strlen (requestheader));
  if(status<0){
    exit(0);
  }
}

void
get_a_imfo(char* page, char* host,int sockfd){
  char requestheader[URLLENGTH];
  bzero(requestheader,URLLENGTH); 
  char* format = "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nUser-Agent: Haoyub\r\n\r\n\0"; 
  int j = snprintf(requestheader, 10000, format,page,host);
  int status = write (sockfd,requestheader , strlen (requestheader));
  if(status<0){
    exit(0);
  }
}

void
split_url_into_hostandpage(char* link,char* store_host, char* store_page){
  char host_and_page[URLLENGTH];
  strcpy(host_and_page,link+7);
  char* page = strstr(host_and_page,SLASH);
  if(page == NULL){
    strcpy(store_page,"");
  }else{
    strcpy(store_page,page+1);
  }
  char* host = strtok(host_and_page,SLASH);
  strcpy(store_host,host_and_page);        
}


void
parse_html(char* token,linked_list* queue,char*host ,char* previous_page){
  char buffer[URLLENGTH];
  strcpy(buffer,token);
  if(regex1(buffer,tagpatten, 1)){
    regex1(buffer,herfpatten, 1);
    regex1(buffer,quotationmarkpatten, 1);
    remover(buffer,'\"');
    filler(buffer,host,previous_page);
    remover(buffer,' ');
    insert_at_the_trail(queue,buffer);
    }
}

int
get_status_code(char* buffer){
  int statucode;
  if(regex1(buffer,STATUSPATTEN, 1)){
    regex1(buffer,STATUSCODEPATTEN, 1);
    statucode=atoi(buffer);
    }
  return statucode;
}

int
isvalidtype(char* buffer){
 if(regex1(buffer,CONTENTTYPEPATTEN,0)){
   if(regex1(buffer,TEXTTYPE,0)||regex1(buffer,HTMLTYPE,0)){
      return 1;
    }
    return -1;
 }
 return 0;
}

int
get_expect_length(char* buffer){
  int expected_length = -1;
  if(regex1(buffer,CONTENTLENGTHPATTEN, 1)){
    regex1(buffer,LENGTHPATTEN, 1);
    return atoi(buffer);
  }
  return expected_length;
}

int
read_response_to_buffer(int sockfd,linked_list* queue,char* host, char* previous_page){
  int n = 0;
  int accumulated_length = 0;
  int statucode = -1;
  int expected_length = -1;
  int isMINETYPE = 0;
  char buffer[10000],Location[URLLENGTH];
  int headerl = 0;
  while ( (n = read(sockfd, buffer, 10000))!=0) {
  accumulated_length = accumulated_length + n;
  char* token = strtok(buffer,"\r\n");
  if(token != NULL){
    headerl = accumulated_length +strlen(buffer)- strlen(token);
  }
  /* get the statucode from header*/
  if(statucode == -1){
    statucode = get_status_code(token);
  }
  if(statucode == 401){
    return 401;
  }
      /* if the satucode == 200, we get this page successfully !*/
  if(statucode == 200 ||statucode == 301){
    while(token != NULL){
      if(isvalidtype(token)<0){   //not the html/ test type
        return -100;
      }
      if(parse301(token,Location)){ // redirect to the location
        insert_at_head(queue, Location);
        return 301;
      }           
      parse_html(token,queue,host,previous_page); // pharse 200 statucode url only
      if( expected_length == -1){
        expected_length = get_expect_length(token);
        expected_length = expected_length -headerl;
      }
      token = strtok(NULL,"\r\n");
    }
    if(accumulated_length >= expected_length && accumulated_length!=-1){// return when the content is fully read
      return statucode;
      break;
    }
    }else{
    return statucode; // reaturn -1 when some of the content is missing, will re-request later 
    }
  }
return statucode;
}

void
after_first_componet(char* buffer){
  char temo[URLLENGTH];
  strcpy(temo, buffer);
  char* stok = strstr(temo,".");
  if(stok != NULL){
    strcpy(buffer,stok);
  }else{
    strcpy(buffer,temo);
  }
}

int
webcomparetor(char* current_url, linked_list* visited){
  node_t* temp = visited -> head;
  char buffer[URLLENGTH],check_host[URLLENGTH],check_page[URLLENGTH],check_host_tempo[URLLENGTH];
  char node_host[URLLENGTH],node_page[URLLENGTH];
  strcpy(buffer,current_url);
  split_url_into_hostandpage(buffer,check_host, check_page);
  strcpy(check_host_tempo,check_host);    
  after_first_componet(check_host_tempo);
  while(temp != NULL) {
    strcpy(buffer,temp->url);
    split_url_into_hostandpage(buffer,node_host, node_page);     // host and http is not case sensitive,  
    if(strcasecmp(check_host,node_host) == 0 && strcmp(check_page,node_page) == 0){ // but page is
      return 1;
    }
    after_first_componet(node_host);
    if(strcasecmp(check_host_tempo,node_host) != 0){// u can find this circustance in specification,saying
      return 1;                                     // that we wont fetch two host name have different all but the first elements.
    }
    temp = temp -> next;    
  }
    return 0;
}
