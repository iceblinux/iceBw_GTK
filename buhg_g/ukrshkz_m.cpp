/*$Id: ukrshkz_m.c,v 1.12 2013/09/26 09:46:56 sasa Exp $*/
/*25.03.2020	28.02.2008	Белых А.И.	ukrshkz_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "ukrshkz.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_KOD_ZAT,
  E_SHET,
  E_KOD_GR_ZAT,
  E_KOD_KONTR,
  KOLENTER  
 };

class ukrshkz_m_data
 {
  public:
  class ukrshkz_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  ukrshkz_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear();
   }
 };


gboolean   ukrshkz_v_key_press(GtkWidget *widget,GdkEventKey *event,class ukrshkz_m_data *data);
void    ukrshkz_v_vvod(GtkWidget *widget,class ukrshkz_m_data *data);
void  ukrshkz_v_knopka(GtkWidget *widget,class ukrshkz_m_data *data);
void  ukrshkz_v_e_knopka(GtkWidget *widget,class ukrshkz_m_data *data);

int ukrshkz_m_provr(class ukrshkz_m_data *data);

extern SQL_baza bd;

int ukrshkz_m(class ukrshkz_data *rek_ras)
{
char strsql[512];


class ukrshkz_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт ведомости по счетам-кодам затрат"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrshkz_v_key_press),&data);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Расчёт ведомости по счетам-кодам затрат"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));



sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));



sprintf(strsql,"%s (,,)",gettext("Код командировочных расходов"));
data.knopka_enter[E_KOD_ZAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_ZAT]), data.knopka_enter[E_KOD_ZAT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_ZAT],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_ZAT],iceb_u_inttochar(E_KOD_ZAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_ZAT],gettext("Выбор кода командировочных расходов"));

data.entry[E_KOD_ZAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_ZAT]), data.entry[E_KOD_ZAT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_ZAT], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_ZAT]),data.rk->kod_zat.ravno());
gtk_widget_set_name(data.entry[E_KOD_ZAT],iceb_u_inttochar(E_KOD_ZAT));


sprintf(strsql,"%s (,,)",gettext("Код группы затрат"));
data.knopka_enter[E_KOD_GR_ZAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR_ZAT]), data.knopka_enter[E_KOD_GR_ZAT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_GR_ZAT],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_GR_ZAT],iceb_u_inttochar(E_KOD_GR_ZAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_GR_ZAT],gettext("Выбор группы"));

data.entry[E_KOD_GR_ZAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR_ZAT]), data.entry[E_KOD_GR_ZAT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_GR_ZAT], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR_ZAT]),data.rk->kod_gr_zat.ravno());
gtk_widget_set_name(data.entry[E_KOD_GR_ZAT],iceb_u_inttochar(E_KOD_GR_ZAT));


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KOD_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.knopka_enter[E_KOD_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_KONTR],"clicked",G_CALLBACK(ukrshkz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KONTR],gettext("Выбор контрагента"));

data.entry[E_KOD_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.entry[E_KOD_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_KONTR], "activate",G_CALLBACK(ukrshkz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KONTR]),data.rk->kod_kontr.ravno());
gtk_widget_set_name(data.entry[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(ukrshkz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(ukrshkz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(ukrshkz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  ukrshkz_v_e_knopka(GtkWidget *widget,class ukrshkz_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("ukrshkz_v_e_knopka knop=%d\n",knop);*/

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  


  case E_KOD_ZAT:

    if(l_ukrzat(1,&kod,&naim,data->window) == 0)
      data->rk->kod_zat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_ZAT]),data->rk->kod_zat.ravno());
      
    return;  


  case E_KOD_GR_ZAT:

    if(l_ukrgrup(1,&kod,&naim,data->window) == 0)
     data->rk->kod_gr_zat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR_ZAT]),data->rk->kod_gr_zat.ravno());
      
    return;  

  case E_SHET:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
     data->rk->shet.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
      
    return;  

  case E_KOD_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     data->rk->kod_kontr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KONTR]),data->rk->kod_kontr.ravno());
      
    return;  


   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrshkz_v_key_press(GtkWidget *widget,GdkEventKey *event,class ukrshkz_m_data *data)
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrshkz_v_knopka(GtkWidget *widget,class ukrshkz_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(ukrshkz_m_provr(data) != 0)
     return;
    
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    ukrshkz_v_vvod(GtkWidget *widget,class ukrshkz_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("ukrshkz_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_ZAT:
    data->rk->kod_zat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_GR_ZAT:
    data->rk->kod_gr_zat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_KONTR:
    data->rk->kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int ukrshkz_m_provr(class ukrshkz_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
 return(1);

return(0);
}


