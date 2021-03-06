/*$Id: m_vprov_bs.c,v 1.36 2013/12/31 11:49:16 sasa Exp $*/
/*19.06.2019	05.01.2004	Белых А.И.	m_vprov_bs.c
Ввод проводок для балансовых счетов
*/
#include  <math.h>
#include  "buhg_g.h"
#include "prov_poi.h"
#include "prov_rek_data.h"
enum
{
  COL_DATA,
  COL_SHETK,
  COL_KODKON,
  COL_DEBET,
  COL_KREDIT,
  COL_KOMENT,
  COL_KEKV,
  COL_DATA_VREM,
  COL_VREM_LONG,
//  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK1,
  FK2,
  FK3,
  FK4,
  FK6,
  FK5,
  FK7,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DEBET,
  E_KREDIT,
  E_SHETK,
  E_DATA,
  E_KOMENT,
  E_KEKV,
  KOLENTER  
 };

class vprov_bs_data
 {
  public:
  class prov_rek_data rvp;//реквизиты выбранной проводки
  OPSHET     rekshet;
  class iceb_u_str shet;    //Счёт по которому выполняются проводки
  /*реквизиыты в меню*/
  class iceb_u_str debet;   //Сумма по дебету
  class iceb_u_str kredit;  //Сумма по кредиту
  class iceb_u_str shetk;   //Счёт корресподет
  class iceb_u_str data;    //Дата проводки
  class iceb_u_str koment;  //Коментарий к проводке
  class iceb_u_str kontr_sh; //Код контрагента для счёта
  class iceb_u_str kontr_shk; //Код контрагента для счёта корреспондента
  class iceb_u_str kekv;
      
  //запомненные реквизиты от предыдущей проводки
  class iceb_u_str shetk_zap;
  class iceb_u_str koment_zap;
  class iceb_u_str kekv_zap;
  
  short dn,mn,gn;
  short dk,mk,gk;
  short metka_r_saldo; //0- расчёт сальдо не производить 1-производить

  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[2];
  GtkWidget *window;
  GtkWidget *label_prov;
  GtkWidget *label_link;
  GtkWidget *sw;
  GtkWidget *treeview;
  class iceb_u_str name_window;

  short kl_shift;
  time_t vrem_start;  
  int kolzap;
  double suma_debet;
  double suma_kredit;
  
  void clear_zero()
   {
    debet.new_plus("");
    kredit.new_plus("");
    shetk.new_plus("");
    data.new_plus("");
    koment.new_plus("");
    kontr_sh.new_plus("");
    kontr_shk.new_plus("");
    kekv.new_plus("");
   }

  void clear_zero1();

  void read_rek()
   {
    debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DEBET])));
    kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT])));
    shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETK])));
    data.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    kekv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KEKV])));
   }  

  //Конструктор
  vprov_bs_data()
   {
    shetk_zap.plus("");
    koment_zap.plus("");
    kekv_zap.plus("");
    
    kl_shift=0;
    window=NULL;
    clear_zero();
    dn=mn=gn=dk=mk=gk=0;
    metka_r_saldo=0;
    dn=mn=gn=dk=mk=gk=0;
    vrem_start=time(NULL);
    treeview=NULL;
    suma_debet=suma_kredit=0.;
   }


 };

gboolean   vprov_bs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vprov_bs_data *data);
void  vprov_bs_v_knopka(GtkWidget *widget,class vprov_bs_data *data);
void    vprov_bs_v_vvod(GtkWidget *widget,class vprov_bs_data *data);
int   vprov_bs_zp(class vprov_bs_data *data);
void   vregp(GtkWidget*);
void  m_vprov_bs_v_e_knopka(GtkWidget *widget,class vprov_bs_data *data);

int m_vdd(short *dn,short *mn,short *gn,short *dk,short *mk,short *gk,char *naim_menu,GtkWidget *wpredok);
void m_vprov_bs_ppzp(const char *shet,GtkWidget *wpredok);
void m_vprov_bs_showprov(class vprov_bs_data *data);

extern SQL_baza	bd;

void  m_vprov_bs(const char *shet)
{
static class iceb_u_str datpr("");

char strsql[512];
vprov_bs_data data;
int gor=0;
int ver=0;
class iceb_u_str stroka;
time_t vrem;
struct tm *bf;

data.shet.new_plus(shet);
data.data.new_plus(datpr.ravno());
time(&vrem);
bf=localtime(&vrem);
data.name_window.plus(__FUNCTION__);

iceb_prsh1(shet,&data.rekshet,NULL);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),-1,500);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод проводок"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 1);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vprov_bs_v_key_press),&data);

sprintf(strsql,"%s %s:%02d.%02d.%d",
gettext("Ввод проводок"),
gettext("Текущая дата"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

stroka.new_plus(strsql);

sprintf(strsql,"%s %s %s",
gettext("Счёт"),
shet,
data.rekshet.naim.ravno());

stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Тип счета"));
if(data.rekshet.tips == 0)
 strcat(strsql,gettext("Активный"));
if(data.rekshet.tips == 1)
 strcat(strsql,gettext("Пассивный"));
if(data.rekshet.tips == 2)
 strcat(strsql,gettext("Активно-пассивный"));

stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Вид счета"));
if(data.rekshet.vids == 0)
 strcat(strsql,gettext("Счёт"));
if(data.rekshet.vids == 1)
 strcat(strsql,gettext("Субсчёт"));
stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Сальдо"));
if(data.rekshet.saldo == 0)
 strcat(strsql,gettext("Свернутое"));
if(data.rekshet.saldo == 3)
 strcat(strsql,gettext("Развернутое"));
stroka.ps_plus(strsql);

sprintf(strsql,"%s: ",gettext("Статус"));
if(data.rekshet.stat == 0)
 strcat(strsql,gettext("Балансовый"));
if(data.rekshet.stat == 1)
 strcat(strsql,gettext("Внебалансовый"));
stroka.ps_plus(strsql);

GtkWidget *label=gtk_label_new(stroka.ravno());
data.label_prov=gtk_label_new("");


GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

GtkWidget *hbox_main = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
gtk_box_set_homogeneous (GTK_BOX(hbox_main),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_enter[KOLENTER];

for(int nom=0; nom < KOLENTER; nom++)
 {
  hbox_enter[nom] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX(  hbox_enter[nom]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
 
gtk_container_add (GTK_CONTAINER (data.window),hbox_main);
gtk_box_pack_start (GTK_BOX (hbox_main),vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox_main),vbox2, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),label, FALSE, FALSE, 0);

for(int nom=0; nom < KOLENTER; nom++)
  gtk_box_pack_start (GTK_BOX (vbox2),hbox_enter[nom], FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox2),separator1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_prov, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),separator2, FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_end(GTK_BOX (vbox2),data.sw, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Сумма по дебету"));
gtk_box_pack_start (GTK_BOX (hbox_enter[E_DEBET]), label, FALSE, FALSE, 0);

data.entry[E_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DEBET]),20);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_DEBET]), data.entry[E_DEBET], FALSE,FALSE, 0);
g_signal_connect(data.entry[E_DEBET], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.debet.ravno());
gtk_widget_set_name(data.entry[E_DEBET],iceb_u_inttochar(E_DEBET));


label=gtk_label_new(gettext("Сумма по кредиту"));
gtk_box_pack_start (GTK_BOX (hbox_enter[E_KREDIT]), label, FALSE, FALSE, 0);

data.entry[E_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KREDIT]),20);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_KREDIT]), data.entry[E_KREDIT], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KREDIT], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.kredit.ravno());
gtk_widget_set_name(data.entry[E_KREDIT],iceb_u_inttochar(E_KREDIT));


sprintf(strsql,"%s",gettext("Счёт корреспондент"));
data.knopka_enter[0]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_SHETK]), data.knopka_enter[0], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[0],"clicked",G_CALLBACK(m_vprov_bs_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[0],iceb_u_inttochar(E_SHETK));
gtk_widget_set_tooltip_text(data.knopka_enter[0],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETK]),20);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_SHETK]), data.entry[E_SHETK], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_SHETK], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.shetk.ravno());
gtk_widget_set_name(data.entry[E_SHETK],iceb_u_inttochar(E_SHETK));


sprintf(strsql,"%s",gettext("Дата проводки (д.м.г)"));
data.knopka_enter[1]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_DATA]), data.knopka_enter[1], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[1],"clicked",G_CALLBACK(m_vprov_bs_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[1],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[1],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),10);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_DATA]), data.entry[E_DATA], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.data.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));


label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),100);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_KOMENT]), label, FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (hbox_enter[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

label=gtk_label_new(gettext("Код экономической классификации затрат"));
gtk_box_pack_start (GTK_BOX (hbox_enter[E_KEKV]), label, FALSE, FALSE, 0);

data.entry[E_KEKV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KEKV]),6);
gtk_box_pack_start (GTK_BOX (hbox_enter[E_KEKV]), data.entry[E_KEKV], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KEKV], "activate",G_CALLBACK(vprov_bs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.kekv.ravno());
gtk_widget_set_name(data.entry[E_KEKV],iceb_u_inttochar(E_KEKV));

sprintf(strsql,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,__FUNCTION__,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(strsql);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK1], TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Просмотр проводок"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр и корректировка только что сделанных проводок"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Просмотр проводок"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Просмотр и корректировка проводок за период"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Блокировка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Установка или снятие блокировки проводок"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5], TRUE, TRUE, 0);

sprintf(strsql,"F6 %s",gettext("Предыдущ."));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Записать в меню данные из предыдущей проводки"));
g_signal_connect(data.knopka[FK6],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6], TRUE, TRUE, 0);

sprintf(strsql,"F7 %s",gettext("Сальдо"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Включить/выключить расчёт сальдо по счёту"));
g_signal_connect(data.knopka[FK7],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vprov_bs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

datpr.new_plus(data.data.ravno());

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  m_vprov_bs_v_e_knopka(GtkWidget *widget,class vprov_bs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("go_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->data,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->data.ravno());
      
    return;  

  case E_SHETK:

    iceb_vibrek(1,"Plansh",&data->shetk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->shetk.ravno());

    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vprov_bs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vprov_bs_data *data)
{
//char  bros[512];
iceb_u_str shet;
iceb_u_str naim;

//printf("vprov_bs_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(TRUE);

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
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
void  vprov_bs_v_knopka(GtkWidget *widget,class vprov_bs_data *data)
{
//char bros[512];
iceb_u_str shet;
iceb_u_str naim;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;
  case FK2:
    if(vprov_bs_zp(data) == 0)
     {
      m_vprov_bs_showprov(data);
      data->clear_zero1();
     }
    return;  

  case FK3:
    vregp(data->window);
    return;  

  case FK4:
    m_vprov_bs_ppzp(data->shet.ravno(),data->window);
    return;  

  case FK5:
//    iceb_f_redfil("blok.alx",0,data->window);
    iceb_l_blok(data->window);
    return;  

  case FK6:
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->shetk_zap.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->koment_zap.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KEKV]),data->kekv_zap.ravno());
    return;  

  case FK7:

    if(m_vdd(&data->dn,&data->mn,&data->gn,&data->dk,&data->mk,&data->gk,
    gettext("Ввод дат для расчёта сальдо по счёту"),data->window) == 0)
     data->metka_r_saldo=1;
    else
     data->metka_r_saldo=0;

    data->clear_zero1();
     
    return;  

  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vprov_bs_v_vvod(GtkWidget *widget,class vprov_bs_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("vprov_bs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DEBET:
    data->debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KREDIT:
    data->kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SHETK:
    data->shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATA:
    data->data.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

if(enter != 0)
  enter+=1;
else
  enter+=2;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/*****************************************/
/*Запись проводки                        */
/*****************************************/
int    vprov_bs_zp(class vprov_bs_data *data)
{
short d,m,g;
OPSHET rekshet;
char strsql[512];

data->read_rek();

if(iceb_u_rsdat(&d,&m,&g,data->data.ravno(),0) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата проводки !"),data->window);
  return(1);
 }
if(data->shetk.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не веден счёт корреспондент !"),data->window);
  return(1);
 }
if(data->debet.ravno()[0] == '\0' && data->kredit.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введена сума !"),data->window);
  return(1);
 }
if(iceb_pvglkni(m,g,data->window) != 0)
 return(1);


double deb=iceb_u_atof(data->debet.ravno());
double kre=iceb_u_atof(data->kredit.ravno());
if(fabs(deb) < 0.01 && fabs(kre) < 0.01)
 {
  iceb_menu_soob(gettext("Не введён ни дебет ни кредит !"),data->window);
  return(1);
 }

if(fabs(deb) > 0.009  && fabs(kre) > 0.009)
 {
  iceb_menu_soob(gettext("Введен и дебет и кредит !"),data->window);
  return(1);
 }

if(iceb_prsh1(data->shetk.ravno(),&rekshet,data->window) != 0)
 return(1);
 
if(rekshet.stat == 1)
 {
  sprintf(strsql,gettext("Счёт %s внебалансовый !"),data->shetk.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

iceb_u_str kod;
iceb_u_str naim;
short metka_c=0;

if(data->rekshet.saldo == 3) //Основной счёт с развернутым сальдо
 {
  iceb_u_str repl;
  repl.plus(gettext("Введите код контрагента для счёта"));
  repl.plus(" ");
  repl.plus(data->shet.ravno());

  while(metka_c == 0)
   {
  
    if(iceb_menu_vvod1(&repl,&kod,20,__FUNCTION__,data->window) != 0)
      return(1);

    

    if(kod.ravno()[0] == '\0' || kod.ravno()[0] == '+') 
     {
      if(kod.ravno()[0] == '+')
        naim.new_plus(&kod.ravno()[1]);
      else
        naim.new_plus(kod.ravno());
      kod.new_plus("");
      
      if(iceb_l_kontrsh(1,data->shet.ravno(),&kod,&naim,data->window) != 0)
        continue;
      
     }  

    //Проверяем есть ли такой код контрагента
    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
    data->shet.ravno_filtr(),kod.ravno_filtr());
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kod.ravno());
      repl.plus(" ");
      repl.plus(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(data->shet.ravno());
      repl.plus(" !");

      iceb_menu_soob(&repl,data->window);
      continue;
     }

    data->kontr_sh.new_plus(kod.ravno());  
    break;
   } 
 }

metka_c=0;
if(rekshet.saldo == 3) //Счёт корреспондетн с развернутым сальдо
 {
  kod.new_plus("");
  
  iceb_u_str repl;
  repl.plus(gettext("Введите код контрагента для счёта"));
  repl.plus(" ");
  repl.plus(data->shetk.ravno());

  while(metka_c == 0)
   {
  
    if(iceb_menu_vvod1(&repl,&kod,20,__FUNCTION__,data->window) != 0)
      return(1);

    if(kod.ravno()[0] == '\0' || kod.ravno()[0] == '+') 
     {

      naim.new_plus(kod.ravno());
      kod.new_plus("");
      
      if(iceb_l_kontrsh(1,data->shetk.ravno(),&kod,&naim,data->window) != 0)
        continue;
      
     }  

    //Проверяем есть ли такой код контрагента
    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
    data->shetk.ravno_filtr(),kod.ravno_filtr());
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kod.ravno());
      repl.plus(" ");
      repl.plus(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(data->shetk.ravno());
      repl.plus(" !");

      iceb_menu_soob(&repl,data->window);
      continue;
     }

    data->kontr_shk.new_plus(kod.ravno());  
    break;
   } 
 }

time_t vrem;
time(&vrem);

iceb_zapprov(0,g,m,d,data->shet.ravno_filtr(),data->shetk.ravno_filtr(),
data->kontr_sh.ravno_filtr(),data->kontr_shk.ravno_filtr(),"","","",deb,kre,data->koment.ravno_filtr(),2,0,vrem,0,0,0,0,
data->kekv.ravno_atoi(),
data->window);



return(0);

}
/*************************************/
/*Очистка меню после записи и формирование строки выполненной проводки*/
/***********************************************************************/
void vprov_bs_data::clear_zero1()
{
char strsql[1024];
char bros[2048];
class iceb_u_str stroka;
double deb_kre[6];

memset(deb_kre,'\0',sizeof(deb_kre));

if(metka_r_saldo == 1)
 {
  if(rekshet.saldo == 3)
   rpshet_rsw(shet.ravno(),dn,mn,gn,dk,mk,gk,deb_kre,NULL);
  else
   rpshet_ssw(shet.ravno(),dn,mn,gn,dk,mk,gk,deb_kre,NULL);

  sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);

  sprintf(bros,"%-*s:%10s",iceb_u_kolbait(20,strsql),strsql,iceb_u_prnbr(deb_kre[0]));
  stroka.plus(bros);
  
  sprintf(bros," %10s",iceb_u_prnbr(deb_kre[1]));
  stroka.plus(bros);
  
  sprintf(bros,"%-*s:%10s",iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(deb_kre[2]));
  stroka.ps_plus(bros);
  
  sprintf(bros," %10s",iceb_u_prnbr(deb_kre[3]));
  stroka.plus(bros);
  
  sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);
  sprintf(bros,"%-*s:%10s",iceb_u_kolbait(20,strsql),strsql,iceb_u_prnbr(deb_kre[4]));
  stroka.ps_plus(bros);
  
  sprintf(bros," %10s",iceb_u_prnbr(deb_kre[5]));
  stroka.plus(bros);

  stroka.plus("\n\n");

 }

sprintf(strsql,"%s:\n%s %.2f %s %.2f",
gettext("Сумма выполненых проводок"),
gettext("Дебет"),
suma_debet,
gettext("Кредит"),
suma_kredit);
stroka.plus(strsql);

gtk_label_set_text(GTK_LABEL(label_prov),stroka.ravno());
gtk_widget_show(label_prov);


//Запоминаем
shetk_zap.new_plus(shet.ravno());
koment_zap.new_plus(koment.ravno());
kekv_zap.new_plus(kekv.ravno());
//Очищаем

debet.new_plus("");
kredit.new_plus("");
shetk.new_plus("");

koment.new_plus("");
kontr_sh.new_plus("");
kontr_shk.new_plus("");

gtk_entry_set_text(GTK_ENTRY(entry[E_DEBET]),debet.ravno());
gtk_entry_set_text(GTK_ENTRY(entry[E_KREDIT]),kredit.ravno());
gtk_entry_set_text(GTK_ENTRY(entry[E_SHETK]),shetk.ravno());
gtk_entry_set_text(GTK_ENTRY(entry[E_DATA]),data.ravno());
gtk_entry_set_text(GTK_ENTRY(entry[E_KOMENT]),koment.ravno());
gtk_entry_set_text(GTK_ENTRY(entry[E_KOMENT]),kekv.ravno());

gtk_widget_grab_focus(entry[0]);

}
/***********************************/
/*Просмотр проводок за период*/
/********************************/

void m_vprov_bs_ppzp(const char *shet,GtkWidget *wpredok)
{
static short dn=0,mn=0,gn=0;
static short dk=0,mk=0,gk=0;
class prov_poi_data data;

if(m_vdd(&dn,&mn,&gn,&dk,&mk,&gk,gettext("Ввод дат для просмотра проводок за период"),wpredok) != 0)
  return;

char bros[512];

sprintf(bros,"%d.%d.%d",dn,mn,gn);
data.datan.new_plus(bros);

sprintf(bros,"%d.%d.%d",dk,mk,gk);
data.datak.new_plus(bros);

forzappoi(&data);
l_prov(&data,wpredok);


}
/*******************/
/*выбрать режим*/
/*********************/

int m_vprov_bs_menur(GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Корректировать проводку"));//0
punkt_m.plus(gettext("Удалить проводку"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok));

}

/****************************/
/*Выбор строки*/
/**********************/
void m_vprov_bs_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class vprov_bs_data *data)
{
int nomr=0;
if((nomr=m_vprov_bs_menur(data->window)) < 0)
 return;

if(nomr == 0)  //корректировать проводку
 {
  if(l_prov_kr(&data->rvp,data->window) != 0)
   return;


 } 

if(nomr == 1) //удалить проводку
 {
  if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;

  iceb_udprov(0,data->rvp.datap.ravno(),data->rvp.shet.ravno(),data->rvp.shetk.ravno(),data->rvp.vremz,data->rvp.debet.ravno_atof(),data->rvp.kredit.ravno_atof(),data->rvp.koment.ravno(),2,data->window);
 }
m_vprov_bs_showprov(data);

data->clear_zero1();

}

/****************************/
/*чтение данных стороки на которой установлена подсветка*/
/**********************/

void m_vprov_bs_readstr(GtkTreeSelection *selection,class vprov_bs_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;


gchar *deb;
gchar *kre;
gchar *shet;
gchar *datap;
gchar *koment;
gchar *kod_kontr;
glong  vrem=0;

gtk_tree_model_get(model,&iter,
COL_DEBET,&deb,
COL_KREDIT,&kre,
COL_SHETK,&shet,
COL_DATA,&datap,
COL_KOMENT,&koment,
COL_KODKON,&kod_kontr,
COL_VREM_LONG,&vrem,-1);

data->rvp.clear_zero();
data->rvp.val=0;
data->rvp.debet.new_plus(deb);
data->rvp.kredit.new_plus(kre);
data->rvp.shet.new_plus(data->shet.ravno());
data->rvp.shetk.new_plus(shet);
data->rvp.datap.new_plus(datap);
data->rvp.koment.new_plus(koment);

class iceb_u_str kod;
iceb_u_pole(kod_kontr,&kod,1,' ');
data->rvp.kontr.new_plus(kod.ravno());

data->rvp.vremz=vrem;
data->rvp.ktoz=iceb_getuid(data->window);

g_free(deb);
g_free(kre);
g_free(shet);
g_free(datap);
g_free(koment);
g_free(kod_kontr);
}
/*****************/
/*Создаем колонки*/
/*****************/

void m_vprov_bs_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата"),renderer,"text",COL_DATA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Счёт к."),renderer,"text",COL_SHETK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHETK);
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дебет"),renderer,"text",COL_DEBET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DEBET);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кредит"),renderer,"text",COL_KREDIT ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KREDIT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Контрагент"),renderer,"text",COL_KODKON,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODKON);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("КЭКЗ"),renderer,"text",COL_KEKV ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KEKV);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); //Разрешение на изменение размеров колонки
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);
/*************
renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); //Разрешение на изменение размеров колонки
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);
********************/
}

/************************************************/
/*Подпрограмма формирования списка сделанных проводок*/
/*************************************************/
void m_vprov_bs_showprov(class vprov_bs_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(m_vprov_bs_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(m_vprov_bs_readstr),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_LONG,
G_TYPE_INT);
sprintf(strsql,"select datp,shk,kodkon,deb,kre,komen,kekv,vrem,ktoi from Prov where vrem >= %ld and ktoi=%d and sh='%s' order by vrem desc",data->vrem_start,iceb_getuid(data->window),data->shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

data->suma_debet=data->suma_kredit=0.;
class iceb_u_str naim_kontr("");
data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  naim_kontr.new_plus("");
  if(row[2][0] != '\0')
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[2]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      naim_kontr.new_plus(row[2]);
      naim_kontr.plus(" ",row1[0]);
     }
   }
  

  data->suma_debet+=atof(row[3]);
  data->suma_kredit+=atof(row[4]);
 
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,iceb_u_datzap(row[0]),
  COL_SHETK,row[1],
  COL_KODKON,naim_kontr.ravno(),
  COL_DEBET,row[3],
  COL_KREDIT,row[4],
  COL_KOMENT,row[5],
  COL_KEKV,row[6],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
//  COL_KTO,ss[COL_KTO].ravno(),
  COL_VREM_LONG,atol(row[7]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

m_vprov_bs_add_columns (GTK_TREE_VIEW (data->treeview));



gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
if(data->kolzap > 0)
 {
  int snanomer=0;
  iceb_snanomer(data->kolzap,&snanomer,data->treeview);
 }

}
