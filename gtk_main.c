#include <gtk/gtk.h>

#include <stdio.h>

char* conc_str_int(const char *str, int num, char *buff)
{
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%s%d", str, num);
	return buff;
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

struct{
	double room_cost;
	int nights;
	double taxes;	// client sets it
} booking_rooms_data;

struct booking_rooms_data[2];

struct{
	GtkWidget *label_date_from;
	GtkWidget *label_date_to;
	GtkWidget *room_cost;
	GtkWidget *nights;
	GtkWidget *remove_button;
} booking_rooms_frame;

struct booking_rooms_frame[2];

struct{
	char room_type[8];
	char details[256];
	char description[2][30];
	double price;
} room_listbox_data;

struct room_listbox_data rooms_data[8];

struct{
	GtkWidget *image;
	GtkWidget *book_button;
	GtkWidget *room_type;
	GtkWidget *details;
	GtkWidget *description;
	GtkWidget *price;
} room_listbox;

GtkWidget *parent_rooms_listbox;

struct room_listbox rooms_display[8];

struct{
	int year;
	int month;
	int day;
} cal_date_from, cal_date_to;

int user_is_authenticated = 0;
char *user_name = NULL;
///////////////////////

GtkBuilder*
new_builder_from_file(const char *filename);

G_MODULE_EXPORT void
auth_user_and_redirect(GtkWidget* widget, gpointer data);

main_window_auth_user(void);

////
G_MODULE_EXPORT void
book_room_func(GtkWidget* widget, gpointer data);

G_MODULE_EXPORT void
login_user_func(GtkWidget* widget, gpointer data);
////

G_MODULE_EXPORT void
user_orders_func(GtkWidget* widget, gpointer data);

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


int main(int argc, char** argv)
{
	GtkBuilder *builder;
	
	gtk_init(&argc, &argv);

	if((builder = new_builder_from_file("home_glade.glade")) == NULL)
		return 1;

	main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
  	
  	/*Hide button and show it again if user is logged in (=authenticated)*/
  	button_user_page = GTK_WIDGET(gtk_builder_get_object(builder, "user_page_button"));
  	button_logout = GTK_WIDGET(gtk_builder_get_object(builder, "logout_button"));

  	main_window_auth_user(void);
  	
  	gtk_builder_connect_signals(builder, NULL);
	
	g_object_unref(builder);

    gtk_widget_show(main_window);
    
    open_window = main_window;
	

	gtk_main ();

	return 0;

}


GtkBuilder *builder
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

G_MODULE_EXPORT void
main_window_auth_user(void)
{
	if(user_is_authenticated)
  	{
  		gtk_button_set_label(button_user_page, user_name);
  		gtk_widget_show(button_user_page);
  		gtk_widget_show(button_logout);
  	}
  	gtk_widget_hide(button_user_page);
  	gtk_widget_hide(button_logout);
  	
  	gtk_widget_hide(open_window);
  	gtk_widget_hide(main_window);
}

G_MODULE_EXPORT void
book_room_func(void)
{
	GtkBuilder *builder = new_builder_from_file("book_window.glade");

	book_main_window = GTK_WIDGET(gtk_builder_get_object(builder, "book_main_window"));
	
	label_date_from_search = GTK_WIDGET(gtk_builder_get_object(builder, "label_date_from_search"));
	label_date_to_search = GTK_WIDGET(gtk_builder_get_object(builder, "label_date_to_search"));
	
	//parent_rooms_listbox = GTK_WIDGET(gtk_builder_get_object(builder, "parent_listbox"));
	
	for(int i = 1; i <= 8; ++i)
	{
		temp_buf[32];
		rooms_display[i-1]->image = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("image_room", i, buf)));
		rooms_display[i-1]->book_button = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("add_room_button", i, buf)));
		rooms_display[i-1]->room_type = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_type_label", i, buf)));
		rooms_display[i-1]->details = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("textview_details", i, buf)));
		rooms_display[i-1]->description = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("short_description_label", i, buf)));
		rooms_display[i-1]->price = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("price_label", i, buf)));
	}
	
	for(int i = 1; i <= 2; ++i)
	{
		temp_buf[32];
		booking_rooms_frame[i-1]->label_date_from = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("label_date_from_booking", i, buf)));
		booking_rooms_frame[i-1]->label_date_to = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("label_date_to_booking", i, buf)));
		booking_rooms_frame[i-1]->room_cost = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("room_cost", i, buf)));
		booking_rooms_frame[i-1]->nights = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("nights_count", i, buf)));
		booking_rooms_frame[i-1]->remove_button = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("remove_room", i, buf)));
	}
	
	
	
	////////////////////// HOW TO SET DATE AT THE BEGINNING??? ///////////
	get_calendar_from_date(calendar_from, &cal_date_from);
	get_calendar_to_date(calendar_to, &cal_date_to);
	set_date_labels_search();
	/////////////////////
	
	
	
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(book_main_window);
	open_window = book_main_window;
	g_object_unref(builder);
}

void
set_date_from_label_search(void)
{
	char date_from_buff[16];
	sprintf(date_from_buff, "%02d.%02d.%d", cal_date_from.day, cal_date_from.month, 																	cal_date_from.year);
	gtk_label_set_text(label_date_from_search, date_from_buff);
}

void
set_date_to_label_search(void)
{
	char date_to_buff[16];
	sprintf(date_to_buff, "%02d.%02d.%d", cal_date_to.day, cal_date_to.month, 																	cal_date_to.year);
	gtk_label_set_text(label_date_to_search, date_to_buff);
	
}

G_MODULE_EXPORT void
get_calendar_from_date(GtkWidget* widget, gpointer data)
{
	gtk_calendar_get_date(calendar_from, &cal_date_from.year, &cal_date_from.month + 1, 																&cal_date_from.day);
	set_date_from_label_search();
}

G_MODULE_EXPORT void
get_calendar_to_date(GtkWidget* widget, gpointer data)
{
	gtk_calendar_get_date(calendar_to, &cal_date_to.year, &cal_date_to.month + 1, 																	&cal_date_to.day);
	set_date_to_label_search();
}

G_MODULE_EXPORT void
user_orders_func(void)
{
	GtkBuilder *builder = new_builder_from_file("user_orders_window.glade");
	
	user_orders_window = GTK_WIDGET(gtk_builder_get_object(builder, "user_page_window"));
	
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(user_orders_window);
	open_window = user_orders_window;
	g_object_unref(builder);
}

G_MODULE_EXPORT void
logout_func(void)
{
	user_is_authenticated = 0;
	main_window_auth_user();
}

G_MODULE_EXPORT void
login_user_func(void)
{
	GtkBuilder *builder = new_builder_from_file("login_window.glade");
	
	user_login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_user_window"));
	label_login_error = GTK_WIDGET(gtk_builder_get_object(builder, "login_error_label"));
	login_grid =  = GTK_WIDGET(gtk_builder_get_object(builder, "login_grid"));
	
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(main_window);
	gtk_widget_show(user_login_window);
	open_window = user_login_window;
	g_object_unref(builder);
}

G_MODULE_EXPORT void
auth_login_page_func(GtkWidget* widget, gpointer data)
{
	//////////////////////////
	///Send GET request to authenticate user by email and password
	//////////////////////////
	if()
	{
		////////////
		//store all user information in global variables
		////////////
		user_is_authenticated = 1;
		main_window_auth_user();
	}
	else
	{
		gtk_label_set_label(label_login_error, "Authentication error\nTry again");
		gtk_entry_set_text(gtk_grid_get_child_at(login_grid, 1, 0), "");
		gtk_entry_set_text(gtk_grid_get_child_at(login_grid, 1, 2), "");
	}
}

G_MODULE_EXPORT void
auth_signup_page_func(GtkWidget* widget, gpointer data)
{
	//////////////////////////
	///Send POST request to register a new user record
	//////////////////////////
	if()
	{
		////////////
		//store all user information in global variables
		////////////
		user_is_authenticated = 1;
		main_window_auth_user();
	}
	else
	{
		gtk_label_set_label(label_signup_error, "Registration error\nTry again");
		for(int i = 0; i <= 14; i+=2)
			gtk_entry_set_text(gtk_grid_get_child_at(signup_grid, 1, i), "");
	}
}

G_MODULE_EXPORT void
signup_page_func(GtkWidget* widget, gpointer data)
{
	GtkBuilder *builder = new_builder_from_file("signup_window.glade");
	
	user_signup_window = GTK_WIDGET(gtk_builder_get_object(builder, "signup_user_window"));
	label_signup_error = GTK_WIDGET(gtk_builder_get_object(builder, "signup_error_label"));
	signup_grid =  = GTK_WIDGET(gtk_builder_get_object(builder, "signup_grid"));
	
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_hide(open_window);
	gtk_widget_show(user_signup_window);
	open_window = user_signup_window;
	g_object_unref(builder);
}

G_MODULE_EXPORT void
search_rooms_func(GtkWidget* widget, gpointer data)
{
	
}








