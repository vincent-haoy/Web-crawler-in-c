#include<ass1make.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

int
main(int argc, char ** argv){
  int sockfd;
  int visited_count = 3;
  char buffer[10000];
  int num_visited = 0;
  char host[URLLENGTH],page[URLLENGTH];
  
  linked_list queue;
  linked_list visited;
  creat_a_linked_list(&queue);
  creat_a_linked_list(&visited);
  //the initial url
  insert_at_the_trail(&queue,argv[1]);
  while(pop_a_linkedist(&queue,buffer) && num_visited <100){
    // if the current page is find in visited queue, just ignore it.
    if(webcomparetor(buffer,&visited)){
      continue;
    }
    num_visited ++;
    char backupbuffer[URLLENGTH];
    strcpy(backupbuffer,buffer);
    
    split_url_into_hostandpage(buffer,host,page);

    sockfd = creat_a_socket(host);
    if(sockfd < 0){
      continue;
    }
    get_a_imfo(page, host, sockfd);
    int statucode = -1;
    /* read the reponse from buffer, and pharse all the url*/
    statucode = read_response_to_buffer(sockfd,&queue,host,buffer);
    /*insert the visted page to the queue*/
    if(statucode != 503){
      insert_at_head(&visited,backupbuffer); 
    }
    if(statucode == 401){
      /*the 401 represents the web wants client to sent back a authorization pass, in this circumstance
      I just add my encoded password to a normal header, which will be sent later on*/
      Authorization(page,host,sockfd);
      strcpy(buffer,backupbuffer);
      statucode = read_response_to_buffer(sockfd,&queue,host,buffer);
    }
    if(statucode == 414 ||statucode == 404 || statucode == 410){
      /*do nothing, since we are unable to can.
      those 3 statucode represent permanent failtures
      404: page not found
      414:  the URL requested by the client is longer than the server can provide
      410:the resource requested by the client has been permanently deleted
      */
      printf("%s\n",backupbuffer);
    }
    if((statucode == 503 || statucode == 504) && visited_count >0){
      /* 503 indicates the web is temporarily unaccesseble, I will insert it at the head
      and assume we didnt fetch it, so that my programe cant retry automatically for three times;*/
      insert_at_head(&queue,backupbuffer);
      visited_count --;
    }else if(statucode == 200){
      visited_count = 3;//resert the 503 vistited time.
      printf("%s\n",backupbuffer);
    }      
    close(sockfd);
  }   
  free_a_linked_list(&queue);
  free_a_linked_list(&visited);
  
  return 0;
}
