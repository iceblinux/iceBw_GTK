/*$Id: l_uplsal_p.c,v 1.10 2013/08/25 08:26:42 sasa Exp $*/
/*07.03.2008	07.03.2008	Белых А.И.	l_uplsal_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_uplsal.h"
enum
 {
  E_GOD,
  E_KOD_AVT,
  E_KOD_VOD,
  E_KOD_TOP,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplsal_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplsal_rek *rk;
  
  
  l_uplsal_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    rk->kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT])));
    rk->kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD])));
    rk->kod_top.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP])));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_uplsal_p_clear(class l_uplsal_p_data *data);
void    l_uplsal_p_vvod(GtkWidget *widget,class l_uplsal_p_data *data);
void  l_uplsal_p_knopka(GtkWidget *widget,class l_uplsal_p_data *data);
gboolean   l_uplsal_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_p_data *data);

void  l_uplsal_p_v_knopka(GtkWidget *widget,class l_uplsal_p_data *data);


int l_uplsal_p(class l_uplsal_rek *rek_poi,GtkWidget *wpredok)
{
class l_uplsal_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplsal_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Год"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));



sprintf(strsql,"%s (,,)",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_AVT],"clicked",G_CALLBACK(l_uplsal_p_v_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"));

data.entry[E_KOD_AVT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_AVT], "activate",G_CALLBACK(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk->kod_avt.ravno());
gtk_widget_set_name(data.entry[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));


sprintf(strsql,"%s (,,)",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_VOD],"clicked",G_CALLBACK(l_uplsal_p_v_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"));

data.entry[E_KOD_VOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_VOD], "activate",G_CALLBACK(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk->kod_vod.ravno());
gtk_widget_set_name(data.entry[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));


sprintf(strsql,"%s (,,)",gettext("Код топлива"));
data.knopka_enter[E_KOD_TOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter[E_KOD_TOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_TOP],"clicked",G_CALLBACK(l_uplsal_p_v_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_TOP],iceb_u_inttochar(E_KOD_TOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_TOP],gettext("Выбор топлива"));

data.entry[E_KOD_TOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_TOP], "activate",G_CALLBACK(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk->kod_top.ravno());
gtk_widget_set_name(data.entry[E_KOD_TOP],iceb_u_inttochar(E_KOD_TOP));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uplsal_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_uplsal_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uplsal_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplsal_p_v_knopka(GtkWidget *widget,class l_uplsal_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD_AVT:
     if(l_uplavt0(1,&kod,&naim,data->window) == 0)
      data->rk->kod_avt.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk->kod_avt.ravno());
      
    return;  

  case E_KOD_VOD:
     if(l_uplout(1,&kod,&naim,1,data->window) == 0)
      data->rk->kod_vod.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk->kod_vod.ravno());
      
    return;  

  case E_KOD_TOP:
     if(l_uplmt(1,&kod,&naim,data->window) == 0)
      data->rk->kod_top.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->rk->kod_top.ravno());
      
    return;  

 }   
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplsal_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_p_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
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
void  l_uplsal_p_knopka(GtkWidget *widget,class l_uplsal_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uplsal_p_vvod(GtkWidget *widget,class l_uplsal_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_GOD:
    data->rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_AVT:
    data->rk->kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
