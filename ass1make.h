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
#define URLLENGTH 10000
#define SLASH "/"
#define PORT_NUMBER 80

typedef struct node node_t;
struct node{
    char *url;
    node_t* next;
};
typedef struct{
  node_t* head;
  node_t* trail;
}linked_list;

int parse301(char* input, char* Location);
void Authorization(char* page, char* host, int sockfd);
int isvalidtype(char* buffer);

void insert_at_head(linked_list* queue, char* buffer);
void creat_a_linked_list(linked_list* queue);
void insert_at_the_trail(linked_list* queue, char* buffer);
void free_a_linked_list(linked_list* list);
int pop_a_linkedist(linked_list* queue,char* buffer);
int creat_a_socket(char* a_valid_url);
void get_a_imfo(char* page, char* host,int sockfd);
void split_url_into_hostandpage(char* link, char* store_host, char* store_page);
int regex1(char buffer[URLLENGTH],char* patten, int modify_flag);
void remover(char* astring, char target);
void filler(char* astring, char* hostname,char* previous_page);
void addingtofront(char* astring, char* staticstring);
void deletethelastslash(char* astring);
int read_response_to_buffer(int sockfd,linked_list* queue,char* host, char* previous_page);
void parse_html(char* token,linked_list* queue,char* host, char* previous_page);
int get_status_code(char* buffer);
int get_expect_length(char* buffer); 
void after_first_componet(char* buffer);
int webcomparetor(char* current_url, linked_list* visited);
