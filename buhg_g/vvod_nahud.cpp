/*$Id: vvod_nahud.c,v 1.13 2013/09/26 09:46:59 sasa Exp $*/
/*25.10.2019	20.09.2006	Белых А.И.	vvod_nahud.cpp
Ввод кода подразделения для подсистемы "Заработная плата"
*/
#include "buhg_g.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_NAHUD,
 KOLENTER
};


class vvod_nahud_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
  class iceb_u_str nahud;
  int prn;
       
  vvod_nahud_data()
   {
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    nahud.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAHUD])));
   }
  
 };


gboolean   vvod_nahud_key_press(GtkWidget *widget,GdkEventKey *event,class vvod_nahud_data *data);
void  vvod_nahud_knopka(GtkWidget *widget,class vvod_nahud_data *data);
void    vvod_nahud_vvod(GtkWidget *widget,class vvod_nahud_data *data);
int l_vrint_zap(class vvod_nahud_data *data);
void l_vrint_kodgr(class vvod_nahud_data *data);
void  vvod_nahud_e_knopka(GtkWidget *widget,class vvod_nahud_data *data);

extern SQL_baza	bd;


int vvod_nahud(int prn, //1-начисления 2-удержания
class iceb_u_str *nahud,GtkWidget *wpredok)
{
char strsql[512];
class vvod_nahud_data data;
data.prn=prn;

    
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vvod_nahud_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
if(data.prn == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод кода начисления"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод кода удержания"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
if(data.prn == 1)
  label=gtk_label_new(gettext("Введите код начисления"));
if(data.prn == 2)
  label=gtk_label_new(gettext("Введите код удержания"));


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

//sprintf(strsql,"%s",gettext("Код подразделения"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_NAHUD]), label, FALSE, FALSE, 0);
if(data.prn == 1)
 data.knopka_enter[E_NAHUD]=gtk_button_new_with_label(gettext("Код начисления"));
if(data.prn == 2)
 data.knopka_enter[E_NAHUD]=gtk_button_new_with_label(gettext("Код удержания"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAHUD]), data.knopka_enter[E_NAHUD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NAHUD],"clicked",G_CALLBACK(vvod_nahud_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NAHUD],iceb_u_inttochar(E_NAHUD));
if(data.prn == 1)
 gtk_widget_set_tooltip_text(data.knopka_enter[E_NAHUD],gettext("Выбор кода начисления"));
if(data.prn == 2)
 gtk_widget_set_tooltip_text(data.knopka_enter[E_NAHUD],gettext("Выбор кода удержания"));


data.entry[E_NAHUD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAHUD]),11);
gtk_box_pack_start (GTK_BOX (hbox[E_NAHUD]), data.entry[E_NAHUD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAHUD], "activate",G_CALLBACK(vvod_nahud_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAHUD]),"");
gtk_widget_set_name(data.entry[E_NAHUD],iceb_u_inttochar(E_NAHUD));




sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vvod_nahud_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vvod_nahud_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 nahud->new_plus(data.nahud.ravno());

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vvod_nahud_e_knopka(GtkWidget *widget,class vvod_nahud_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_NAHUD:

//    g_signal_emit_by_name(data->entry[E_NOMST],"activate");
    if(data->prn == 1)
     if(l_zarnah(1,&kod,&naim,data->window) == 0)
      data->nahud.new_plus(kod.ravno());
    if(data->prn == 2)
     if(l_zarud(1,&kod,&naim,data->window) == 0)
      data->nahud.new_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAHUD]),data->nahud.ravno());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vvod_nahud_key_press(GtkWidget *widget,GdkEventKey *event,class vvod_nahud_data *data)
{

//printf("vvod_nahud_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  vvod_nahud_knopka(GtkWidget *widget,class vvod_nahud_data *data)
{
char strsql[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    //Проверяем код
    if(data->prn == 1)
     sprintf(strsql,"select kod from Nash where kod=%d",data->nahud.ravno_atoi());
    if(data->prn == 2)
     sprintf(strsql,"select kod from Uder where kod=%d",data->nahud.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      if(data->prn == 1)
       sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),data->nahud.ravno_atoi());
      if(data->prn == 2)
       sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),data->nahud.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vvod_nahud_vvod(GtkWidget *widget,class vvod_nahud_data *data)
{
char strsql[512];

int enter=atoi(gtk_widget_get_name(widget));
//g_print("vvod_nahud_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NAHUD:
    data->nahud.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->nahud.getdlinna() <= 1)
     break;
    //Проверяем код
    if(data->prn == 1)
     sprintf(strsql,"select kod from Nash where kod=%d",data->nahud.ravno_atoi());
    if(data->prn == 2)
     sprintf(strsql,"select kod from Uder where kod=%d",data->nahud.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      if(data->prn == 1)
       sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),data->nahud.ravno_atoi());
      if(data->prn == 2)
       sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),data->nahud.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    break;
 }

 
}
