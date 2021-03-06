/*$Id: sprtabotpw_m.c,v 1.14 2013/08/25 08:27:01 sasa Exp $*/
/*21.06.2016	26.05.2016	Белых А.И.	sprtabotpw_m.c
Меню для расчёта справок о зарплате
*/
#include "buhg_g.h"
#include "sprtabotpw.h"
enum
 {
  E_TABNOM,
  E_DATAN,
  E_DATAK,
  E_VID_OTP,
  E_PRIKAZ,
  E_PERIOD,
  E_KOL_DNEI,
  E_RI,
  E_KOD_POD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class sprtabotpw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *label_fio;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class sprtabotp_poi *rk;
    
  sprtabotpw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->vid_otp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VID_OTP])));
    rk->prikaz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PRIKAZ])));
    rk->period.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PERIOD])));
    rk->kol_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_DNEI])));
    rk->kod_podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POD])));
    
    rk->metka_ri=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_label_set_text(GTK_LABEL(label_fio),"");
   }
 };


gboolean   sprtabotpw_m_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_m_data *data);
void  sprtabotpw_m_knopka(GtkWidget *widget,class sprtabotpw_m_data *data);
void    sprtabotpw_m_vvod(GtkWidget *widget,class sprtabotpw_m_data *data);


void  sprtabotpw_m_e_knopka(GtkWidget *widget,class sprtabotpw_m_data *data);

extern SQL_baza  bd;

int sprtabotpw_m(class sprtabotp_poi *rek,GtkWidget *wpredok)
{

class sprtabotpw_m_data data;
char strsql[512];
class iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать справку о зарплате"));

label=gtk_label_new(gettext("Распечатать расчёт отпускных"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sprtabotpw_m_key_press),&data);

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
gtk_container_add (GTK_CONTAINER (vbox), label);

data.label_fio=gtk_label_new(data.rk->fio.ravno());
gtk_container_add (GTK_CONTAINER (vbox), data.label_fio);

for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);



gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(sprtabotpw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));



sprintf(strsql,"%s %s",gettext("Дата начала"),gettext("(м.г)"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(sprtabotpw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s %s",gettext("Дата конца"),gettext("(м.г)"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(sprtabotpw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


label=gtk_label_new(gettext("Вид отпуска"));
gtk_box_pack_start (GTK_BOX (hbox[E_VID_OTP]), label, FALSE, FALSE, 0);

data.entry[E_VID_OTP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_VID_OTP]), data.entry[E_VID_OTP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VID_OTP], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VID_OTP]),data.rk->vid_otp.ravno());
gtk_widget_set_name(data.entry[E_VID_OTP],iceb_u_inttochar(E_VID_OTP));/******************/


label=gtk_label_new(gettext("Приказ"));
gtk_box_pack_start (GTK_BOX (hbox[E_PRIKAZ]), label, FALSE, FALSE, 0);

data.entry[E_PRIKAZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PRIKAZ]), data.entry[E_PRIKAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PRIKAZ], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PRIKAZ]),data.rk->prikaz.ravno());
gtk_widget_set_name(data.entry[E_PRIKAZ],iceb_u_inttochar(E_PRIKAZ));/******************/


label=gtk_label_new(gettext("Период"));
gtk_box_pack_start (GTK_BOX (hbox[E_PERIOD]), label, FALSE, FALSE, 0);

data.entry[E_PERIOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PERIOD]), data.entry[E_PERIOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PERIOD], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PERIOD]),data.rk->period.ravno());
gtk_widget_set_name(data.entry[E_PERIOD],iceb_u_inttochar(E_PERIOD));/******************/


sprintf(strsql,"%s (,,)",gettext("Количество дней отпуска"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), label, FALSE, FALSE, 0);

data.entry[E_KOL_DNEI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), data.entry[E_KOL_DNEI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_DNEI], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_DNEI]),data.rk->period.ravno());
gtk_widget_set_name(data.entry[E_KOL_DNEI],iceb_u_inttochar(E_KOL_DNEI));/******************/


label=gtk_label_new(gettext("Способ расчёта индексации"));
gtk_box_pack_start (GTK_BOX (hbox[E_RI]), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.plus(gettext("Расчёт с использованием должностных окладов"));
spmenu.plus(gettext("Расчёт с использованием коэффициентов"));

iceb_pm_vibor(&spmenu,&data.opt,data.rk->metka_ri);
gtk_box_pack_start (GTK_BOX (hbox[E_RI]),data.opt, FALSE, FALSE, 0);


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_KOD_POD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.knopka_enter[E_KOD_POD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_POD],"clicked",G_CALLBACK(sprtabotpw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_POD],gettext("Выбор кода подразделения"));

data.entry[E_KOD_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.entry[E_KOD_POD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_POD], "activate",G_CALLBACK(sprtabotpw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POD]),data.rk->kod_podr.ravno());
gtk_widget_set_name(data.entry[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));






sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(sprtabotpw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(sprtabotpw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Настройка расчёта"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(sprtabotpw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(sprtabotpw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);


gtk_widget_show_all (data.window);

// gtk_widget_hide(data.knopka[FK5]);
 
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  sprtabotpw_m_e_knopka(GtkWidget *widget,class sprtabotpw_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
class iceb_u_str datnr("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
   
  case E_DATAN:
    iceb_calendar1(&data->rk->datan,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
    return;

  case E_DATAK:
    iceb_calendar1(&data->rk->datak,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
    return;

  case E_TABNOM:

    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     {
      data->rk->tabnom.z_plus(kod.ravno());
      data->rk->fio.new_plus(naim.ravno());
      char strsql[1024];     
      SQL_str row;
      class SQLCURSOR cur;
      sprintf(strsql,"select datn from Kartb where tabn=%d",data->rk->tabnom.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        datnr.new_plus(iceb_u_datzap(row[0]));

      sprintf(strsql,"%s\n%s:%s",data->rk->fio.ravno(),gettext("Дата начала работы"),datnr.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_fio),strsql);

     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
    return;

  case E_KOD_POD:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     {
      data->rk->kod_podr.z_plus(kod.ravno()); 
     }

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POD]),data->rk->kod_podr.ravno());
    return;
  
 }
}


/********************************/
/*меню выбора*/
/***********************/
void sprtabotpw_m_menu(class sprtabotpw_m_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Выбор"));
class iceb_u_str tabnom("");

zagolovok.plus(gettext("Выбор настройки"));

punkt_m.plus(gettext("Работа с файлом настройки"));//0
punkt_m.plus(gettext("Работа со справочником должностных окладов"));//1



  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_f_redfil("zarotp.alx",0,data->window);
      break;


    case 1:
      tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_TABNOM])));

      l_zarsdo(tabnom.ravno_atoi(),data->window);
      break;
 
       
   }
   
}




/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sprtabotpw_m_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_m_data *data)
{
//char  bros[512];

//printf("sprtabotpw_m_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);



  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");

    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");

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
void  sprtabotpw_m_knopka(GtkWidget *widget,class sprtabotpw_m_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:

    data->read_rek();
    if(data->rk->tabnom.getdlinna() > 1 && iceb_u_pole2(data->rk->tabnom.ravno(),',') == 0)
     {
      sprintf(strsql,"select fio from Kartb where tabn=%d",data->rk->tabnom.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->rk->tabnom.ravno_atoi());
        iceb_menu_soob(strsql,data->window);
        return;
       }
      data->rk->fio.new_plus(row[0]);
     }
    
    if(data->rk->datan.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата начала !"),data->window);
      return;
     }

    if(data->rk->datak.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата конца !"),data->window);
      return;
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;

  case FK5:
    sprtabotpw_m_menu(data);

    //iceb_f_redfil("zarotp.alx",0,data->window);
    return;

  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    sprtabotpw_m_vvod(GtkWidget *widget,class sprtabotpw_m_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("sprtabotpw_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select fio,datn from Kartb where tabn=%d",data->rk->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->rk->tabnom.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }
    data->rk->fio.new_plus(row[0]);
    sprintf(strsql,"%s\n%s:%s",row[0],gettext("Дата начала работы"),iceb_u_datzap(row[1]));
    gtk_label_set_text(GTK_LABEL(data->label_fio),strsql);
    break;

 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
