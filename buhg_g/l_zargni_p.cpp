/*$Id: l_zargni_p.c,v 1.13 2013/08/25 08:26:45 sasa Exp $*/
/*23.06.2006	23.06.2006	Белых А.И.	l_zargni_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_zargni.h"
enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_zargni_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_zargni_rek *rk;
  
  
  l_zargni_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_zargni_p_clear(class l_zargni_p_data *data);
void    l_zargni_p_vvod(GtkWidget *widget,class l_zargni_p_data *data);
void  l_zargni_p_knopka(GtkWidget *widget,class l_zargni_p_data *data);
gboolean   l_zargni_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_zargni_p_data *data);


int l_zargni_p(class l_zargni_rek *rek_poi,GtkWidget *wpredok)
{
class l_zargni_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zargni_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_zargni_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_zargni_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(l_zargni_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[PFK4],"clicked",G_CALLBACK(l_zargni_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK4],iceb_u_inttochar(PFK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(l_zargni_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zargni_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_zargni_p_data *data)
{
//char  bros[512];

//printf("vl_zargni_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[PFK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[PFK10],"clicked");
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
void  l_zargni_p_knopka(GtkWidget *widget,class l_zargni_p_data *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zargni_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    data->clear_data();
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zargni_p_vvod(GtkWidget *widget,class l_zargni_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_zargni_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}