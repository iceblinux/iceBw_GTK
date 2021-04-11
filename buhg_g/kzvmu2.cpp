/*$Id:$*/
/*22.03.2017	16.08.2004	Белых А.И.	kzvmu2.c
Коррертировка расходной записи в документе материального учёта 
*/
#include <math.h>
#include "buhg_g.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH,
  E_CENASNDS,
  E_CENABNDS,
  E_SHET,
  E_NOMERZAK,
  E_METKA_IMP_TOV,
  E_KDSTV,
  KOLENTER  
 };

class kzvmu2_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  //Реквизиты меню
  class iceb_u_str kolih;
  class iceb_u_str cenasnds;
  class iceb_u_str cenabnds;
  class iceb_u_str shet;
  class iceb_u_str nomerzak;  
  class iceb_u_str metka_imp_tov;
  class iceb_u_str kdstv;

  //Реквизиты записи
  short dd,md,gd;
  int   kodm;
  class iceb_u_str nomdok;
  int   sklad;
  int   nom_kar;  
  double kolihz;   
  int ktoz;
  
  int sklad1;
  class iceb_u_str nomdokp;
  int nomkarp;
  
  kzvmu2_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }
  void read_rek()
   {
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    cenasnds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_CENASNDS])));
    cenabnds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_CENABNDS])));
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    nomerzak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMERZAK])));
    metka_imp_tov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_METKA_IMP_TOV])));
    kdstv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KDSTV])));
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

void    kzvmu2_v_vvod(GtkWidget *widget,class kzvmu2_data *data);
void  kzvmu2_v_knopka(GtkWidget *widget,class kzvmu2_data *data);
void   kzvmu2_rekviz(class kzvmu2_data *data);
gboolean   kzvmu2_v_key_press(GtkWidget *widget,GdkEventKey *event,class kzvmu2_data *data);

int kzvmu2_zap(class kzvmu2_data *data);

extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;
extern SQL_baza bd;

int kzvmu2(short dd,short md,short gd,int skl,
const char *nomdok,int kodm,int nk,
int sklad1,
const char *nomdokp,
GtkWidget *wpredok)
{
char strsql[512];
class kzvmu2_data data;
iceb_u_str kikz;
iceb_u_str naim_mat;
SQLCURSOR cur;
SQL_str   row;
iceb_u_str repl;

float pnds=iceb_pnds(dd,md,gd,wpredok);

data.dd=dd;
data.md=md;
data.gd=gd;
data.sklad=skl;
data.kodm=kodm;
data.nom_kar=nk;
data.nomdok.plus(nomdok);
data.sklad1=sklad1;
data.nomdokp.plus(nomdokp);
naim_mat.plus("");

sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naim_mat.new_plus(row[0]); 


sprintf(strsql,"select kolih,cena,shet,nomz,ktoi,vrem,nomkarp,mi,kdstv from Dokummat1 where \
sklad=%d and nomd='%s' and kodm=%d and nomkar=%d and datd='%04d-%02d-%02d' \
and tipz=2",skl,nomdok,kodm,nk,gd,md,dd);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
 {
  repl.new_plus(gettext("Не найдена запись для корректировки !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

data.kolihz=atof(row[0]);
sprintf(strsql,"%.10g",data.kolihz);

data.kolih.plus(strsql);

data.shet.plus(row[2]);
data.nomerzak.plus(row[3]);
double cena=atof(row[1]);
sprintf(strsql,"%.10g",cena);

data.ktoz=atoi(row[4]);

data.cenabnds.plus(strsql);

cena=cena+cena*pnds/100.;
cena=iceb_u_okrug(cena,okrg1);
sprintf(strsql,"%.10g",cena);
data.cenasnds.plus(strsql);

kikz.plus(iceb_kikz(row[4],row[5],wpredok));   
data.nomkarp=atoi(row[6]);
if(atoi(row[7]) == 1)
 data.metka_imp_tov.plus("X");
else
 data.metka_imp_tov.plus("");
data.kdstv.plus(row[8]);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка выбранной записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kzvmu2_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
repl.new_plus(gettext("Корректировка выбранной записи"));

repl.ps_plus(gettext("Материал"));
repl.plus(":");
repl.plus(kodm);
repl.plus(" ");
repl.plus(naim_mat.ravno());
if(kikz.getdlinna() > 1)
 repl.ps_plus(kikz.ravno());

GtkWidget *label=gtk_label_new(repl.ravno());


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(strsql);
data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

sprintf(strsql,"%s",gettext("Цена с НДС"));
label=gtk_label_new(strsql);
data.entry[E_CENASNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENASNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), data.entry[E_CENASNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENASNDS], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENASNDS]),data.cenasnds.ravno());
gtk_widget_set_name(data.entry[E_CENASNDS],iceb_u_inttochar(E_CENASNDS));


sprintf(strsql,"%s",gettext("Цена без НДС"));
label=gtk_label_new(strsql);
data.entry[E_CENABNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENABNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENABNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENABNDS]), data.entry[E_CENABNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENABNDS], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENABNDS]),data.cenabnds.ravno());
gtk_widget_set_name(data.entry[E_CENABNDS],iceb_u_inttochar(E_CENABNDS));

sprintf(strsql,"%s",gettext("Счёт"));
label=gtk_label_new(strsql);
data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s",gettext("Номер заказа"));
label=gtk_label_new(strsql);
data.entry[E_NOMERZAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMERZAK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERZAK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERZAK]), data.entry[E_NOMERZAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMERZAK], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERZAK]),data.nomerzak.ravno());
gtk_widget_set_name(data.entry[E_NOMERZAK],iceb_u_inttochar(E_NOMERZAK));

sprintf(strsql,"%s (X)",gettext("Признак импорта"));
label=gtk_label_new(strsql);
data.entry[E_METKA_IMP_TOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_METKA_IMP_TOV]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_IMP_TOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_IMP_TOV]), data.entry[E_METKA_IMP_TOV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_METKA_IMP_TOV], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_METKA_IMP_TOV]),data.metka_imp_tov.ravno());
gtk_widget_set_name(data.entry[E_METKA_IMP_TOV],iceb_u_inttochar(E_METKA_IMP_TOV));

sprintf(strsql,"%s",gettext("Код вида деятельности сельскохозяйственного товаропроизводителя"));
label=gtk_label_new(strsql);
data.entry[E_KDSTV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KDSTV]),32);
gtk_box_pack_start (GTK_BOX (hbox[E_KDSTV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KDSTV]), data.entry[E_KDSTV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KDSTV], "activate",G_CALLBACK(kzvmu2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDSTV]),data.kdstv.ravno());
gtk_widget_set_name(data.entry[E_KDSTV],iceb_u_inttochar(E_KDSTV));


sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введенную в меню информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(kzvmu2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(kzvmu2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kzvmu2_v_key_press(GtkWidget *widget,GdkEventKey *event,class kzvmu2_data *data)
{

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
void  kzvmu2_v_knopka(GtkWidget *widget,class kzvmu2_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:

    if( kzvmu2_zap(data) != 0)
     return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    kzvmu2_v_vvod(GtkWidget *widget,class kzvmu2_data *data)
{
iceb_u_str repl;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENASNDS:
    data->cenasnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENABNDS:
    data->cenabnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMERZAK:
    data->nomerzak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*****************************/
/*Запись*/
/*********************/

int kzvmu2_zap(class kzvmu2_data *data)
{
data->read_rek();
//printf("kzvmu2_zap\n");

short		mnz=0; /*0- не подтверждено 
                       1-подтверждено одной записью на все количество
                       2-подтверждено не на все количество
                       */
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
iceb_u_str repl;

if(mu_pvkuw(data->ktoz,data->window) != 0)
 return(1);

/*Проверяем подтверждена ли запись в карточке*/
sprintf(strsql,"select kolih from Zkart where sklad=%d and nomk=%d and \
nomd='%s' and datd='%04d-%02d-%02d'",
data->sklad,data->nom_kar,data->nomdok.ravno(),data->gd,data->md,data->dd);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  if(fabs(atof(row[0]) - data->kolihz) < 0.00001)
    mnz=1;
  else
    mnz=2;    
 }

if(data->nom_kar != 0)
 {
   /*Читаем подтвержденное количество*/
//  double bb=ATOFM(VV.VVOD_SPISOK_return_data(0));
   double kolp=readkolkw(data->sklad,data->nom_kar,data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);  

   if(mnz == 0)
   if(data->kolih.ravno_atof()-kolp < 0.000001)
    {
     sprintf(strsql,"%s (%.10g > %.10g)",
     gettext("Уже подверждено в карточке больше !"),
     data->kolih.ravno_atof(),kolp);
     iceb_menu_soob(strsql,data->window);
     return(1);
     
    }
   if(mnz != 0)
    {
     short d,m,g;
     iceb_u_poltekdat(&d,&m,&g);
     class ostatok ost;     
     ostkarw(1,1,g,31,m,g,data->sklad,data->nom_kar,&ost);
     if(ost.ostg[3] + kolp - data->kolih.ravno_atof()  < -0.00000000009)
       {
        repl.new_plus(gettext("Отрицательный остаток в карточке. Корректировка не возможна !"));
        iceb_menu_soob(&repl,data->window);
        return(1);
       } 
    }
 }


if(data->shet.getdlinna() > 1)
 {
  OPSHET shetv;
  if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
    return(1);
 }

double cena=data->cenabnds.ravno_atof();
cena=iceb_u_okrug(cena,okrcn);
time_t vrem;

time(&vrem);

if(data->sklad1 != 0 && data->nomdokp.getdlinna() > 1)
 {
 //Корректируем в парном документе
  sprintf(strsql,"update Dokummat1 \
set \
kolih=%.10g,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d and tipz=1",
   data->kolih.ravno_atof(),iceb_getuid(data->window),vrem,
   data->gd,data->md,data->dd,data->sklad1,data->nomdokp.ravno(),data->kodm,data->nomkarp);

  if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);

  //Проверка подтверждения документа
  podvdokw(data->dd,data->md,data->gd,data->nomdokp.ravno(),data->sklad1,data->window);
        
 }

int metka_imp_tov=0;
if(data->metka_imp_tov.getdlinna() > 1)
 metka_imp_tov=1;
 
sprintf(strsql,"update Dokummat1 \
set \
kolih=%.10g,\
cena=%.10g,\
ktoi=%d,\
vrem=%ld,\
shet='%s',\
nomz='%s',\
mi=%d,\
kdstv='%s' \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
data->kolih.ravno_atof(),cena,iceb_getuid(data->window),vrem,data->shet.ravno_filtr(),data->nomerzak.ravno_filtr(),
metka_imp_tov,
data->kdstv.ravno_filtr(),
data->gd,data->md,data->dd,data->sklad,data->nomdok.ravno_filtr(),data->kodm,data->nom_kar);
//printf("%s\n",strsql);
if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);

/*Проверяем если подтверждено все количество то исправляем в  карточке тоже */

if(data->nom_kar == 0)
  return(0);

int kolstr=0;
   
sprintf(strsql,"select kolih from Zkart \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d and nomk=%d",
data->gd,data->md,data->dd,data->nomdok.ravno_filtr(),data->sklad,data->nom_kar);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 1)
 {
  cur.read_cursor(&row);
  double bb=atof(row[0]);
  if(fabs(bb-data->kolihz) < 0.0001)
   {
    sprintf(strsql,"update Zkart \
set \
kolih=%.10g,\
cena=%.10g \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d and nomk=%d",
    data->kolih.ravno_atof(),cena,
    data->gd,data->md,data->dd,data->nomdok.ravno_filtr(),data->sklad,data->nom_kar);

    if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);
   }
 }     

return(0);

}
