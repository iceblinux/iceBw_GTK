/*$Id: l_uplmt_v.c,v 1.16 2013/08/25 08:26:42 sasa Exp $*/
/*10.07.2015	05.03.2008	Белых А.И.	l_uplmt_v.c
Ввод и корректировка марок топлива
*/
#include "buhg_g.h"
#include "l_uplmt.h"
enum
 {
  E_KOD_TOP,
  E_KOD_MAT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplmt_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_mat;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplmt_rek rk;
  class iceb_u_str kod_topliva;  
  
  l_uplmt_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod_top.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP])));
    rk.kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_MAT])));
   }
 };


gboolean   l_uplmt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplmt_v_data *data);
void  l_uplmt_v_knopka(GtkWidget *widget,class l_uplmt_v_data *data);
void    l_uplmt_v_vvod(GtkWidget *widget,class l_uplmt_v_data *data);
int l_uplmt_pk(const char *god,GtkWidget*);
int l_uplmt_zap(class l_uplmt_v_data *data);
void l_uplmt_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_uplmt_v_e_knopka(GtkWidget *widget,class l_uplmt_v_data *data);

extern SQL_baza  bd;

int l_uplmt_v(class iceb_u_str *kod_topliva,GtkWidget *wpredok)
{
class iceb_u_str naim_mat("");
class l_uplmt_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_topliva.new_plus(kod_topliva->ravno());

data.rk.clear_data();

if(data.kod_topliva.getdlinna() >  1)
 {
  data.rk.kod_top.new_plus(data.kod_topliva.ravno());
  sprintf(strsql,"select * from Uplmt where kodt=%d",data.kod_topliva.ravno_atoi());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kod_mat.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  /*узнаём наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%d",data.rk.kod_mat.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_mat.new_plus(row[0]);
 }
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_topliva.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplmt_v_key_press),&data);

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
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);


data.label_naim_mat=gtk_label_new(naim_mat.ravno());
gtk_box_pack_start (GTK_BOX (vbox), data.label_naim_mat, FALSE, FALSE,1);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код топлива"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), label, FALSE, FALSE,1);

data.entry[E_KOD_TOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_TOP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_TOP], "activate",G_CALLBACK(l_uplmt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk.kod_top.ravno());
gtk_widget_set_name(data.entry[E_KOD_TOP],iceb_u_inttochar(E_KOD_TOP));


sprintf(strsql,"%s",gettext("Код материалла"));
data.knopka_enter[E_KOD_MAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), data.knopka_enter[E_KOD_MAT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_MAT],"clicked",G_CALLBACK(l_uplmt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_MAT],iceb_u_inttochar(E_KOD_MAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_MAT],gettext("Выбор материалла"));

data.entry[E_KOD_MAT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_MAT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), data.entry[E_KOD_MAT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_MAT], "activate",G_CALLBACK(l_uplmt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_MAT]),data.rk.kod_mat.ravno());
gtk_widget_set_name(data.entry[E_KOD_MAT],iceb_u_inttochar(E_KOD_MAT));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uplmt_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uplmt_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_topliva->new_plus(data.rk.kod_top.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplmt_v_e_knopka(GtkWidget *widget,class l_uplmt_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_uplmt_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_KOD_MAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk.kod_mat.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_MAT]),data->rk.kod_mat.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_mat),naim.ravno());

     }

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplmt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplmt_v_data *data)
{
//char  bros[512];

//printf("l_uplmt_v_key_press\n");
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
void  l_uplmt_v_knopka(GtkWidget *widget,class l_uplmt_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_uplmt_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplmt_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    l_uplmt_v_vvod(GtkWidget *widget,class l_uplmt_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_uplmt_v_vvod enter=%d\n",enter);
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
switch (enter)
 {
  case E_KOD_TOP:
    data->rk.kod_top.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_MAT:
    data->rk.kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    /*узнаём наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%d",data->rk.kod_mat.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);

    gtk_label_set_text(GTK_LABEL(data->label_naim_mat),naim.ravno());

    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_uplmt_zap(class l_uplmt_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.kod_top.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->rk.kod_mat.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено код материалла!"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }
sprintf(strsql,"select kodm from Material where kodm=%d",data->rk.kod_mat.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) < 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не нашли код материалла"),data->rk.kod_mat.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

short metkakor=0;

if(iceb_u_SRAV(data->kod_topliva.ravno(),data->rk.kod_top.ravno(),0) != 0)
  if(l_uplmt_pk(data->rk.kod_top.ravno(),data->window) != 0)
     return(1);

if(data->kod_topliva.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplmt \
values ('%s',%d,%d,%ld)",
   data->rk.kod_top.ravno_filtr(),
   data->rk.kod_mat.ravno_atoi(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_topliva.ravno(),data->rk.kod_top.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplmt \
set \
kodt='%s',\
kodm=%d,\
ktoz=%d,\
vrem=%ld \
where kodt='%s'",
   data->rk.kod_top.ravno_filtr(),
   data->rk.kod_mat.ravno_atoi(),
   iceb_getuid(data->window),vrem,
   data->kod_topliva.ravno_filtr());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_uplmt_kkvt(data->kod_topliva.ravno(),data->rk.kod_top.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplmt_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uplmt where kod='%s'",kod);
//printf("l_uplmt_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void l_uplmt_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Upldok1 set kodtp='%s' where kodtp='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
   return;
  }
 }
sprintf(strsql,"update Upldok2a set kt='%s' where kt='%s'",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Uplsal set kmt='%s' where kmt='%s'",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Uplmtsc set kodt='%s' where kodt='%s'",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}







