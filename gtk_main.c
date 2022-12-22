#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "http/http-api.h"
#include <json-c/json.h>


char* conc_str_int(const char *str, int num, char *buff)
{
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%s%d", str, num);
	return buff;
}

char* concat(const char *s1, const char *s2)
{
    char result[64]; // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

const char* map_num_to_month[12] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August", "September", 
	"October", "November", "December"
};

GtkWindow *main_window = NULL;
GtkWindow *book_main_window = NULL;
GtkWindow *user_login_window = NULL;
GtkWindow *user_signup_window = NULL;
GtkWindow *user_orders_window = NULL;
GtkWindow *admin_login_window = NULL;
GtkWindow *admin_table_window = NULL;

GtkWindow *open_window = NULL;

GtkWidget *book_button_home;
GtkWidget *box_header;
GtkWidget *button_user_page;
GtkWidget *button_logout;
GtkWidget *label_login_error;
GtkWidget *label_signup_error;
GtkWidget *signup_grid;
GtkWidget *login_grid;
GtkWidget *calendar_from;
GtkWidget *calendar_to;
GtkWidget *label_date_from_search;
GtkWidget *label_date_to_search;
GtkWidget *total_cost;
GtkWidget *submit_booking_button;

GtkWidget *admin_table_error
GtkWidget *admin_user_error

GtkBuilder *login_builder;
GtkBuilder *signup_builder;
GtkBuilder *admin_table_builder;

GtkBuilder *open_builder = NULL;

GtkWidget *booking1, *booking2;
GtkWidget *transaction1, *transaction2;
GtkWidget *status1, *status2;

struct order_rooms_frame{
	GtkWidget *room_type;
	GtkWidget *label_date;
};
struct order_rooms_frame orders_user[4];


//////////////////

struct room_cost_data{
	double room_total_cost;
	GtkWidget *parent;
};
struct room_cost_data rooms_cost_data[2];

struct booking_rooms_frame{
	GtkWidget *box_parent;
	GtkWidget *label_date_from;
	GtkWidget *label_date_to;
	GtkWidget *room_cost;
	GtkWidget *nights;
	GtkWidget *taxes;
	GtkWidget *remove_button;
};
struct booking_rooms_frame booking_basket[2];

struct room_listbox_data{
	char room_type[8];
	char description[2][30];
	double price;
};
struct room_listbox_data rooms_data[8];

struct room_listbox{
	GtkWidget *grid_parent;
	GtkWidget *image;
	GtkWidget *book_button;
	GtkWidget *room_type;
	GtkWidget *description;
	GtkWidget *price;
};
struct room_listbox rooms_display[8];

struct date{
	int year;
	int month;
	int day;
};
struct date cal_date_from = {0, 0, 0}, cal_date_to = {0, 0 , 0}, 
aux_cal_date_from = {0, 0, 0}, aux_cal_date_to = {0, 0 , 0};

const int rooms_count_response;
int rooms_count_track; 
double room_cost_total;
double taxes = 37.5;

///////////////////////

const char *admin_name = "admin";
const char *admin_email = "admin@admin";
const char *admin_password = "admin";

///////////////////////

int user_is_authenticated = 0;
char user_name[64];
char user_email[64];
char user_password[64];
char user_surname[64];
char user_country[32];
char user_phone[16];
char user_city[32];
char user_address[64];
char user_zipcode[32];

///////////////////////

struct http serv;

///////////////////////

int getDays(int year, int month, int day);
int isLeap(int num);
int daysBetweenDates(struct date date1, struct date date2);

GtkBuilder*
new_builder_from_file(const char *filename);

G_MODULE_EXPORT void
auth_user_and_redirect(GtkWidget* widget, gpointer data);

void
main_window_auth_user(void);

void
book_room_func(void);

void
login_user_func(void);

void
user_orders_func(void);

G_MODULE_EXPORT void
user_check_redirect(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
logout_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
signup_page_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
auth_login_page_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
auth_signup_page_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
get_calendar_from_date(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
get_calendar_to_date(GtkWidget* widget, gpointer data);

void set_date_labels_search(void);

G_MODULE_EXPORT void
search_rooms_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
add_room_to_booking(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
remove_room(GtkWidget* widget, gpointer data);

void
admin_profile_func(void);

void
open_admin_table_window_func(void);

G_MODULE_EXPORT void
admin_record_add(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
admin_button_modify(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
admin_button_delete(GtkWidget* widget, gpointer data);

///////////////////////

G_MODULE_EXPORT void
roomtype_get_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
roombooked_get_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
room_get_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
transaction_get_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
booking_get_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
user_get_func(GtkWidget* widget, gpointer data);

///////////////////////


int main(int argc, char** argv)
{
	http_init(&serv, argv[1], atoi(argv[2]));
	
	GtkBuilder *builder;
	
	gtk_init(&argc, &argv);

	if((builder = new_builder_from_file("home_glade.glade")) == NULL)
		return 1;
		
	fprintf(stderr, "FAT COCK");
	main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	open_window = main_window;
  	
  	/*Hide button and show it again if user is logged in (=authenticated)*/
  	button_user_page = GTK_WIDGET(gtk_builder_get_object(builder, "user_page_button"));
  	button_logout = GTK_WIDGET(gtk_builder_get_object(builder, "logout_button"));

  	main_window_auth_user();
  	
  	gtk_builder_connect_signals(builder, NULL);
	
	g_object_unref(builder);

    gtk_widget_show(main_window);
    
    open_window = main_window;
	

	gtk_main ();

	return 0;

}


GtkBuilder *
new_builder_from_file(const char *filename)
{
	GError *error = NULL;
	GtkBuilder *builder = gtk_builder_new();
  	if(gtk_builder_add_from_file(builder, filename, &error) == 0)
    {
      g_printerr("Error loading file: %s\n", error->message);
      g_clear_error(&error);
      return NULL;
    }
    return builder;
}

G_MODULE_EXPORT void
auth_user_and_redirect(GtkWidget* widget, gpointer data)
{	
	if(user_is_authenticated)
		book_room_func();
	else
		login_user_func();
}

void
main_window_auth_user(void)
{
	if(user_is_authenticated)
  	{
  		gtk_button_set_label(button_user_page, "Profile");
  		gtk_widget_show(button_logout);
  	}
  	else
  	{
  		gtk_button_set_label(button_user_page, "Log in");
	  	gtk_widget_hide(button_logout);
  	}
  	
	// smells fishy. Check later!!!
  	if(open_builder != NULL){
		g_object_unref(open_builder);  	
  	}
  	
  	gtk_widget_hide(open_window);
  	gtk_widget_show(main_window);
}

void
book_room_func(void)
{
	GtkBuilder *builder = new_builder_from_file("book_window.glade");

	book_main_window = GTK_WIDGET(gtk_builder_get_object(builder, "book_main_window"));
	
	calendar_from = GTK_WIDGET(gtk_builder_get_object(builder, "calendar_from"));
	calendar_to = GTK_WIDGET(gtk_builder_get_object(builder, "calendar_to"));
	
	label_date_from_search = GTK_WIDGET(gtk_builder_get_object(builder, "label_date_from_search"));
	label_date_to_search = GTK_WIDGET(gtk_builder_get_object(builder, "label_date_to_search"));
	
	for(int i = 1; i <= 8; ++i)
	{
		char buf[32];
		rooms_display[i-1].grid_parent = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_grid", i, buf)));
		rooms_display[i-1].image = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("image_room", i, buf)));
		rooms_display[i-1].book_button = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("add_room_button", i, buf)));
		rooms_display[i-1].room_type = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_type_label", i, buf)));
		rooms_display[i-1].description = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("short_description_label", i, buf)));
		rooms_display[i-1].price = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("price_label", i, buf)));
		//printf("PRICE for ROOM %s: %f", i-1, gtk_label_get_text(rooms_display[i-1]->price));
		gtk_widget_hide(rooms_display[i-1].grid_parent);
	}
	
	for(int i = 1; i <= 2; ++i)
	{
		char buf[32];
		booking_basket[i-1].box_parent = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_basket_box", i, buf)));
		booking_basket[i-1].label_date_from = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("label_date_from_booking", i, buf)));
		booking_basket[i-1].label_date_to = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("label_date_to_booking", i, buf)));
		booking_basket[i-1].room_cost = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_cost", i, buf)));
		booking_basket[i-1].nights = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("nights_count", i, buf)));
		booking_basket[i-1].taxes = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("taxes", i, buf)));
		booking_basket[i-1].remove_button = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("remove_room", i, buf)));
		gtk_widget_hide(booking_basket[i-1].box_parent);
	}
	
	total_cost = GTK_WIDGET(gtk_builder_get_object(builder, "total_cost"));
	
	submit_booking_button = GTK_WIDGET(gtk_builder_get_object(builder, "submit_booking_button"));
	gtk_widget_hide(submit_booking_button);
	
	////////////////////// HOW TO SET DATE AT THE BEGINNING??? ///////////
	get_calendar_from_date(calendar_from, &cal_date_from);
	get_calendar_to_date(calendar_to, &cal_date_to);
	/////////////////////
	
	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(builder);
	gtk_widget_hide(open_window);
	gtk_widget_show(book_main_window);
	open_window = book_main_window;
}

void
set_date_from_label_search(void)
{
	char date_from_buff[16];
	sprintf(date_from_buff, "%02d.%02d.%d", cal_date_from.day, cal_date_from.month + 1, 																	cal_date_from.year);
	gtk_label_set_text(label_date_from_search, date_from_buff);
}

void
set_date_to_label_search(void)
{
	char date_to_buff[16];
	sprintf(date_to_buff, "%02d.%02d.%d", cal_date_to.day, cal_date_to.month + 1, 																	cal_date_to.year);
	gtk_label_set_text(label_date_to_search, date_to_buff);
	
}

G_MODULE_EXPORT void
get_calendar_from_date(GtkWidget* widget, gpointer data)
{
	gtk_calendar_get_date(calendar_from, &cal_date_from.year, &cal_date_from.month, 																&cal_date_from.day);
	set_date_from_label_search();
}

G_MODULE_EXPORT void
get_calendar_to_date(GtkWidget* widget, gpointer data)
{
	gtk_calendar_get_date(calendar_to, &cal_date_to.year, &cal_date_to.month, 																	&cal_date_to.day);
	set_date_to_label_search();
}

G_MODULE_EXPORT void
user_check_redirect(GtkWidget* widget, gpointer data)
{
	if(user_is_authenticated)
		user_orders_func();
	else
		login_user_func();
}

void
user_orders_func(void)
{
	GtkBuilder *builder = new_builder_from_file("user_orders_window.glade");
	
	user_orders_window = GTK_WIDGET(gtk_builder_get_object(builder, "user_page_window"));
	
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "name_entry")), user_name);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "surname_entry")), user_surname);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "email_entry")), user_email);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "password_entry")), user_password);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "phone_entry")), user_phone);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "country_entry")), user_country);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "city_entry")), user_city);
	gtk_entry_set_text(GTK_WIDGET(gtk_builder_get_object(builder, "zipcode_entry")), user_zipcode);
	
	booking1 = GTK_WIDGET(gtk_builder_get_object(builder, "booking1"));
	booking2 = GTK_WIDGET(gtk_builder_get_object(builder, "booking2"));
	transaction1 = GTK_WIDGET(gtk_builder_get_object(builder, "total1"));
	transaction2 = GTK_WIDGET(gtk_builder_get_object(builder, "total2"));
	status1 = GTK_WIDGET(gtk_builder_get_object(builder, "status1"));
	status2 = GTK_WIDGET(gtk_builder_get_object(builder, "status2"));		
	
	
	
	for(int i = 1; i <= 4; ++i)
	{
		char buf[64];
		orders_user[i-1].room_type = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_type", i, buf)));
		orders_user[i-1].label_date = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("date", i, buf)));
	}
	
	///////////////////
	///GET bookings of user
	///////////////////
	
	if(1)
	{
		
	}
	else
	{
		
	}

	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(user_orders_window);
	open_window = user_orders_window;
	g_object_unref(builder);
}

G_MODULE_EXPORT void
logout_func(GtkWidget* widget, gpointer data)
{
	user_is_authenticated = 0;
	main_window_auth_user();
}

void
admin_profile_func(void)
{
	GtkBuilder *builder = new_builder_from_file("admin_main_window.glade");
	admin_login_window = GTK_WIDGET(gtk_builder_get_object(builder, "admin_main_window"));
	
	GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_roomtype"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "roomType");
	
	button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_roombooked"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "room_booked");
	
	button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_room"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "room");
	
	button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_transaction"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "transaction");
	
	button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_booking"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "booking");
	
	button = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "button_user"));
	g_signal_connect(button, "clicked", G_CALLBACK(admin_table_modify), "user");
	
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(admin_login_window);
	open_window = admin_login_window;
	
	g_object_unref(builder);
}

void
open_admin_table_window_func(void)
{
	admin_table_builder = new_builder_from_file("admin_table1_window.glade");
	
	admin_table_window = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "admin_table_main_window"));
	
	
	
	gtk_builder_connect_signals(admin_table_builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(admin_table_window);
	open_window = admin_table_window;
	
	g_object_unref(admin_table_builder);
}

void
admin_button_modify();

void
admin_button_delete();

G_MODULE_EXPORT void
admin_table_modify(GtkWidget* widget, gpointer data)
{
	char table_buf[64];
	strncpy(table_buf, data, 63);
	open_admin_table_window_func();
	
	GtkWidget *box_table = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "box_table"));
	admin_table_error = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "admin_table_error"));
	gtk_label_set_text(admin_table_error, "");
	
	char *body_prepare = "";
	char post_body[1024];
	char *response_body = NULL;
    	int response_body_size = 0;
    	char req_addr[64];
	strcpy(req_addr, concat("/api/", table_buf));
	
	printf("\nREQUEST PATH: |%s|\n", req_addr);
	
    	int ret_code = http_get(&serv, req_addr, post_body, sizeof(post_body), &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{	
		json_object *jobj = json_tokener_parse(response_body);
		
		int objects_count_json = json_object_object_length(jobj);
		
		json_object *jobj_child;
		for(int i = 0; i < objects_count_json; ++i)
		{
			GtkWidget *grid_ = gtk_grid_new();
			GtkWidget *label_num = gtk_label_new();
			GtkWidget *label_id = gtk_label_new();
			GtkWidget *button_modify = gtk_button_new();
			GtkWidget *button_delete = gtk_button_new();
			
			jobj_child = json_object_array_get_idx(jobj, i);
			
			char index_buf[4];
			sprintf(index_buf, "%d", i);
			gtk_label_set_text(label_num, id_buf);
			gtk_label_set_text(label_id, json_object_get_string(json_object_object_get(jobj_child, "id")));
			gtk_grid_attach(grid, label_num, 0, 0, 1, 1);
			gtk_grid_attach(grid, label_id, 1, 0, 1, 1);
			gtk_grid_attach(grid, button_modify, 2, 0, 1, 1);
			gtk_grid_attach(grid, button_delete, 3, 0, 1, 1);
			gtk_grid_set_row_homogeneous(grid, TRUE);
			gtk_grid_set_column_homogeneous(grid, FALSE);
			gtk_box_pack_end(box_table, grid, FALSE, TRUE, 0);
			
			char update_buf[64];

			char delete_buf[64];
			char delete_id_buf[64];
			
			strcpy(update_buf, concat(req_addr, json_object_get_string(json_object_object_get(jobj_child, "/id")));
			
			strcpy(delete_buf, concat(req_addr, "/delete/"));
			strcpy(delete_id_buf, concat(delete_buf, json_object_get_string(json_object_object_get(jobj_child, "id")));
	
			g_signal_connect(button_modify, "clicked", G_CALLBACK(admin_button_modify), update_buf);
			g_signal_connect(button_delete, "clicked", G_CALLBACK(admin_button_delete), delete_id_buf);		
		}
		
	}
	else
	{
		gtk_label_set_text(admin_table_error, "Table records fetching error!");
	}
	
}

G_MODULE_EXPORT void
admin_record_add(GtkWidget* widget, gpointer data)
{

}

G_MODULE_EXPORT void
admin_button_modify(GtkWidget* widget, gpointer data)
{
	GtkBuilder *admin_user_builder = new_builder_from_file("admin_user_window.glade");
	GtkWidget *box_general = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "box_general"));
	admin_user_error = GTK_WIDGET(gtk_builder_get_object(admin_table_builder, "admin_user_error"));

	char *body_prepare = ""	
	char *response_body = NULL;
    	int response_body_size = 0;
	
    	int ret_code = http_get(&serv, data, get_body, sizeof(get_body), &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{
		json_object * jobj = json_tokener_parse(response_body);
		enum json_type type;
		
		json_object_object_foreach(jobj, key, val)
		{
			GtkWidget *grid = gtk_grid_new();
			GtkWidget *entry = gtk_entry_new();
			GtkWidget *label = gtk_label_new();
			gtk_grid_attach(grid, label_num, 0, 0, 1, 1);
			gtk_grid_attach(grid, label_id, 1, 0, 1, 1);
			gtk_grid_set_row_homogeneous(grid, FALSE);
			gtk_grid_set_column_homogeneous(grid, TRUE);
			gtk_grid_set_column_spacing(grid, 50);
			gtk_label_set_text(label, key);
			gtk_entry_set_text(entry, json_object_get_string(json_object_object_get(jobj_child, key));
			
			switch(type) {
				case json_type_boolean: 
				case json_type_double: 
				case json_type_int: 
				case json_type_string:
					gtk_box_pack_end(box_general, grid, FALSE, TRUE, 0);
					
					break;
				case json_type_object:
					GtkWidget *combo = gtk_combo_box_text_new();
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "never", "Not visible");
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "when-active", "Visible when active");
					gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "always", "Always visible");
					gtk_grid_attach(grid, combo, 1, 0, 1, 1);
					gtk_box_pack_end(box_general, grid, FALSE, TRUE, 0);
					
					//jobj = json_object_object_get(jobj, key);
 
					break;
			}
    		}	
	}
	else
	{
		gtk_label_set_text(admin_table_error, "Record fetching error!");
	}
		
		
		

	char *body_prepare = "\
	{\"email\":\"%s\", \	
	\"password\":\"%s\"}";	
	
				strcpy(update_buf, concat(req_addr, "/update/"));
			strcpy(update_id_buf, concat(update_buf, json_object_get_string(json_object_object_get(jobj_child, "id")));
	
	char get_body[1024];
	sprintf(get_body, body_prepare, user_email, user_password);
	
	char *response_body = NULL;
    	int response_body_size = 0;
	
    	int ret_code = http_put(&serv, data, get_body, sizeof(get_body), &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{
		user_is_authenticated = 1;
		
		json_object * jobj = json_tokener_parse(response_body);
}

G_MODULE_EXPORT void
admin_button_delete(GtkWidget* widget, gpointer data)
{
	char *body_prepare = "";
	
	char *response_body = NULL;
    	int response_body_size = 0;
	
    	int ret_code = http_delete(&serv, data, body_prepare, 0, &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{	
		
	}
	else
	{
		gtk_label_set_text(admin_table_error, "Table record deletion error!");
	}

void
login_user_func(void)
{
	login_builder = new_builder_from_file("login_window.glade");
	
	user_login_window = GTK_WIDGET(gtk_builder_get_object(login_builder, "login_user_window"));
	label_login_error = GTK_WIDGET(gtk_builder_get_object(login_builder, "login_error_label"));
	
	gtk_builder_connect_signals(login_builder, NULL);
	gtk_widget_hide(main_window);
	gtk_widget_show(user_login_window);
	open_window = user_login_window;
	open_builder = login_builder;
}

G_MODULE_EXPORT void
auth_login_page_func(GtkWidget* widget, gpointer data)
{	
	strcpy(user_email, gtk_entry_buffer_get_text(gtk_builder_get_object(login_builder, "entrybuffer1")));
	strcpy(user_password, gtk_entry_buffer_get_text(gtk_builder_get_object(login_builder, "entrybuffer2")));
	
	gtk_entry_buffer_delete_text(gtk_builder_get_object(login_builder, "entrybuffer1"), 0, -1);
	gtk_entry_buffer_delete_text(gtk_builder_get_object(login_builder, "entrybuffer2"), 0, -1);

	// Validate for admin credentials
	if(strcmp(admin_email, user_email) == 0 && strcmp(admin_password, user_password) == 0)
	{
		admin_profile_func();
	}


	
	
	///////////////////////
	///Send GET /api/user/byPasswordAndEmail
	///////////////////////
	
	char *body_prepare = "\
	{\"email\":\"%s\", \	
	\"password\":\"%s\"}";	
	
	char get_body[1024];
	sprintf(get_body, body_prepare, user_email, user_password);
	
	char *response_body = NULL;
    	int response_body_size = 0;
	
    	int ret_code = http_get(&serv, "/api/user/byPasswordAndEmail", get_body, sizeof(get_body), &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{
		user_is_authenticated = 1;
		
		json_object * jobj = json_tokener_parse(response_body);
		
		strncpy(user_name, json_object_get_string(json_object_object_get(jobj, "name")), 63);
		strncpy(user_surname, json_object_get_string(json_object_object_get(jobj, "surname")), 63);
		strncpy(user_email, json_object_get_string(json_object_object_get(jobj, "email")), 63);
		strncpy(user_password, json_object_get_string(json_object_object_get(jobj, "password")), 63);
		strncpy(user_phone, json_object_get_string(json_object_object_get(jobj, "phone")), 15);
		strncpy(user_country, json_object_get_string(json_object_object_get(jobj, "country")), 31);
		strncpy(user_city, json_object_get_string(json_object_object_get(jobj, "city")), 31);
		strncpy(user_address, json_object_get_string(json_object_object_get(jobj, "address")), 63);
		strncpy(user_zipcode, json_object_get_string(json_object_object_get(jobj, "zipcode")), 31);
		
		json_object_put(jobj);

		main_window_auth_user();
	}
	else
	{
		gtk_label_set_label(label_login_error, "Authentication error\nTry again");
		
		gtk_entry_buffer_delete_text(gtk_builder_get_object(login_builder, "entrybuffer1"), 0, -1);
		gtk_entry_buffer_delete_text(gtk_builder_get_object(login_builder, "entrybuffer2"), 0, -1);
	}
}		

G_MODULE_EXPORT void
signup_page_func(GtkWidget* widget, gpointer data)
{
	g_object_unref(login_builder);
	signup_builder = new_builder_from_file("signup_window.glade");
	open_builder = signup_builder;
	
	user_signup_window = GTK_WIDGET(gtk_builder_get_object(signup_builder, "signup_user_window"));
	label_signup_error = GTK_WIDGET(gtk_builder_get_object(signup_builder, "signup_error_label"));
	
	gtk_builder_connect_signals(signup_builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(user_signup_window);
	open_window = user_signup_window;
}

G_MODULE_EXPORT void
auth_signup_page_func(GtkWidget* widget, gpointer data)
{
	strcpy(user_name, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "name_buffer"))));
	strcpy(user_surname, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "surname_buffer"))));
	strcpy(user_email, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "email_buffer"))));
	strcpy(user_password, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "password_buffer"))));
	strcpy(user_phone, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "phone_buffer"))));
	strcpy(user_country, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "country_buffer"))));
	strcpy(user_city, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "city_buffer"))));
	strcpy(user_address, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "address_buffer"))));
	strcpy(user_zipcode, gtk_entry_buffer_get_text(GTK_WIDGET(gtk_builder_get_object(signup_builder, "zipcode_buffer"))));

	//////////////////////////
	///Send POST /api/user/add to create new user
	//////////////////////////
	
	char *body_prepare = "\
	{\"name\":\"%s\",\
	\"surname\":\"%s\",\	
	\"phone\":%s,\
	\"email\":\"%s\",\	
	\"password\":\"%s\",\
	\"country\":\"%s\",\
	\"city\":\"%s\",\	
	\"address\":\"%s\",\	
	\"zipcode\":\"%s\" }";	
	
	char post_body[1024];
	sprintf(post_body, body_prepare, user_name, user_surname, user_phone, user_email, user_password, user_country, \
	user_city, user_address, user_zipcode);
	
	char *response_body = NULL;
    	int response_body_size = 0;

    	int ret_code = http_post(&serv, "/api/user/add", post_body, sizeof(post_body), &response_body, &response_body_size);
	
	
	if(ret_code == 200)
	{	
		user_is_authenticated = 1;
		main_window_auth_user();
	}
	else
	{
		gtk_label_set_label(label_signup_error, "Registration error\nTry again");
	}
	
}

G_MODULE_EXPORT void
search_rooms_func(GtkWidget* widget, gpointer data)
{
	aux_cal_date_from.year = cal_date_from.year;
	aux_cal_date_from.month = cal_date_from.month;
	aux_cal_date_from.day = cal_date_from.day;
	aux_cal_date_to.year = cal_date_to.year;
	aux_cal_date_to.month = cal_date_to.month;
	aux_cal_date_to.day = cal_date_to.day;
		
	///////////////////////////////
	///Make GET /api/room_booked/getRoomByDate
	//////////////////////////////
	
	//////////////////////////////////////////
	struct room_listbox_data{
	char room_type[8];
	char description[2][30];
	double price;
	};
	struct room_listbox_data rooms_data[8];

	struct room_listbox{
		GtkWidget *grid_parent;
		GtkWidget *image;
		GtkWidget *book_button;
		GtkWidget *room_type;
		GtkWidget *description;
		GtkWidget *price;
	};
	struct room_listbox rooms_display[8];
	//////////////////////////////////////////
	
	char *body_prepare = "\
	{\"dateFrom\":\"%s\", \	
	\"dateTo\":\"%s\"}";
	
	char buf_date_from[16];
	char buf_date_to[16];
	
	sprintf(buf_date_from, "%d-%02d-%02d", aux_cal_date_from.year, aux_cal_date_from.month, aux_cal_date_from.day);
	sprintf(buf_date_to, "%d-%02d-%02d", aux_cal_date_to.year, aux_cal_date_to.month, aux_cal_date_to.day);
	
	char post_body[1024];
	sprintf(post_body, body_prepare, buf_date_from, buf_date_to);
	
	char *response_body = NULL;
    	int response_body_size = 0;
	
    	int ret_code = http_post(&serv, "/api/room_booked/getRoomByDate", post_body, sizeof(post_body), &response_body, &response_body_size);
	
	if(ret_code == 200 && *response_body)
	{	
		json_object * jobj = json_tokener_parse(response_body);
		
		int rooms_count_json = json_object_object_length(jobj);
		
		for(int i = 1; i <= rooms_count_json; ++i)
		{
			gtk_label_set_text(rooms_display[i-1].description, json_object_get_string(json_object_object_get(jobj, "description")));
			image;
			book_button;
			room_type;
			description
			price;
		}
		
		json_object_put(jobj);
	}
	else
	{
			
	}
}

G_MODULE_EXPORT void
add_room_to_booking(GtkWidget* widget, gpointer data)
{
	int i;
	for(i = 0; i < rooms_count_response; ++i)
	{
		if(widget == rooms_display[i].book_button)
		{
			printf("FOUND ROOM BUTTON OAOAOAOAOAOAOAOAOAOAOAOAOAOa\n");
			++rooms_count_track;
			break;	
		}
	}
	
	char buf[32];
	int index = rooms_count_track - 1;
	
	sprintf(buf, "%s %d, %d", map_num_to_month[aux_cal_date_from.month], aux_cal_date_from.day, aux_cal_date_from.year);
	gtk_label_set_text(booking_basket[index].label_date_from, buf);
	
	sprintf(buf, "%s %d, %d", map_num_to_month[aux_cal_date_to.month], aux_cal_date_to.day, aux_cal_date_to.year);
	gtk_label_set_text(booking_basket[index].label_date_to, buf);
	
	int nights_count = daysBetweenDates(aux_cal_date_from, aux_cal_date_to);
	sprintf(buf, "%d", nights_count);
	gtk_label_set_text(booking_basket[index].nights, buf);
	
	double room_cost_all_nights = nights_count*atof(gtk_label_get_text(rooms_display[i].price));
	sprintf(buf, "%f", room_cost_all_nights);
	gtk_label_set_text(booking_basket[index].room_cost, buf);
	
	sprintf(buf, "%f", taxes*nights_count);
	gtk_label_set_text(booking_basket[index].taxes, buf);
	
	room_cost_total = room_cost_all_nights + taxes*nights_count;
	rooms_cost_data[index].room_total_cost = room_cost_total;
	rooms_cost_data[index].parent = booking_basket[index].box_parent;
	double total_cost_temp = atof(gtk_label_get_text(total_cost));;
	sprintf(buf, "%f", total_cost_temp + room_cost_total);
	gtk_label_set_text(total_cost, buf);
	
	gtk_widget_show(booking_basket[index].box_parent);
	
	if(rooms_count_track == 2)
	{
		for(i = 0; i < rooms_count_response; ++i)
			gtk_widget_hide(rooms_display[i].book_button);
	}
}

G_MODULE_EXPORT void
remove_room(GtkWidget* widget, gpointer data)
{
	if((widget=gtk_widget_get_parent(widget)) != NULL)
	{
		if((widget=gtk_widget_get_parent(widget)) != NULL)
		{
			gtk_widget_hide(widget);
			--rooms_count_track;
		}
		else
		{
			printf("ERROR 2 IN REMOVE_ROOM_1 FUNCTION!\n");
		}
	}
	else
	{
		printf("ERROR 1 IN REMOVE_ROOM_1 FUNCTION!\n");
	}
		
	if(rooms_count_track == 1)
	{
		double temp_total_cost = atof(gtk_label_get_text(total_cost));
		if(rooms_cost_data[0].parent == widget)
			temp_total_cost -= rooms_cost_data[0].room_total_cost;
		else if(rooms_cost_data[1].parent == widget)
			temp_total_cost -= rooms_cost_data[1].room_total_cost;
		else
			printf("ERROR 3 IN REMOVE_ROOM_1 FUNCTION!\n");	
		char buf[16];
		sprintf(buf, "%f", temp_total_cost);
		gtk_label_set_text(total_cost, buf);
		
		for(int i = 0; i < rooms_count_response; ++i)
			gtk_widget_show(rooms_display[i].book_button);
	}
	else
	{
		gtk_label_set_text(total_cost, "0");
		gtk_widget_hide(submit_booking_button);
	}
}

int daysBetweenDates(struct date date1, struct date date2)
{
        return abs(getDays(date1.year, date1.month, date1.day) - getDays(date2.year, date2.month, date2.day));
}

int getDays(int year, int month, int day) 
{
	int ans = 0;
	for (int i = 1900; i < year; ++ i) 
	{
	    if (isLeap(i))
		ans += 366;
	    else
		ans += 365;
	}
	for (int i = 1; i < month; ++ i) 
	{
	    switch(i) 
	    {
		case 1: ans += 31; break;
		case 2: ans += isLeap(year) ? 29 : 28; break;
		case 3: ans += 31; break;
		case 4: ans += 30; break;
		case 5: ans += 31; break;
		case 6: ans += 30; break;
		case 7: ans += 31; break;
		case 8: ans += 31; break;
		case 9: ans += 30; break;
		case 10: ans += 31; break;
		case 11: ans += 30; break;
		case 12: ans += 31; break;
	    }
	}
	return ans += day - 1;
}
    
int isLeap(int num) 
{
	if (num % 400 == 0)
	    return 1;
	else if (num % 100 == 0)
	    return 0;
	else if (num % 4 == 0)
	    return 1;
	else
	    return 0;        
}






