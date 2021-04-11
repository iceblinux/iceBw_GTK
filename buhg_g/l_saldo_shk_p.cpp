/*$Id: l_saldo_shk_p.c,v 1.17 2014/08/31 06:19:21 sasa Exp $*/
/*18.08.2014	31.12.2003	Белых А.И.	l_saldo_shk_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_saldo_shk.h"

enum
 {
  E_GOD,
  E_SHET,
  E_KODKONTR,
  E_NAIM_KONTR,
  E_DEBET,
  E_KREDIT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };


class saldo_shkp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  saldo_shk_poi *rk;

  saldo_shkp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk->kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODKONTR])));
    rk->debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DEBET])));
    rk->kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT])));
    rk->naim_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_KONTR])));
   }
 };




void saldo_shk_p_clear(class saldo_shkp_data *data);
void    saldo_shk_p_vvod(GtkWidget *widget,class saldo_shkp_data *data);
void  saldo_shk_p_knopka(GtkWidget *widget,class saldo_shkp_data *data);
gboolean   saldo_shk_p_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkp_data *data);

void  l_saldo_shkp_v_e_knopka(GtkWidget *widget,class saldo_shkp_data *data);


int l_saldo_shk_p(class saldo_shk_poi *rkp,GtkWidget *wpredok)
{
char strsql[512];
class saldo_shkp_data data;
data.rk=rkp;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Пoиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldo_shk_p_key_press),&data);

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

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);

GtkWidget *hbox[KOLENTER];
for(int nom=0; nom < KOLENTER; nom++)
 {
  hbox[nom]=gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
  gtk_box_set_homogeneous (GTK_BOX(hbox[nom]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
  gtk_box_pack_start (GTK_BOX (vbox), hbox[nom], TRUE, TRUE,1);
 }

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE,1);

GtkWidget *label=gtk_label_new(gettext("Год"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),5);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_saldo_shkp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KODKONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKONTR]), data.knopka_enter[E_KODKONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODKONTR],"clicked",G_CALLBACK(l_saldo_shkp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODKONTR],iceb_u_inttochar(E_KODKONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODKONTR],gettext("Выбор контрагента"));

data.entry[E_KODKONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODKONTR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKONTR]), data.entry[E_KODKONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KODKONTR], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKONTR]),data.rk->kodkontr.ravno());
gtk_widget_set_name(data.entry[E_KODKONTR],iceb_u_inttochar(E_KODKONTR));

label=gtk_label_new(gettext("Наименование контрагента"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), label, FALSE, FALSE,1);

data.entry[E_NAIM_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM_KONTR]),5);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.entry[E_NAIM_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NAIM_KONTR], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_KONTR]),data.rk->god.ravno());
gtk_widget_set_name(data.entry[E_NAIM_KONTR],iceb_u_inttochar(E_NAIM_KONTR));

label=gtk_label_new(gettext("Дебет"));
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), label, FALSE, FALSE,1);

data.entry[E_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DEBET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), data.entry[E_DEBET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DEBET], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.rk->debet.ravno());
gtk_widget_set_name(data.entry[E_DEBET],iceb_u_inttochar(E_DEBET));


label=gtk_label_new(gettext("Кредит"));
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), label, FALSE, FALSE,1);

data.entry[E_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KREDIT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), data.entry[E_KREDIT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KREDIT], "activate",G_CALLBACK(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.rk->kredit.ravno());
gtk_widget_set_name(data.entry[E_KREDIT],iceb_u_inttochar(E_KREDIT));





sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(saldo_shk_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(saldo_shk_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(saldo_shk_p_knopka),&data);
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
void  l_saldo_shkp_v_e_knopka(GtkWidget *widget,class saldo_shkp_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());

    return;  

  case E_KODKONTR:

    iceb_vibrek(0,"Kontragent",&data->rk->kodkontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKONTR]),data->rk->kodkontr.ravno());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_shk_p_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkp_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    data->kl_shift=1;
    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_shk_p_knopka(GtkWidget *widget,class saldo_shkp_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    
    return;  

  case FK4:
    saldo_shk_p_clear(data);
    return;  


  case FK10:
    data->voz=1;
    data->rk->metka_poi=0;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    saldo_shk_p_vvod(GtkWidget *widget,class saldo_shkp_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("saldo_shk_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_GOD:
    data->rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIM_KONTR:
    data->rk->naim_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODKONTR:
    data->rk->kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DEBET:
    data->rk->debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KREDIT:
    data->rk->kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void saldo_shk_p_clear(class saldo_shkp_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
