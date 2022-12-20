#include <gtk/gtk.h>

#include <stdio.h>

#include <math.h>

#include <string.h>



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

GtkWidget *total_cost;

GtkWidget *submit_booking_button;



GtkWidget *bookings[2];

GtkWidget *transactions[2];

GtkWidget *statuses[2];



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

	char details[256];

	char description[2][30];

	double price;

};

struct room_listbox_data rooms_data[8];



struct room_listbox{

	GtkWidget *grid_parent;

	GtkWidget *image;

	GtkWidget *book_button;

	GtkWidget *room_type;

	GtkWidget *details;

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



int user_is_authenticated = 0;

char user_name[64];

char user_email[64];

char user_password[64];

char user_surname[64];

char user_country[32];

char user_city[32];

char user_address[64];

char user_zipcode[32];



///////////////////////



GtkWidget *name_entry;



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



G_MODULE_EXPORT void

add_room_to_booking(GtkWidget* widget, gpointer data);



G_MODULE_EXPORT void

remove_room(GtkWidget* widget, gpointer data);




#include "http/http-api.h"

struct http server;

int main(int argc, char** argv)

{

	GtkBuilder *builder;
	
	http_init(&server, argv[1], atoi(argv[2]));
	fprintf(stderr, "Server %s:%s SET", argv[1], argv[2]);

	gtk_init(&argc, &argv);

	if((builder = new_builder_from_file("home_glade.glade")) == NULL)
		return 1;



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

  		gtk_button_set_label(button_user_page, user_name);

  		gtk_widget_show(button_user_page);

  		gtk_widget_show(button_logout);

  	}

  	gtk_widget_hide(button_user_page);

  	gtk_widget_hide(button_logout);

  	

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

		rooms_display[i-1].details = GTK_WIDGET(gtk_builder_get_object(builder, conc_str_int("textview_details", i, buf)));

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

	gtk_widget_hide(open_window);

	gtk_widget_show(book_main_window);

	open_window = book_main_window;

	g_object_unref(builder);

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

user_orders_func(GtkWidget* widget, gpointer data)

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

logout_func(GtkWidget* widget, gpointer data)

{

	user_is_authenticated = 0;

	main_window_auth_user();

}



void

login_user_func(void)

{

	GtkBuilder *builder = new_builder_from_file("login_window.glade");

	

	user_login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_user_window"));

	label_login_error = GTK_WIDGET(gtk_builder_get_object(builder, "login_error_label"));

	login_grid = GTK_WIDGET(gtk_builder_get_object(builder, "login_grid"));

	

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

	// char *response
	// http_get(&server, "/user/login", );

	

	if(1)

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

signup_page_func(GtkWidget* widget, gpointer data)

{

	GtkBuilder *builder = new_builder_from_file("signup_window.glade");

	

	user_signup_window = GTK_WIDGET(gtk_builder_get_object(builder, "signup_user_window"));

	label_signup_error = GTK_WIDGET(gtk_builder_get_object(builder, "signup_error_label"));

	signup_grid = GTK_WIDGET(gtk_builder_get_object(builder, "signup_grid"));

	

	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_hide(open_window);

	gtk_widget_show(user_signup_window);

	open_window = user_signup_window;

	g_object_unref(builder);

}



G_MODULE_EXPORT void

auth_signup_page_func(GtkWidget* widget, gpointer data)

{



	GtkBuilder *builder = new_builder_from_file("signup_window.glade");



	strncpy(user_name, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "name_entry"))), 63);

	strncpy(user_surname, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "surname_entry"))), 63);

	strncpy(user_email, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "email_entry"))), 63);

	strncpy(user_password, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"))), 63);

	strncpy(user_country, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "country_entry"))), 31);

	strncpy(user_city, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "city_entry"))), 31);

	strncpy(user_address, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "address_entry"))), 63);

	strncpy(user_zipcode, gtk_entry_get_text(GTK_WIDGET(gtk_builder_get_object(builder, "zipcode_entry"))), 31);

	

	//////////////////////////

	///Send POST request to register a new user record

	//////////////////////////

	

	if(1)

	{

		// strcpy(user_name, name);

		// strcpy(user_email, email);

		// strcpy(user_password, password);

		

		user_is_authenticated = 1;

		main_window_auth_user();

	}

	else

	{

		gtk_label_set_label(label_signup_error, "Registration error\nTry again");

		for(int i = 0; i <= 14; i+=2)

			gtk_entry_set_text(gtk_grid_get_child_at(signup_grid, 1, i), "");

	}

	

	g_object_unref(builder);

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

	///Make GET request with date interval

	//////////////////////////////

	

	//rooms_count_response -> how many rooms were actually retreived

	

	if(1){

		

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











