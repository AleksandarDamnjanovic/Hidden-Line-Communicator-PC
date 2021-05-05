#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <glib-2.0/glib.h>
#include <gtk-3.0/gdk/gdk.h>
#include <math.h>
#include <gtk-3.0/gdk/gdkconfig.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <string.h>
#include <webkitgtk-4.0/webkit2/webkit2.h>
#include <pthread.h>
#include <dlfcn.h>

GtkBuilder* builder;

struct page{
    char* top;
    char* bottom;
    char* left_1;
    char* right_1;
    char* left_2;
    char* right_2;
};

struct table{
    char* addresses[10];
    char* userNames[10];
    char* passwords[10];
    int size;
};

struct message{
    char* sender;
    char* message;
    struct message* ptr;
};

struct references{
    char* name;
    int ref;
    struct message* mess;
    struct references* ptr;
};
int ref_size=0;

typedef struct{

    /*MAIN WIDGETS*/
    GtkWidget* window;
    GtkWidget* web;
    GtkWidget* conf_options;
    GtkWidget* edit_connections_menu_button;
    GtkWidget* message_box;
    GtkWidget* message_send;

    /*CONTROLS OF EDIT CONFIGURATIONS DIALOG*/
    GtkWidget* edit_connections;
    GtkWidget* confBox;
    GtkWidget* close_ec_button;
    GtkWidget* web_address_box;
    GtkWidget* user_name_box;
    GtkWidget* password_box;
    GtkWidget* save_connection_button;
    GtkWidget* conf_status_label;
    GtkWidget* rm_connection;
    GtkWidget* save_single_conf;
    GtkWidget* new_conf_button;
    GtkWidget* default_conf_check;

    /*CONTROLS IN CONNECTING DIALOG*/
    GtkWidget* connect;
    GtkWidget* connecting_dialog;
    GtkWidget* connecting_cancel;
    GtkWidget* status_connecting;
    GtkWidget* configuration_connecting;
    GtkWidget* connecting_animation;
    GtkWidget* disconnected_show_dialog;

    /*SELECT CHAT ROOM DIALOG*/
    GtkWidget* select_chat_room_button;
    GtkWidget* cancel_selection_chat_room_button;
    GtkWidget* select_chat_room_dialog;
    GtkWidget* chat_room_list_box;

    /*INFO DIALOGS*/
    GtkWidget* open_tutorial_dialog;
    GtkWidget* open_about_dialog;
    GtkWidget* tutorial_dialog;
    GtkWidget* about_dialog;
    GtkWidget* close_tutorial_dialog_button;
    GtkWidget* close_about_dialog_button;

}widget;

/*FUNCTIONS OF EDIT CONFIGURATION DIALOG*/
void populateConfigurationsBox();
void saveConfs();
void getConfContent(char** value);
int getNumOfConfs();
void getSingleConf(int lineNum, char** li);
void getConfRow(char** addr, char** usr, char** pass, char** lin, int row);
void readConfs();
void insertConf(int index);
void on_default_conf_check_toggled();

/*FUNCTIONS IN CONNECTING DIALOG*/
void on_connecting_cancel_clicked();
void *connectingThread();
void *connectingDialogAnimationThread();
void *disconnectingShowDialogFunction();

/*SELECT CHAT ROOM DIALOG*/
void on_select_chat_room_button_activate();
void on_cancel_selection_chat_room_button_clicked();
void on_select_chat_room_button_clicked();
void *chatThread();
int getRef(char* name);
int setRef(char* name, int new_ref);
struct references* getReference(char* name);
void loadPage();
void loadFile(char** buf, char* address);

int def= 0;
int port= 8080;
bool connectionProcess= FALSE;
bool connected= FALSE;
char* chat_room;
bool running= FALSE;
int message_size=0;

widget *w;
struct table *t;
struct references *ref;
struct page *html;

char* chatWith;

int main(int argc, char** argv){
    gtk_init(&argc, &argv);
    w= g_slice_new(widget);
    struct table tt;
    t=&tt;
    t->size=0;

    loadPage();

    webkit_web_view_get_type();
    webkit_settings_get_type();

    builder= gtk_builder_new_from_file("window.glade");

    /*MAIN WIDGETS DECLARATIONS*/
    w->window= GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_window_set_title(GTK_WINDOW(w->window), "Hidden Line Communicator");
    w->web= GTK_WIDGET(gtk_builder_get_object(builder, "web"));
    w->message_box= GTK_WIDGET(gtk_builder_get_object(builder, "message_box"));
    w->message_send= GTK_WIDGET(gtk_builder_get_object(builder, "message_send"));
    
    /*DECLARATION OF CONTROLS IN EDIT CONFIGURATION DIALOG*/
    w->edit_connections= GTK_WIDGET(gtk_builder_get_object(builder, "edit_connections"));
    w->conf_options= GTK_WIDGET(gtk_builder_get_object(builder, "conf_options"));
    w->confBox= GTK_WIDGET(gtk_builder_get_object(builder, "conf_box"));
    gtk_window_set_title(GTK_WINDOW(w->edit_connections), "Edit connections dialog");
    w->close_ec_button= GTK_WIDGET(gtk_builder_get_object(builder, "close_ec_button"));
    w->edit_connections_menu_button= GTK_WIDGET(gtk_builder_get_object(builder, "Configurations"));
    w->web_address_box= GTK_WIDGET(gtk_builder_get_object(builder, "web_address_box"));
    w->user_name_box= GTK_WIDGET(gtk_builder_get_object(builder, "user_name_box"));
    w->password_box= GTK_WIDGET(gtk_builder_get_object(builder, "password_box"));
    w->save_connection_button= GTK_WIDGET(gtk_builder_get_object(builder, "save_connection_button"));
    w->conf_status_label= GTK_WIDGET(gtk_builder_get_object(builder, "conf_status_label"));
    w->rm_connection= GTK_WIDGET(gtk_builder_get_object(builder, "rm_connection_button"));
    w->save_single_conf= GTK_WIDGET(gtk_builder_get_object(builder, "save_single_conf"));
    w->new_conf_button= GTK_WIDGET(gtk_builder_get_object(builder, "new_conf_button"));
    w->default_conf_check= GTK_WIDGET(gtk_builder_get_object(builder,"default_conf_check"));

    /*DECLARATION OF CONTROLS IN CONNECTION DIALOG*/
    w->connect= GTK_WIDGET(gtk_builder_get_object(builder, "connect_to_server"));
    w->connecting_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "connecting_dialog"));
    w->connecting_cancel= GTK_WIDGET(gtk_builder_get_object(builder, "connecting_cancel"));
    w->status_connecting= GTK_WIDGET(gtk_builder_get_object(builder, "status_connecting"));
    w->configuration_connecting= GTK_WIDGET(gtk_builder_get_object(builder, "configuration_connecting"));
    w->connecting_animation= GTK_WIDGET(gtk_builder_get_object(builder, "connecting_animation"));
    w->disconnected_show_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "disconnected_show_dialog"));

    /*DECLARATIONS OF SELECT CHAT ROOM DIALOG*/
    w->select_chat_room_button= GTK_WIDGET(gtk_builder_get_object(builder, "select_chat_room_button"));
    w->cancel_selection_chat_room_button= GTK_WIDGET(
        gtk_builder_get_object(builder, "cancel_selection_chat_room_button"));
    w->select_chat_room_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "select_chat_room_dialog"));
    w->chat_room_list_box= GTK_WIDGET(gtk_builder_get_object(builder, "chat_room_list_box"));
    w->select_chat_room_button= GTK_WIDGET(gtk_builder_get_object(builder, "select_chat_room_button"));

    /*DECLARATIONS FOR INFO DIALOGS*/
    w->open_tutorial_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "open_tutorial_dialog"));
    w->open_about_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "open_about_dialog"));
    w->tutorial_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "tutorial_dialog"));
    w->about_dialog= GTK_WIDGET(gtk_builder_get_object(builder, "about_dialog"));
    w->close_tutorial_dialog_button= GTK_WIDGET(gtk_builder_get_object(builder, "close_tutorial_dialog_button"));
    w->close_about_dialog_button= GTK_WIDGET(gtk_builder_get_object(builder, "close_about_dialog_button"));

    g_signal_connect(GTK_WINDOW(w->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, w);

    /*FUNCTION CALLS FOR EDIT CONFIGURATION DIALOG*/
    readConfs();
    populateConfigurationsBox();

    webkit_web_view_load_html(WEBKIT_WEB_VIEW(w->web), "", NULL);

    g_object_unref(builder);
    gtk_widget_show_all(w->window);

    gtk_main();
    g_slice_free(widget, w);

    return 0;
}

/*get content of conf file*/
/*value: pointer towards content of conf file*/
void getConfContent(char** value){

    FILE *f;
    f=fopen("confs", "r");

    if(f==NULL)
        return;

    int size=0;

    fseek(f, 0, SEEK_END);
    size= ftell(f);
    fseek(f, 0, SEEK_SET);
    size+=1;

    char buf[size];
    fread(buf, size, sizeof(char), f);
    buf[size-1]='\0';

    char* content=NULL;
    content= realloc(content, strlen(buf) * sizeof(char));
    strcpy(content, buf);

    *value=content;

    fclose(f);
}

/*gets single configuration*/
/*lineNum: number of line to be read from*/
/*li: content of the line*/
void getSingleConf(int lineNum, char** li){
    char* content=NULL;
    getConfContent(&content);

    char* line= strtok(content, "\n");
    for(int i=0; i< lineNum;i++)
        line= strtok(NULL, "\n");

    char* back=NULL;
    back= realloc(back, strlen(line) * sizeof(char));
    strcpy(back, line);
    
    *li=back;
    free(content);
}

/*ads new configuration to the configuration structure max 10 configurations allowed*/
void insertConf(int index){
    const gchar* addr= (const gchar*) gtk_entry_get_text(GTK_ENTRY(w->web_address_box));
    const gchar* usr= (const gchar*) gtk_entry_get_text(GTK_ENTRY(w->user_name_box));
    const gchar* pass= (const gchar*) gtk_entry_get_text(GTK_ENTRY(w->password_box));
    t->addresses[index]=NULL;
    t->addresses[index]= realloc(t->addresses[index], strlen(addr) * sizeof(char));
    strcpy(t->addresses[index], addr);
    t->userNames[index]=NULL;
    t->userNames[index]= realloc(t->userNames[index], strlen(usr) * sizeof(char));
    strcpy(t->userNames[index], usr);
    t->passwords[index]=NULL;
    t->passwords[index]= realloc(t->passwords[index], strlen(pass) * sizeof(char));
    strcpy(t->passwords[index], pass);
}

/*gets parameters of single configuration*/
/*address: server address*/
/*use: user name*/
/*pass: user password*/
/*lin: line of text that is goint to be used for data to be extracted*/
/*row: set default configuration if row contains DEFAULT keyword*/
void getConfRow(char** addr, char** usr, char** pass, char** lin, int row){

    char* word= strtok((char*)*lin, " ");
    char* addr1=NULL;
    addr1= realloc(addr1, strlen(word) * sizeof(char));
    strcpy(addr1, word);
    *addr= addr1;
    word= strtok(NULL, " ");
    char* usr1=NULL;
    usr1= realloc(usr1, strlen(word) * sizeof(char));
    strcpy(usr1, word);
    *usr=usr1;
    word= strtok(NULL, " ");
    char* pass1=NULL;
    pass1= realloc(pass1, strlen(word) * sizeof(char));
    strcpy(pass1, word);
    *pass= pass1;
    word= strtok(NULL, " ");
    if(word!=NULL)
        def=row;
}

/*reads conf file, line by line, and store its data into structure*/
void readConfs(){
    t->size= getNumOfConfs();
    int size=t->size;

    if(t->size==0)
        return;

    char* addresses[size];
    char* userNames[size];
    char* passwords[size];

    char* lines[size];

    for(int i=0;i<size;i++){

        getSingleConf(i, &lines[i]);
        getConfRow(&addresses[i], &userNames[i], &passwords[i], &lines[i], i);
        free(lines[i]);

        t->addresses[i]= addresses[i];
        t->userNames[i]= userNames[i];
        t->passwords[i]= passwords[i];
    }
}

/*populates configuration box with addresses of configurations from the structure*/
/*but cuts its size to max 20 characters*/
void populateConfigurationsBox(){
    for(int i=0;i<t->size;i++){
        const gchar* addr=(const gchar*)t->addresses[i];
        char opt[21];
        memcpy(opt, addr, 20);
        opt[20]='\0';
        gtk_combo_box_text_insert(GTK_COMBO_BOX_TEXT(w->confBox), i, NULL, opt);
    }
}

/*get number of configuration stored in the conf file*/
int getNumOfConfs(){
    int size=0;
    char* content=NULL;
    getConfContent(&content);

    if(content==NULL)
        return 0;

    if(strlen(content)==0)
        return 0;

    char* line= strtok(content, "\n");

    while(line!=NULL){
        size++;
        line= strtok(NULL, "\n");
    }

    free(content);
    return size;
}

/*hanldes process of storin informations from structure into the conf file*/
void on_save_connection_button_clicked(){

    if(t->size==0)
        return;

    int size=0;
    for(int i=0;i<t->size;i++){
        size+= strlen(t->addresses[i]);
        size+= strlen(t->userNames[i]);
        size+= strlen(t->passwords[i]);
        if(i==def)
            size+=10;
        else
            size+=2;

        size++;
    }

    char text[size];
    for(int i=0;i<t->size;i++){

        if(i==0)
            strcpy(text, t->addresses[i]);
        else
            strcat(text, t->addresses[i]);

        strcat(text, " ");
        strcat(text, t->userNames[i]);
        strcat(text, " ");
        strcat(text, t->passwords[i]);

        if(i==def)
            strcat(text, " DEFAULT\n");
        else
            strcat(text, "\n");

    }
    text[size]= '\0';

    FILE *f;
    f= fopen("confs", "w");
    fwrite(text, size, sizeof(char), f);
    fclose(f);
    gtk_label_set_text(GTK_LABEL(w->conf_status_label),"Status: State saved.");
}

/*shows window for creation and editing of configurations and conf file itself*/
void on_Configurations_activate(){
    gtk_widget_show(w->edit_connections);
}

/*clear data from text boxes and close window for configuration editing*/
void on_close_ec_button_clicked(){
    gtk_entry_set_text(GTK_ENTRY(w->web_address_box),"");
    gtk_entry_set_text(GTK_ENTRY(w->password_box),"");
    gtk_entry_set_text(GTK_ENTRY(w->user_name_box),"");
    gtk_widget_hide(w->edit_connections);
}

/*handles of change of selected configuration*/
void on_conf_box_changed(){

    gint id= gtk_combo_box_get_active(GTK_COMBO_BOX(w->confBox));
    if(id>-1){
        gtk_entry_set_text(GTK_ENTRY(w->web_address_box),t->addresses[id]);
        gtk_entry_set_text(GTK_ENTRY(w->password_box),t->userNames[id]);
        gtk_entry_set_text(GTK_ENTRY(w->user_name_box),t->passwords[id]);
    }else{
        gtk_entry_set_text(GTK_ENTRY(w->web_address_box), "");
        gtk_entry_set_text(GTK_ENTRY(w->password_box), "");
        gtk_entry_set_text(GTK_ENTRY(w->user_name_box),""); 
    }

    if(id==def)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->default_conf_check), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->default_conf_check), FALSE);

}

/*removes configuration from the structure*/
void on_rm_connection_button_clicked(){

    gint id= gtk_combo_box_get_active(GTK_COMBO_BOX(w->confBox));
    
    if(id==-1)
        return;

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(w->confBox), None);
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(w->confBox), id);

    free(t->addresses[id]);
    free(t->userNames[id]);
    free(t->passwords[id]);

    t->addresses[id]= NULL;
    t->userNames[id]= NULL;
    t->passwords[id]= NULL;

    if(id!=9)
        for(int i=0;i<t->size;i++){
            if(!t->addresses[i]){
                t->addresses[i]= t->addresses[i+1];
                t->addresses[i+1]= NULL;
                t->userNames[i]= t->userNames[i+1];
                t->userNames[i+1]= NULL;
                t->passwords[i]= t->passwords[i+1];
                t->passwords[i+1]= NULL;
            }
        }

    t->size-=1;
    def-=1;

    on_conf_box_changed();
}

/*saves single configuration into the structure*/
void on_save_single_conf_clicked(){

    gint id= gtk_combo_box_get_active(GTK_COMBO_BOX(w->confBox));

    if(id==-1){

        if(t->size==10){
            gtk_label_set_text(GTK_LABEL(w->conf_status_label),
            (const gchar*)"Status: You can't create more than 10 configurations.");
            return;
        }

        insertConf(t->size);

        gtk_combo_box_text_insert(GTK_COMBO_BOX_TEXT(w->confBox), t->size, NULL, t->addresses[t->size]);
        t->size+=1;
        gtk_combo_box_set_active(GTK_COMBO_BOX(w->confBox), None);
    }else{
        free(t->addresses[id]);
        free(t->userNames[id]);
        free(t->passwords[id]);
        t->addresses[id]=NULL;
        t->userNames[id]=NULL;
        t->passwords[id]=NULL;

        insertConf(id);

        gtk_combo_box_set_active(GTK_COMBO_BOX(w->confBox), None);
        gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(w->confBox),id);
        char opt[21];
        memcpy(opt, t->addresses[id], 20);
        opt[20]='\0';
        gtk_combo_box_text_insert(GTK_COMBO_BOX_TEXT(w->confBox), (gint)id, NULL, opt);
        gtk_combo_box_set_active(GTK_COMBO_BOX(w->confBox), id);
    }

}

/*ads new configuration to the structure*/
void on_new_conf_button_clicked(){

    if(t->size==10){
        gtk_label_set_text(GTK_LABEL(w->conf_status_label),
        (const gchar*)"Status: You can't create more than 10 configurations.");
        return;
    }

    insertConf(t->size);
    
    char opt[21];
    memcpy(opt, t->addresses[t->size], 20);
    opt[20]='\0';
    gtk_combo_box_text_insert(GTK_COMBO_BOX_TEXT(w->confBox), t->size, NULL, opt);
    t->size+=1;
    gtk_combo_box_set_active(GTK_COMBO_BOX(w->confBox), t->size-1);
    on_conf_box_changed();

}

/*handles seting of default configuration*/
void on_default_conf_check_toggled(){
    gboolean deff=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->default_conf_check));
    if(deff){
        def= (int)gtk_combo_box_get_active(GTK_COMBO_BOX(w->confBox));
    }
}

pthread_t thread_connecting_id;
pthread_t thread_connecting_animation_id;
pthread_t thread_disconnected_show_dialog_id;
int dots=1;

/*handles connection process*/
void *connectingThread(){
    gtk_label_set_text(GTK_LABEL(w->status_connecting), "Connecting");

    char confString[strlen("Connecting to: ") + 
    (strlen(t->addresses[def])>24?24:strlen(t->addresses[def]))];

    strcpy(confString, "Connecting to: ");
    char opt[24];
    memcpy(opt, t->addresses[def], 20);
    opt[20]='.';
    opt[21]='.';
    opt[22]='.';
    opt[23]='\0';
    strcat(confString, strlen(t->addresses[def])>24?opt:t->addresses[def]);

    gtk_label_set_text(GTK_LABEL(w->configuration_connecting), confString);

    void *request= dlopen("./communicator.so" ,RTLD_LAZY);
    bool (*func)(char*, int, char*, char*, char*);
    func= dlsym(request, "requestConnection");
    bool result= func(t->addresses[def], port, "/mainHoll", t->userNames[def], t->passwords[def]);
    dlclose(request);

    connected= result;
    connectionProcess= FALSE;

    if(result){
        gtk_menu_item_set_label(GTK_MENU_ITEM(w->connect), "Disconnect");
        gtk_label_set_text(GTK_LABEL(w->status_connecting), "Server accepted your request.");
    }else
        gtk_label_set_text(GTK_LABEL(w->status_connecting), "Server rejected your request.");

    sleep(2);
    
    gtk_widget_hide(w->connecting_dialog);


    return NULL;
}

/*hanldes animations with dots on connecting dialog*/
void *connectingDialogAnimationThread(){

    while(connectionProcess){

        char d[dots];
        strcpy(d, ".");
        for(int i=1;i< dots;i++)
            strcat(d, ".");

        dots+=1;

        if(dots==10)
            dots=1;

        sleep(1);

        gtk_label_set_text(GTK_LABEL(w->connecting_animation), d);
    }

    return NULL;
}

/*shows dialog after disconnecting*/
void *disconnectingShowDialogFunction(){
    gtk_menu_item_set_label(GTK_MENU_ITEM(w->connect), "Connect");
    sleep(3);
    gtk_widget_hide(w->disconnected_show_dialog);
    running= FALSE;
    connected= FALSE;

    return NULL;
}

/*hanldes connecting and disconnecting from the server*/
void on_connect_to_server_activate(){
    if(!connected){
        gtk_widget_show(w->connecting_dialog);
        pthread_create(&thread_connecting_id, NULL, connectingThread, NULL);
        pthread_create(&thread_connecting_animation_id, NULL, connectingDialogAnimationThread, NULL);
        connectionProcess= TRUE; 
    }else{
        gtk_widget_show(w->disconnected_show_dialog);
        pthread_create(&thread_disconnected_show_dialog_id, NULL, disconnectingShowDialogFunction, NULL);
        connected= FALSE;
    }
}

/*hanldes breaking of the connection process*/
void on_connecting_cancel_clicked(){
    if(connectionProcess){
        connectionProcess= FALSE;
        pthread_join(thread_connecting_animation_id, NULL);
        pthread_join(thread_connecting_id, NULL);
        gtk_widget_hide(w->connecting_dialog);
    }
}

pthread_t chat_thread;
/*handles contact selection from dialog with contact list combo box*/
void on_select_chat_room_button_activate(){
    if(connected){
        void *request= dlopen("./communicator.so", RTLD_LAZY);
        int (*func)(char*, int, char*, char*, char*);
        func= dlsym(request, "getNumOfClients");
        int size= func(t->addresses[def], port, "/UserPage", t->userNames[def], t->passwords[def]);
        char* clients[size];
        void (*client)(char*, int, char*, char*, char*, int, char*[]);
        client= dlsym(request, "getClientList");
        client(t->addresses[def], port, "/UserPage", t->userNames[def], t->passwords[def], size, clients);

        gtk_combo_box_set_active(GTK_COMBO_BOX(w->chat_room_list_box), None);
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(w->chat_room_list_box));

        /*creates array of references that is going to store new list of contacts*/
        struct refereces* r[size];

        /*populates temporeraly array of contacts and populate combobox*/
        for(int i=0;i<size;i++){
            struct references *temp= malloc(sizeof(*ref));
            temp->name= malloc(strlen(clients[i]) * sizeof(char));
            strcpy(temp->name, clients[i]);
            temp->ref=0;
            temp->mess= NULL;
            r[i]=malloc(sizeof(temp));
            r[i]= temp;
                
            gtk_combo_box_text_insert(
                GTK_COMBO_BOX_TEXT(w->chat_room_list_box), i, NULL, clients[i]);
            free(clients[i]);
        }

        /*adds reference towards next element in array*/
        for(int i=0;i<size;i++){
            struct references* pp=NULL;
            pp= r[i];
            if(i<size-1)
                pp->ptr= r[i+1];
            else
                pp->ptr= NULL;
        }

        /*compare names of new temporeraly array with already existing one
        if found the same name value it copies reference number to temporeraly
        array element and pointer towards the first element of message list
        If new list doesn't contains new client, all of messages from that client
        are going to be deleted after label end of this loop*/
        for(int i=0;i<size;i++){
            struct references* p1=r[i];
            struct references* p2= ref;
            bool oldList=ref!=NULL?TRUE:FALSE;
            struct message* p3;
            
            if(oldList)
                p3= p2->mess;
            else
                p3=NULL;

            bool existing= FALSE;

            for(int o=0;o<ref_size;o++){
                if(p1!=NULL && p2!=NULL)
                    if(strcmp(p1->name, p2->name)==0){
                        p1->ref=p2->ref;
                        p1->mess= p2->mess;
                        existing= TRUE;
                        goto end;
                    }
                p2= p2->ptr;
                p3= p2->mess;
            }

            end:
            if(!existing)
                while(p3!=NULL){
                    struct message* p4= p3;
                    p3= p3->ptr;
                    free(p4);
                }
            
        }

        /*frees all of elements in old array of contacts*/
        struct references* p1= ref;
        struct references* p2= ref;
        for(int i=0;i<ref_size;i++){
            p2= p1->ptr;
            if(p1->ptr!=NULL)
                free(p1);

            p1=p2;
        }

        /*finally public list of references are going to point towards temporeraly one*/
        ref_size= size;
        ref= r[0];

        dlclose(request);
        gtk_widget_show(w->select_chat_room_dialog);
    }
}

/*closes dialog for selecting contact*/
void on_cancel_selection_chat_room_button_clicked(){
    gtk_widget_hide(w->select_chat_room_dialog);
}

/*In this function chatWith and reference number is selected based on selected index from
combo box with list of contacts*/
void on_select_chat_room_button_clicked(){
    
    int index= gtk_combo_box_get_active(GTK_COMBO_BOX(w->chat_room_list_box));
    struct references* p2= ref;

    for(int i=0;i<index; i++){
        p2= p2->ptr;
    }

    chatWith= realloc(chatWith, strlen(p2->name) * sizeof(char));
    strcpy(chatWith, p2->name);
    gtk_widget_hide(w->select_chat_room_dialog);
    running= TRUE;
    pthread_create(&chat_thread, NULL, chatThread, NULL);
    message_size=0;
}

/*thread that hanldes communication*/
void *chatThread(){

    /*Here it references two functions for message retreaval
    one for getting number of messages and second to get actual messages*/
    void *request= dlopen("./communicator.so" ,RTLD_LAZY);
    int (*num)(char*, int, char*, char*, char*, char*, int);
    void (*func)(char*, int, char*, char*, char*, char*, int, char*[], char*[]);
    num= dlsym(request, "getNumOfMessages");
    func= dlsym(request, "getMessages");

    /*in case that contact doesn't exists creates new list of messages connected to the reference
    in the newly created contact. In other case, ads new messages to the already created array*/
    while(running){
        sleep(1.5);

        struct references* chat= getReference(chatWith);
        int ref_num= getRef(chatWith);
        int res= num(t->addresses[def], port, "/chatRoom", t->userNames[def], t->passwords[def], chatWith, ref_num);

        if(res!=0){

            char* senders[res];
            char* messages[res];

            if(res>0)
                func(t->addresses[def], port, "/chatRoom", t->userNames[def], t->passwords[def], 
                chatWith, ref_num, senders, messages);

            setRef(chatWith, res+ ref_num);

            /*All of retreaved messages are going to form link list from relevant struct refereces*/

            struct message* p1= NULL;
            if(chat->mess==NULL){
                chat->mess= (struct message*)calloc(1 ,sizeof(struct message*));
                p1= chat->mess;
            }else{
                p1= chat->mess;
                while(p1->ptr!=NULL)
                    p1= p1->ptr;

                p1->ptr= (struct message*)calloc(1, sizeof(struct message*));
                p1=p1->ptr;
            }

            for(int i= 0; i < res; i++){
                p1->sender= (char*)calloc(strlen(senders[i]), sizeof(char*));
                strcpy(p1->sender, senders[i]);
                p1->message= (char*)calloc(strlen(messages[i]), sizeof(char*));
                strcpy(p1->message, messages[i]);

                if(i<res-1){
                    p1->ptr= NULL;
                    p1->ptr= (struct message*)calloc(1, sizeof(struct message*));
                    p1= p1->ptr;
                }else
                    p1->ptr= NULL;
            }

            for(int i= 0; i < res; i++){
                free(senders[i]);
                free(messages[i]);
            }

        }
        
        int mess_num=0;
        struct message* po= chat->mess;
        while(po!=NULL){
            mess_num++;
            po=po->ptr;
        }

        char* mess1[mess_num];
        int mess_count= mess_num;
        mess_num=0;
        int m_size=0;

        struct message* pp= chat->mess;
        while(pp!=NULL){
            
            bool me= FALSE;
            if(strcmp(pp->sender, chatWith)==0)
                me= TRUE;
            
            int size_div=0;
            if(me)
                size_div= strlen(html->left_1)+strlen(pp->sender)+2+strlen(pp->message)+strlen(html->left_2);
            else
                size_div= strlen(html->right_1)+strlen(pp->sender)+2+strlen(pp->message)+strlen(html->right_2);

            mess1[mess_num]= (char*)calloc(size_div, sizeof(char*));

            if(me){
                strcpy(mess1[mess_num], html->left_1);
                strcat(mess1[mess_num], pp->sender);
                strcat(mess1[mess_num], ": ");
                strcat(mess1[mess_num], pp->message);
                strcat(mess1[mess_num], html->left_2);
            }else{
                strcpy(mess1[mess_num], html->right_1);
                strcat(mess1[mess_num], pp->sender);
                strcat(mess1[mess_num], ": ");
                strcat(mess1[mess_num], pp->message);
                strcat(mess1[mess_num], html->right_2);
            }

            m_size+= size_div;
            mess_num++;
            pp= pp->ptr;
        }

        m_size+= strlen(html->top)+strlen(html->bottom);

        char fullPage[m_size*4+1];
        strcpy(fullPage, html->top);

        for(int i= 0 ; i< mess_count; i++){
            strcat(fullPage, mess1[i]);
            free(mess1[i]);
            mess1[i]= NULL;
        }

        strcat(fullPage, html->bottom);
        fullPage[m_size]='\0';

        if(m_size>message_size){
            message_size= m_size;
            webkit_web_view_load_html(WEBKIT_WEB_VIEW(w->web), fullPage, NULL);
        }
        
    }

    dlclose(request);
    return NULL;
}

/*gets pointer to the structure specified by name*/
/*paramether 1. name: contact name of the reference*/
struct references* getReference(char* name){
    struct references* p1= ref;
    struct references* p2= NULL;
    struct references* ret= NULL;

    for(int i= 0; i < ref_size; i++){
        p2=p1->ptr;
        if(strcmp(name, p1->name)==0){
            ret= p1;
            goto end;
        }
        p1=p2;
    }

    end:
    return ret;
}

/*gets reference number from specified struct reference*/
/*paramether 1. name: contact name of the reference to get number of reference*/
int getRef(char* name){
    struct references* p1=ref;
    struct references* p2= NULL;
    int ret=0;

    for(int i= 0; i < ref_size; i++){
        p2=p1->ptr;
        if(strcmp(name, p1->name)==0){
            ret= p1->ref;
            goto end;
        }
        p1=p2;
    }

    end:
    return ret;
}

/*set new reference number to struct with specified name*/
/*paramether 1. name: contact name of the reference*/
/*paramether 2. new_ref: new reference number to be set to*/
int setRef(char* name, int new_ref){

    struct references* p1=ref;
    struct references* p2= NULL;

    for(int i= 0; i < ref_size; i++){
        p2=p1->ptr;
        if(strcmp(name, p1->name)==0){
            p1->ref= new_ref;
            goto end;
        }
        p1=p2;
    }

    end:
    return 0;
}

/*loads all parts of html at the moment of application start*/
void loadPage(){
    html= realloc(html, sizeof(html));
    loadFile(&html->top, "page/top.txt");
    loadFile(&html->bottom, "page/bottom.txt");
    loadFile(&html->left_1, "page/left-1.txt");
    loadFile(&html->left_2, "page/left-2.txt");
    loadFile(&html->right_1, "page/right-1.txt");
    loadFile(&html->right_2, "page/right-2.txt");
}

/*load specific file from address*/
/*paramether 1. buf: buffer to store all of data from the selected file*/
/*paramether 2. address: address of the file to be read from*/
void loadFile(char** buf, char* address){
    FILE *f;
    int size=0;
    f= fopen(address, "r");
    fseek(f,0, SEEK_END);
    size = ftell(f);
    size+=1;
    fseek(f, 0, SEEK_SET);
    char* temp= NULL;
    char t[size];
    temp= realloc(temp, sizeof(char) * size);
    fread(t, size-1, sizeof(char), f);
    t[size-1]='\0';
    strcpy(temp, t);
    *buf= temp;
    fclose(f);
}

/*hanldes message send event*/
void on_message_send_clicked(){
    
    if(!connected && !running)
        return;

    gchar* text= gtk_entry_get_text(GTK_ENTRY(w->message_box));

    void* request= dlopen("./communicator.so", RTLD_LAZY);
    /*address, port, page, userName, password, chatWith, message*/
    void (*send)(char*, int, char*, char*, char*, char*, char*);
    send= dlsym(request, "sendMessages");
    send(t->addresses[def], port, "/chatRoom", t->userNames[def], t->passwords[def], chatWith, text);

    dlclose(request);
    gtk_entry_set_text(GTK_ENTRY(w->message_box), (const gchar*)"");
}

/*opens tutorial dialog*/
void on_open_tutorial_dialog_activate(){
    gtk_widget_show(w->tutorial_dialog);
}

/*closes tutorial dialog*/
void on_close_tutorial_dialog_button_clicked(){
    gtk_widget_hide(w->tutorial_dialog);
}

/*open about dialog*/
void on_open_about_dialog_activate(){
    gtk_widget_show(w->about_dialog);
}

/*closes about dialog*/
void on_close_about_dialog_button_clicked(){
    gtk_widget_hide(w->about_dialog);
}